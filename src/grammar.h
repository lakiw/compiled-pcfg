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


#ifndef _GRAMMAR_H
#define _GRAMMAR_H


// The maximum length of a terminal. Aka D32
// Needs to be one more than the desired length since the index starts at 0,
// and I don't want to have to fix-up the D32 = D31 when using this
#define MAX_TERM_LENGTH 33


// Contains the replacements that all have the same probabability
//
typedef struct PcfgReplacements {
    
    // The number of items for this group
    int size;
    
    // The probabilities for this terminal
    float prob;
    
    // The parent of this replacement group
    struct PcfgReplacements *parent;
    
    // The child of this replacement group
    struct PcfgReplacements *child;
    
    // The values for this terminal
    char *value[][MAX_TERM_LENGTH];
        
}PcfgReplacements;


// Top level structure that contains the PCFG
typedef struct PcfgGrammar {
   
    PcfgReplacements *a[MAX_TERM_LENGTH];
    PcfgReplacements *d[MAX_TERM_LENGTH];
    PcfgReplacements *o[MAX_TERM_LENGTH][MAX_TERM_LENGTH];
    PcfgReplacements *k[MAX_TERM_LENGTH];
    PcfgReplacements *x[MAX_TERM_LENGTH];
    PcfgReplacements *y[MAX_TERM_LENGTH];
    PcfgReplacements *c[MAX_TERM_LENGTH];
    PcfgReplacements *m[MAX_TERM_LENGTH];
    
}PcfgGrammar;


#endif