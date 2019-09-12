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

#include "grammar_io.h"


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


// Loads the grammar for a particular terminal
//
// Function returns a non-zero value if an error occurs
//     1 = problem opening the file or malformed ruleset
//
int load_terminal(char *config_filename, char *structure) {
    
    // Get the filenames associated with the structure
    char result[256][MAX_CONFIG_ITEM];
    int list_size;
    if (config_get_list(config_filename, structure, "filenames", result, &list_size, 256) != 0) {
        fprintf(stderr, "Error reading the rules file. Exiting\n");
        return 1;
	}

    return 0;
}


// Loads a ruleset/grammar from disk
//
// Function returns a non-zero value if an error occurs
//     1 = problem opening the file
//     2 = malformed ruleset
//     3 = unsupported feature/ruleset
//
int load_grammar(char *arg_exec, struct program_info program_info, PcfgGrammar *pcfg) {
    
    // Directory the executable is running in
    // Adding in a plus 1 to PATH_MAX to deal with a potential edge case
    // when formatting this to represent the directory. Should never happen
    // but still...
    char exec_directory[PATH_MAX + 1];
    strncpy(exec_directory, arg_exec, PATH_MAX);
    
    // Hate doing it this way but haven't found a good way to get the directory
    char *tail_slash = strrchr(exec_directory, SLASH);
    
    // Holds the return value of function calls
    int ret_value;
    
    // Using the current directory
    if (tail_slash == NULL) {
        snprintf(exec_directory, PATH_MAX, ".%c", SLASH);
    }
    // Using a directory passed in via argv[0]
    else {   
        tail_slash[1]= '\0';
    }
    
    // Create the base directory to load the rules from
    char base_directory[FILENAME_MAX];
    snprintf(base_directory, FILENAME_MAX, "%s%cRules%c%s%c", exec_directory, SLASH, SLASH, program_info.rule_name, SLASH);
    
    fprintf(stderr, "Loading Ruleset:%s\n",base_directory);
    
    // Save the config file name for easy reference
    char config_filename[FILENAME_MAX];
    snprintf(config_filename, FILENAME_MAX, "%sconfig.ini", base_directory);
    
    // Currently we only support UTF-8 for the compiled version, so ensure
    // the ruleset uses that
    //
    // Note: The Python version of the pcfg_guesser supports other encoding
    //       types.
    ret_value = check_encoding(config_filename);
    if (ret_value != 0) {
        return ret_value;
    }
    
    // Read the rules filelength
    char result[256][MAX_CONFIG_ITEM];
    int list_size;
    if (load_terminal(config_filename, "BASE_A") != 0) {
        fprintf(stderr, "Error reading the rules file. Exiting\n");
        return 1;
	}
    
    return 0;
}