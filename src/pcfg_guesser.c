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
    
    
    // Intiazlie the grammar and Priority Queue
    if (load_grammar(argv[0], program_info, &pcfg) != 0) {
        fprintf(stderr, "Error loading ruleset. Exiting\n");
        return 0;
	}

    fprintf(stderr, "Initailizing the Priority Queue\n");
    priority_queue_t* pq;

    initialize_pcfg_pqueue(&pq, &pcfg);
    
    fprintf(stderr, "Starting to generate guesses\n");

    // Start generating guesses
    while (!priority_queue_empty(pq)) {
        PQItem* pq_item = pcfg_pq_pop(pq);
        if (pq_item == NULL) {
            printf("Memory allocation error when popping item from pqueue\n");
            return 1;
        }
        //printf("Popped: %e\n",pq_item->prob);
        free(pq_item->pt);
        free(pq_item);
    }


	return 0;
}
