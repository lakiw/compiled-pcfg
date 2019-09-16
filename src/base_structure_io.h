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

#ifndef _BASE_STRUCTURE_IO_H
#define _BASE_STRUCTURE_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "grammar.h"
#include "config_parser.h"
#include "helper_io.h"


// Splits up a base structure string, and allocates an array of BaseReplace
//
// Aka turns A4D3 into C(4)->A(4)->D(3)
//
// Function returns a non-zero value if an error occurs
//     1 = Base structuer not supported
//     2 = Error occured processing the input
//
extern int split_base(char* input, BaseReplace* base, int *list_size);


// Loads the grammar for base structures
extern int load_base_structures(char *config_filename, char *base_directory, PcfgBase *base_structures); 


#endif