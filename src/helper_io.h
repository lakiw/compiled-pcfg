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

#ifndef _HELPER_IO_H
#define _HELPER_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "config_parser.h"


// Checks the encoding of a ruleset
extern int check_encoding(char *config_filename);

// Splits an input line into a value, prob pair
extern int split_value(char *input, char *value, double *prob);

#endif