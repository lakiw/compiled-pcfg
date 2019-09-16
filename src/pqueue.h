//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Pretty Cool Fuzzy Guesser (PCFG)
//  --Probabilistic Context Free Grammar (PCFG) Password Guessing Program
//
//  Written by Matt Weir
//  Original code taken from stackexchange user arcomber
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

#ifndef _PQUEUE_H
#define _PQUEUE_H

#include <stdlib.h>

/*
Heap ordered priority queue storing in a resizable array
*/

struct priority_queue;
typedef struct priority_queue priority_queue_t;

/* priority_queue_init initialises the priority queue and returns a handle which 
must be passed to subsequent priority_queue_xxx functions..  Argument is the
comparison function.  This comparison function must return a negative value if
the first argument is less than the second, a positive integer value if the 
first argument is greater than the second, and zero if the arguments are equal.
The function must also not modify the objects passed to it.  The meaning of
greater or less can be reversed. */
priority_queue_t* priority_queue_init(int(*compare)(const void* element1, const void* element2));
/* priority_queue_free frees memory used by priority queue. init in constant time */
void priority_queue_free(priority_queue_t* pq);
/* returns 1 if the queue is empty, 0 otherwise. constant time */
int priority_queue_empty(const priority_queue_t* pq);
/* insert an object into the priority queue. insert in logarithmic time */
void priority_queue_insert(priority_queue_t* pq, void* el);
/* pops the 'top' element and removes from the priority queue. pop in logarithmic time */
void* priority_queue_pop(priority_queue_t* pq);
/* returns the top element but does not remove from priority queue. top in constant time */
void* priority_queue_top(const priority_queue_t* pq);
/* returns number of elements in priority queue. constant time */
int priority_queue_size(const priority_queue_t* pq);

#endif // PRIORITY_QUEUE_