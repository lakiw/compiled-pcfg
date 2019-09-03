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

#include "command_line.h"


//   OPTIONS.  Field 1 in ARGP.
//   Order of fields: {NAME, KEY, ARG, FLAGS, DOC}.
static struct argp_option options[] =
{
    {"rule_name",  'r', "OUTFILE", 0, "The ruleset to use. Default is: 'Default'"},
    {"debug", 'd', 0, 0, "Prints out debugging info vs guesses."},
    {0}
};


//   PARSER. Field 2 in ARGP.
//   Order of parameters: KEY, ARG, STATE.
static error_t parse_opt (int key, char *arg, struct argp_state *state){
    struct program_info *program_info = state->input;

    switch (key) {
        case 'd':
            program_info->debug = 1;
            break;
        case 'r':
            program_info->rule_name = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}


// ARGS_DOC. Field 3 in ARGP.
// A description of the non-option command-line arguments that we accept.
static char args_doc[] = "ARG1 ARG2";


// DOC.  Field 4 in ARGP.
// Program documentation.
static char doc[] =
  "Pretty Cool Fuzzy Guesser: \v A program for generationg password guesses";


// The ARGP structure itself.
static struct argp argp = {options, parse_opt, args_doc, doc};


int parse_command_line(int argc, char **argv, struct program_info *program_info) {
    const char *argp_program_version = VERSION;
	
    // Set argument defaults
    program_info->rule_name = "Default";
    program_info->debug = 0;
    
    argp_parse(&argp, argc, argv, 0, 0, program_info);
    
    printf("SELECTED RULE NAME: %s\n",program_info->rule_name);
    
    return 0;
}