//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Pretty Cool Fuzzy Guesser (PCFG)
//  --Probabilistic Context Free Grammar (PCFG) Password Guessing Program
//
//  Written by Matt Weir
//  Special thanks to the John the Ripper and Hashcat communities where some 
//  of the code was copied from. And thank you whoever is reading this. Be good!
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

#include "pcfg_guesser.h"


// The main program
int main(int argc, char *argv[]) {
	
	// Holds the info from the command line
	struct program_info program_info;

    // Parse the command line
	if (parse_command_line(argc, argv, &program_info) != 0) {
		fprintf(stderr, "Error, parsing command line. Exiting\n");
        return 0;
	}
    
    // Print the startup banner
    print_banner(program_info.version);
    
    // Create the empty grammar
    PcfgGrammar pcfg;
    
    load_grammar(argv[0], program_info, &pcfg);
    
    priority_queue_t* pq = priority_queue_init(descending);
    
    printf("size of pq now = %d\n", priority_queue_size(pq));
    int weights[] = { 14,8,15,16,11,1,12,13,4,10,9,3,5,7,2,6,6,6 };
    int size = sizeof(weights) / sizeof(weights[0]);

    // insert each one into priority queue
    for (int i = 0; i < size; ++i) {
        element* el = malloc(sizeof(element));

        // generate string
        char buffer[20];
        sprintf(buffer, "added no: %d", i + 1);
        el->data = malloc(strlen(buffer) + 1);
        strcpy(el->data, buffer);
        el->weight = weights[i];
        priority_queue_insert(pq, el);
    }
    printf("size of pq now = %d\n", priority_queue_size(pq));
    element* el = malloc(sizeof(element));
    el->weight = 22;
    el->data = "hi guys";
    priority_queue_insert(pq, el);
    printf("size of pq now = %d\n", priority_queue_size(pq));

    const element* top = priority_queue_top(pq);
    printf("peek of top item: %d %s\n", top->weight, top->data);

    while (!priority_queue_empty(pq)) {
        element* top = priority_queue_pop(pq);
        printf("top is: %d %s\n", top->weight, top->data);
        free(top);
    }
    printf("size of pq now = %d\n", priority_queue_size(pq));
    priority_queue_free(pq);

    // try using different data/comparator
    pq = priority_queue_init(ascending);
    edge* e1 = malloc(sizeof(edge));
    e1->vertex = 0;
    e1->weight = 1;
    priority_queue_insert(pq, e1);
    edge* e2 = malloc(sizeof(edge));
    e2->vertex = 1;
    e2->weight = 3;
    priority_queue_insert(pq, e2);

    edge* e3 = malloc(sizeof(edge));
    e3->vertex = 2;
    e3->weight = 3;  // same weight
    priority_queue_insert(pq, e3);

    while (!priority_queue_empty(pq)) {
        edge* top = priority_queue_pop(pq);
        printf("top is: %d %d\n", top->weight, top->vertex);
        free(top);
    }
    printf("size of pq now = %d\n", priority_queue_size(pq));
    priority_queue_free(pq);

	return 0;
}
