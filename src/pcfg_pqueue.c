//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Pretty Cool Fuzzy Guesser (PCFG)
//  --Probabilistic Context Free Grammar (PCFG) Password Guessing Program
//
//  Written by Matt Weir
//  Original backend priority queue code taken from stackexchange user arcomber
//  https://codereview.stackexchange.com/questions/186670/priority-queue-implementation-in-c-based-on-heap-ordered-resizable-array-tak
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


// Pops the 'top' element and removes from the priority queue.
// Will also add children of that item using the deadbeat dad "next" algorithm
void* pcfg_pq_pop(priority_queue_t* pq) {

    PQItem *pq_item = priority_queue_pop(pq);
    
    // Generate potential children
    for (int i = 0; i< pq_item->size; i++) {
        if (pq_item->pt[i]->child != NULL) {
            
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
            
            free(child->pt);
            free(child);
 
            // Advance the parse tree of the child so it is an actual child
            // of the parent
            if (child->pt[i]->child != NULL) {
                //printf("child : %e\n",child->pt[i]->prob);
                //child->pt[i] = child->pt[i]->child;
            }
              
            //calculate the probability of the pq_item
            //calculate_prob(child);
            //free(child->pt);
            //free(child);
            // Push it into the queue
            //priority_queue_insert(pq, child);
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
                pq_item->pt[i] = &pcfg->alpha[cur_base->value->id];
            }
            else if (strncmp(type,"C",10) == 0) {
                pq_item->pt[i] = &pcfg->capitalization[cur_base->value->id];
            }
            else if (strncmp(type,"D",10) == 0) {
                pq_item->pt[i] = &pcfg->digits[cur_base->value->id];
            }
            else if (strncmp(type,"Y",10) == 0) {
                pq_item->pt[i] = &pcfg->years[cur_base->value->id];
            }
            else if (strncmp(type,"O",10) == 0) {
                pq_item->pt[i] = &pcfg->other[cur_base->value->id];
            }
            else if (strncmp(type,"K",10) == 0) {
                pq_item->pt[i] = &pcfg->keyboard[cur_base->value->id];
            }
            else if (strncmp(type,"X",10) == 0) {
                pq_item->pt[i] = &pcfg->x[cur_base->value->id];
            }
            else {
                return 1;
            }       
        }
        
        //calculate the probability of the pq_item
        calculate_prob(pq_item);
        
        // Push it into the queue
        priority_queue_insert((*pq), pq_item);

        cur_base = cur_base->next;
        
    }
    
    return 0;
}