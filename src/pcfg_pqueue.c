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