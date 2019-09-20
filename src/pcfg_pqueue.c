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
    const element* element1 = a;
    const element* element2 = b;
    if (element1->weight < element2->weight)
        return -1;
    if (element1->weight > element2->weight)
        return 1;

    return 0;
}


int ascending(const void* a, const void* b) {
    const edge* edge1 = a;
    const edge* edge2 = b;
    if (edge2->weight < edge1->weight)
        return -1;

    if (edge2->weight > edge1->weight)
        return 1;

    return 0;
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

        for (int i = 0; i< cur_base->size; i++) {
            printf("(%s:%i) ", cur_base->value[i].type, cur_base->value[i].id);
        }
        printf("\n");
        cur_base = cur_base->next;
        
    }
    
    return 0;
}