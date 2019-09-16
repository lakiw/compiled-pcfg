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


#include "pqueue.h"


typedef int(*compare)(const void* element1, const void* element2);

struct priority_queue {
    int capacity;
    int n;
    void** array;
    compare cmp;
};

static const int initial_size = 16;

static void swap(priority_queue_t* pq, int index1, int index2) {
    // shallow copy of pointers only
    void* tmp = pq->array[index1];
    pq->array[index1] = pq->array[index2];
    pq->array[index2] = tmp;
}


static void rise(priority_queue_t* pq, int k) {
    while (k > 1 && pq->cmp(pq->array[k / 2], pq->array[k]) < 0) {
        swap(pq, k, k / 2);
        k = k / 2;
    }
}

static void fall(priority_queue_t* pq, int k) {
    while (2 * k <= pq->n) {
        int child = 2 * k;
        
        if (child < pq->n && pq->cmp(pq->array[child], pq->array[child + 1]) < 0) {
            child++;
        }

        if (pq->cmp(pq->array[k], pq->array[child]) < 0) {
            swap(pq, k, child);
        }
        k = child;
    }
}

static void** array_resize(void** array, int newlength) {
    /* reallocate array to new size
       this is problematic because realloc may fail and return NULL
       in which case there is a leak because array is still allocated
       but not returned so cannot be free'd */
    return realloc(array, newlength * sizeof(void*));
}

priority_queue_t* priority_queue_init(int(*compare)(const void* element1, const void* element2)) {
    priority_queue_t* pq = malloc(sizeof(priority_queue_t));
    pq->array = NULL;
    pq->capacity = 0;
    pq->n = 0;
    pq->cmp = compare;
    return pq;
}

void priority_queue_free(priority_queue_t* pq) {
    free(pq->array);
    free(pq);
}

int priority_queue_empty(const priority_queue_t* pq) {
    return pq->n == 0;
}

void priority_queue_insert(priority_queue_t* pq, void* el) {

    if (pq->capacity == 0) {
        pq->capacity = initial_size;
        pq->array = array_resize(pq->array, pq->capacity + 1);
    }
    else if (pq->n == pq->capacity) {
        pq->capacity *= 2;
        // we need to resize the array
        pq->array = array_resize(pq->array, pq->capacity + 1);
    }

    // we always insert at end of array
    pq->array[++pq->n] = el;
    rise(pq, pq->n);
}

void* priority_queue_pop(priority_queue_t* pq) {

    // reduce array memory use if appropriate
    if (pq->capacity > initial_size && pq->n < pq->capacity / 4) {
        pq->capacity /= 2;
        pq->array = array_resize(pq->array, pq->capacity + 1);
    }

    void* el = pq->array[1];
    swap(pq, 1, pq->n--);
    pq->array[pq->n + 1] = NULL;  // looks tidier when stepping through code - not really necessary
    fall(pq, 1);
    return el;
}

void* priority_queue_top(const priority_queue_t* pq) {
    return pq->array[1];
}

int priority_queue_size(const priority_queue_t* pq) {
    return pq->n;
}