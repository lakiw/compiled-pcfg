//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Pretty Cool Fuzzy Guesser (PCFG)
//  --Probabilistic Context Free Grammar (PCFG) Password Guessing Program
//
//  Written by Matt Weir
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//

#include "pcfg_pqueue.h"


int descending(const void* a, const void* b) {
    const PQItem* element1 = a;
    const PQItem* element2 = b;
    if (element1->prob < element2->prob)
        return -1;
    if (element1->prob > element2->prob)
        return 1;

    return 0;
}


// Calculates and sets the probability of a pq_item
//
void calculate_prob(PQItem *pq_item) {
    
    // Initalize the probability to be that of the base structure
    pq_item->prob = pq_item->base_prob;

    // Multiply the probability by all of the other transitions
    for (int i=0; i < pq_item->size; i++) {
        pq_item->prob *= pq_item->pt[i]->prob;
    }
    return;
}


// Calculates a parse tree's probability
//
// Note, this is slightly lighter weight than the calculate_prob function
// since it doesn't have to take into account the base_probability
double calc_pt_prob(PcfgReplacements **pt, int size) {
    
    double prob = 1.0;
    
    for (int i = 0; i < size; i++) {
        prob *= pt[i]->prob;
    }
    return prob;
}


// Checks to see if the current parent should handle this child, or if
// there is a lower probability parent still in the PQ that will handle
// the child later
//
// Returns 0 if this parent is responsible for this child
//
// Returns 1 if this parent is not responsible
//
int is_this_my_child(int parent_id , PQItem *parent_pq) {
    
    // Not sure if it makes sense to recalculate the popped parent's prob
    // without the base_structure prob so that doesn't need to be included in
    // potential parent calculations, or to include the base_structure prob in
    // all comparisions.
    //
    // Going with coming up with a stripped down version of the parent's prob
    // for comparisions now
    double popped_parent_prob = calc_pt_prob(parent_pq->pt, parent_pq->size);
    
    // Create the potential child's parse tree       
    PcfgReplacements *child_pt[parent_pq->size];
    for (int i=0; i<parent_pq->size; i++) {
        child_pt[i] = parent_pq->pt[i];
    }
    // Advance the item for the child to make it a child of the parent
    if (child_pt[parent_id]-> child == NULL) {
        // Shouldn't happen, but it doesn't hurt to do some sanity checking
        return 2;
    }   
    child_pt[parent_id] = child_pt[parent_id]->child;

    // Now generate the other parents and see if they are lower probabilty
    // than the current parent
    for (int i=0; i<parent_pq->size; i++) {
        // Skip the current parent
        if (i == parent_id) {
            continue;
        }
        
        // Skip if there isn't a parent for this position
        if (child_pt[i]->parent == NULL) {
            continue;
        }
        
        // Check this parent's probability
        // Modify the child's pt to be the new parent
        child_pt[i] = child_pt[i]->parent;
        
        //Calculate the new parent's probability
        double new_parent_prob = calc_pt_prob(child_pt, parent_pq->size);
        
        // If the new parent's probability is less than the current parent,
        // then this child is the new parent's responsibilty
        if (new_parent_prob < popped_parent_prob) {
            return 1;
        }
        
        // If the parent's probability is equal, the one to the leftmost is
        // responsible. It's arbitrary, but we need a tiebreaker
        if ((new_parent_prob == popped_parent_prob) && (i < parent_id)) {
            return 1;
        }
        
        // The new parent is not responsible for this child, so reset the child
        // to be itself
        child_pt[i] = child_pt[i]->child;
    }
               
    return 0;
}


// Pops the 'top' element and removes from the priority queue.
// Will also add children of that item using the deadbeat dad "next" algorithm
void* pcfg_pq_pop(priority_queue_t* pq) {

    PQItem *pq_item = priority_queue_pop(pq);

    // Generate potential children
    for (int i = 0; i< pq_item->size; i++) {
        
        // There is a potential child at this position
        if (pq_item->pt[i]->child != NULL) {
            
            // Check if this parent should handle this child, and if not
            // skip to checking the next child.
            if (is_this_my_child(i,pq_item) != 0) {
                continue;
            }
            
            // This is a child this parent needs to take care of
            //
            // Create the child and insert it into the queue
            PQItem *child = malloc(sizeof(PQItem));
            if (child == NULL) {
                return NULL;
            }
            
            child->base_prob = pq_item->base_prob;
            child->size = pq_item->size;  
              
            //Allocate the size for the parse trees and fill them out
            child->pt = malloc(child->size * sizeof(PcfgReplacements *));
            if (child->pt == NULL) {
                return NULL;
            }   

            // Map the partent's parse tree onto the child'sbrk
            for (int y = 0; y< pq_item->size; y++) {
                child->pt[y] = pq_item->pt[y];
            }
            // Advance the parse tree of the child so it is an actual child
            // of the parent
            child->pt[i] = child->pt[i]->child;
            
            //calculate the probability of the pq_item
            calculate_prob(child);
            // Push it into the queue
            priority_queue_insert(pq, child);
        }
    }
            
    return pq_item;
}
 


// Initialize a PCFG PQueue structure from a PCFG Grammar
//
// Returns 0 on successful compleation
//
// Returns 1 if an error occured
// 
int initialize_pcfg_pqueue(priority_queue_t **pq, PcfgGrammar *pcfg) {
    
    // Initialize the priority queue itself
    (*pq) = priority_queue_init(descending);
    
    // Generate all the initial pre-terminals and push them into the pqueue
    
    // Keeps track of the current base structure that is being processed
    PcfgBase *cur_base = pcfg->base_structures;
    
    while (cur_base != NULL) {

        // Create the inital pq_item
        PQItem *pq_item = malloc(sizeof(PQItem));
        if (pq_item == NULL) {
            return 1;
        }
        pq_item->base_prob = cur_base->prob;
        pq_item->size = cur_base->size;
        
        //Allocate the size for the parse trees and fill them out
        pq_item->pt = malloc(cur_base->size * sizeof(PcfgReplacements *));
        if (pq_item->pt == NULL) {
            return 1;
        }        

        for (int i = 0; i< cur_base->size; i++) {
            
            // Need to find the right pointer based on the type
            char *type = cur_base->value[i].type;
            if (strncmp(type,"A",10) == 0) {    
                pq_item->pt[i] = pcfg->alpha[cur_base->value[i].id];
            }
            else if (strncmp(type,"C",10) == 0) {
                pq_item->pt[i] = pcfg->capitalization[cur_base->value[i].id];
            }
            else if (strncmp(type,"D",10) == 0) {
                pq_item->pt[i] = pcfg->digits[cur_base->value[i].id];
            }
            else if (strncmp(type,"Y",10) == 0) {
                pq_item->pt[i] = pcfg->years[cur_base->value[i].id];
            }
            else if (strncmp(type,"O",10) == 0) {
                pq_item->pt[i] = pcfg->other[cur_base->value[i].id];
            }
            else if (strncmp(type,"K",10) == 0) {
                pq_item->pt[i] = pcfg->keyboard[cur_base->value[i].id];
            }
            else if (strncmp(type,"X",10) == 0) {
                pq_item->pt[i] = pcfg->x[cur_base->value[i].id];
            }
            else {
                return 1;
            }       
        }

        calculate_prob(pq_item);
        
        // Push it into the queue
        priority_queue_insert((*pq), pq_item);

        cur_base = cur_base->next;
        
    }
    
    return 0;
}