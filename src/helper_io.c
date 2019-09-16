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

#include "helper_io.h"


// Checks the version the ruleset was created and makes sure it is supported
//
// Function returns a non-zero value if an error occurs
//     1 = problem opening the file
//     2 = malformed ruleset
//     3 = unsupported feature/ruleset
//
int check_encoding(char *config_filename) {
    
    char result[MAX_CONFIG_LINE];
    
    int ret_value = get_key(config_filename, "TRAINING_DATASET_DETAILS", "encoding", result);
    
    switch (ret_value) {
        // Version was found
        case 0:
            break;
        case 1:
            fprintf(stderr, "Error opening config file:%s\n",config_filename);
            return 1;
        case 2:
            fprintf(stderr, "Malformed config file:%s\n",config_filename);
            return 2;
        default:
            return ret_value;
    }
    
    // It is utf-8 or ASCII encoding so it is supported
    if ((strncmp(result, "utf-8", MAX_CONFIG_LINE) == 0) || (strncmp(result,"ascii", MAX_CONFIG_LINE) == 0)){
        return 0;
    }
    
    fprintf(stderr, "Unfortunatly, only UTF-8 or ASCII rulesets are currently supported by the compiled pcfg_guesser\n");
    fprintf(stderr, "Note: The Python pcfg_guesser supports other encoding schemes\n");
    fprintf(stderr, "Detected Encodign: %s\n", result);
    return 3;
}


// Splits an input line into a value, prob pair
//
// Input: Note, memory needs to be allocated by the calling program
//
//     input: the string that needs to be split_value
//
//     value: contains the return value string (must be allocated by calling program)
//
//     prob: contains the probability from the split value (must be allocated by calling program)
//
// Function returns a non-zero value if an error occurs
//     1 = Problem splitting the input
//
int split_value(char *input, char *value, double *prob) {
    
    //find the split point
    char *split_point = strchr(input,'\t');
        
    //If there isn't a tab to split the input up
    if (split_point == NULL) {
        return 1;
    }
        
    (*prob) = strtod(split_point,NULL);
        
    // Check to make sure it was a number
    if ((errno == EINVAL) || (errno == ERANGE))
    {
        fprintf(stderr, "Invalid probability found. Exiting\n");
        return 1;
    }
     
    // Make sure the id falls within the acceptable range
    if ( ((*prob) < 0.0) || ((*prob) > 1.0) ) {
        fprintf(stderr, "Invalid probability found in rules. Exiting\n");
        return 1;
    }      

    //Assign the value
    strncpy(value, input, split_point-input);
    value[split_point-input] = '\0';
    
    return 0;
}