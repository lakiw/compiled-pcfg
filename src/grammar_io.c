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


// Opens a file and actually loads the grammar for a particular terminal
//
// Function returns a non-zero value if an error occurs
//     1 = problem opening the file or malformed ruleset
//
int load_term_from_file(char *filename, PcfgReplacements *terminal_pointer) {
    
    // Pointer to the open terminal file
    FILE *fp;
    
    // Open the terminal file
    fp= fopen(filename,"r");
    
    // Check to make sure the file opened correctly
    if (fp== NULL) {
        
        //Could not open the file. Print error and return an error
        fprintf(stderr, "Error. Could not read the file: %s\n",filename);
        return 1;
    }
    
    // Note: There will be two passes through the file
    //   1) First pass gets count info since I want to allocate memory without
    //      being wasteful, and I want to use arrays to hopefully make
    //      generating guesses faster
    //
    //   2) Actually load the terminals to the pcfgreplacement struct
    //
    
    // Holds the current line in the config file
    char buff[MAX_CONFIG_LINE];
    
    // Initalize things and get the first line of the file
    if (!fgets(buff, MAX_CONFIG_LINE , (FILE*)fp)) {
        // The file should have at least one line in it
        return 1;
    }
    
    // This contains the number of items at the current probability
    int num_term = 1;
    
    // The previous probability we saw
    double previous_prob;
    
    // The probability of the current item;
    double prob;
    
    // A temp holder for the string value in the file
    char value[MAX_CONFIG_LINE];
    
    // Break up the first line. Using previous_prob since this will be the
    // inital value for it when comparing against other items.
    if (split_value(buff, value, &previous_prob) != 0) {
        return 1;
    }
    
    // Initalize the first PcfgReplacement
    if ((terminal_pointer = malloc(sizeof(struct PcfgReplacements))) == NULL) {
        return 1;
    } 
    terminal_pointer->parent = NULL;
    terminal_pointer->child = NULL;
    terminal_pointer->prob = previous_prob;
    
    PcfgReplacements *cur_pointer = terminal_pointer;
        
    // Loop through the config file
    while (fgets(buff, MAX_CONFIG_LINE , (FILE*)fp)) {
        
        if (split_value(buff, value, &prob) != 0) {
            return 1;
        }
        
        if (prob == previous_prob) {
            num_term++;
        }
        else {
            // Finalize this structure
            cur_pointer->size = num_term;
            if ((cur_pointer->value = (char **) malloc(num_term * sizeof(char *))) == NULL) {
                return 1;
            }

            // intialize the next, and make it current
            previous_prob = prob;
            num_term = 1;
            
            cur_pointer->child = malloc(sizeof(struct PcfgReplacements));
            if (cur_pointer->child == NULL) {
                return 1;
            }
            
            cur_pointer->child->parent = cur_pointer;
            cur_pointer = cur_pointer->child;
            cur_pointer->child = NULL;
            cur_pointer->prob = prob;
        }
    }
    
    //Save the size of the last items
    cur_pointer->size = num_term;
    if ((cur_pointer->value = (char **) malloc(num_term * sizeof(char *))) == NULL) {
        return 1;
    }
    
    // Now on to the second loop where we will save all of the items
    
    // Reset the file pointer to the start of the file
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fprintf(stderr, "Error. Could not seek in: %s\n",filename);
        return 1;
    }
    cur_pointer = terminal_pointer;
    num_term = 0;
    
    while (fgets(buff, MAX_CONFIG_LINE , (FILE*)fp)) {
        
        // Advance to the next container if needed
        if (num_term == cur_pointer->size) {
            cur_pointer = cur_pointer->child;
            num_term = 0;
        }
        
        if (split_value(buff, value, &prob) != 0) {
            fprintf(stderr, "Error. Could not split value in rules file\n");
            return 1;
        }
        // Sanity checking
        if (prob != cur_pointer->prob) {
            fprintf(stderr, "Error. Probability mismatch in rules file\n");
            return 1;
        }
        
        // Save the actual value
        int value_len = strnlen(value, MAX_CONFIG_LINE);

        cur_pointer->value[num_term] = malloc((value_len +1) * sizeof(char *));
        if (cur_pointer->value[num_term] == NULL) {
            return 1;
        }
        strncpy(cur_pointer->value[num_term], value, value_len +1);
        
        num_term++;
        
    }
    
    return 0;
}


// Loads the grammar for a particular terminal
//
// Function returns a non-zero value if an error occurs
//     1 = problem opening the file or malformed ruleset
//
int load_terminal(char *config_filename, char *base_directory, char *structure, PcfgReplacements *grammar_item[]) {
    
    // Get the folder where the files will be saved
    char section_folder[MAX_CONFIG_LINE];
    
    if (get_key(config_filename, structure, "directory", section_folder) != 0) {
        fprintf(stderr, "Could not get folder name for section. Exiting\n");
        return 1;
    }
    
    // Get the filenames associated with the structure
    char result[256][MAX_CONFIG_ITEM];
    int list_size;
    if (config_get_list(config_filename, structure, "filenames", result, &list_size, 256) != 0) {
        fprintf(stderr, "Error reading the config for a rules file. Exiting\n");
        return 1;
	}

    //Load each of the files
    for (int i = 0; i< list_size; i++) {

        //Find the id for this file, aka A1 vs A23. This is the 1, or 23
        char *end_pos = strchr(result[i],'.');
        
        //If there isn't a .txt, this is an invalid file
        if (end_pos == NULL) {
            return 1;
        }
        
        long id = strtol(result[i],&end_pos, 10);
        
        // Check to make sure it was a number
        if ((errno == EINVAL) || (errno == ERANGE))
        {
            fprintf(stderr, "Invalid File name found in rules. Exiting\n");
            return 1;
        }
         
        // Make sure the id falls within the acceptable range
        if (id <= 0) {
            fprintf(stderr, "Invalid File name found in rules. Exiting\n");
            return 1;
        }      

        // Make sure the value isn't too long for the current compiled pcfg_guesser
        if (id > MAX_TERM_LENGTH) {
            continue;
        }
        
        char filename[PATH_MAX];
        snprintf(filename, PATH_MAX, "%s%s%c%s", base_directory,section_folder,SLASH,result[i]);

        if (load_term_from_file(filename, grammar_item[id]) != 0) {
            return 1;
        }

    }

    return 0;
}


// Splits up a base structure string, and allocates an array of BaseReplace
//
// Aka turns A4D3 into C(4)->A(4)->D(3)
//
// Function returns a non-zero value if an error occurs
//     1 = Base structuer not supported
//     2 = Error occured processing the input
//
int split_base(char* input, BaseReplace* base, int *list_size) {
    
    // Do two passes through the data. First finds the number of items
    // and then allocates the memory to store them. Also checks to make sure
    // they are valid.
    //
    // Second saves the data into base
    
    // The number of base items found so far
    int num_items = 0;
    
    // If processing the category (=1), or the id (=0) Aka are we working
    // on the 'A' in "A3"
    int process_category = 1;
    
    // The start position for the current value
    int start_pos = 0;
    
    // A temporary holder for processing values
    char temp_holder[MAX_CONFIG_LINE];
       
    // First pass
    for (int i=0; i< MAX_CONFIG_LINE; i++) {
        
        // We have reached the end of the string
        if (input[i] == '\0') {
            break;
        }
        
        // If we need to switch from processing the cateogy to the id
        if ((process_category == 1) && (isalpha(input[i]) == 0)) {
            
            // Sanity check to make sure the value is a digit
            if (isdigit(input[i])==0) {
                return 2;
            }
            
            // Sanity check to make sure the first item we processed is a
            // category
            if (i == 0) {
                return 2;
            }
            
            // Save the current selection for procesing
            strncpy(temp_holder, input + start_pos, i - start_pos);
            temp_holder[i-start_pos] = '\0';
            
            // Start a new item to process
            //
            // Technically don't need to do this since only using this for
            // processing categories, and the switch from 'id' *should* update
            // this, but it is better to be safe in case I modify the code
            // later
            start_pos = i;
            
            // Check to make the sure the category is supported
            
            // Markov currently isn't supported, but is valid for a rules file
            if (strncmp(temp_holder,"M", MAX_CONFIG_LINE) == 0) {
                return 1;
            }
            else if (strncmp(temp_holder,"A", MAX_CONFIG_LINE) == 0) {
            }
            else if (strncmp(temp_holder,"C", MAX_CONFIG_LINE) == 0) {
            }
            else if (strncmp(temp_holder,"D", MAX_CONFIG_LINE) == 0) {
            }
            else if (strncmp(temp_holder,"Y", MAX_CONFIG_LINE) == 0) {
            }
            else if (strncmp(temp_holder,"O", MAX_CONFIG_LINE) == 0) {
            }
            else if (strncmp(temp_holder,"K", MAX_CONFIG_LINE) == 0) {
            }
            else if (strncmp(temp_holder,"X", MAX_CONFIG_LINE) == 0) {
            }
            // Unknown value was found, error out
            else {
                return 2;
            }
             
            // Start processing a new item pair 
            process_category = 0; 
    
        }
        // If we need to switch from processing the id to a new category
        else if ((process_category == 0) && (isdigit(input[i]) == 0)) {
            
            // Sanity check to make sure the value is an alpha
            if (isalpha(input[i])==0) {
                return 2;
            }
            
            // One full item has been processed
            num_items++;
            
            // Start a new item to process
            start_pos = i;   
        }
    }
    
    // Need to process the final item
    if (process_category == 0) {
        // Make sure there was at least one digit for the id
        // Shouldn't need to do this, but just a sanity check
        if (input[start_pos] == '\0') {
            return 2;
        }
        
        num_items++;
    }
    
    // Only should end with no id for Markov which isn't currently supported
    else {
        // Markov is only one character so check that
        if (start_pos < (MAX_CONFIG_LINE -2)) {
            if (input[start_pos+1] == '\0') {
                if (input[start_pos] == 'M') {
                    return 1;
                }
            }
            // Value is greater than one character
            else {
                return 2;
            }
        }
        else {
            // Hmm loolks like the input wasn't null terminated. This shouldn'tab
            // happen but this is here as a sanity check
            return 2;
        }
    }
    
    // We now know the line is well formatted and how many items there will be
    // so we can allocate memory
    base = malloc(num_items * sizeof(BaseReplace));
    if (base == NULL) {
        return 2;
    }
    (*list_size) = num_items;
        
    // Loop through the line a second time and save the values
    // The start position for the current value
    start_pos = 0;
    process_category = 1;
    num_items = 0;
    
    for (int i=0; i< MAX_CONFIG_LINE; i++) {
        // We have reached the end of the string
        if (input[i] == '\0') {
            break;
        }
        
        // If we need to switch from processing the cateogy to the id
        if ((process_category == 1) && (isalpha(input[i]) == 0)) {
            
            // Save the current selection
            base[num_items].type = malloc( ((i - start_pos) + 1) * sizeof(char));
            strncpy(base[num_items].type, input + start_pos, i - start_pos);
            base[num_items].type[i-start_pos] = '\0';
            
            // Start a new item to processr
            start_pos = i;
            
            process_category = 0;
        }
        
        // If we need to switch from processing the id to a new category
        else if ((process_category == 0) && (isdigit(input[i]) == 0)) {
            
            // Save the id
            strncpy(temp_holder, input + start_pos, i - start_pos);
            temp_holder[i-start_pos] = '\0';
            
            base[num_items].id = atoi(temp_holder);
            
            // One full item has been processed
            num_items++;
            
            // Start a new item to process
            start_pos = i; 

            process_category = 1;            
        }
        
    }
    
    // Need to process the last id
    strncpy(temp_holder, input + start_pos, MAX_CONFIG_LINE);
    base[num_items].id = atoi(temp_holder);
 
    return 0;
}

// Loads the grammar for base structures
//
// Function returns a non-zero value if an error occurs
//     1 = problem opening the file or malformed ruleset
//
int load_base_structures(char *config_filename, char *base_directory, PcfgBase *base_structures) {
    
    // Get the folder where the files will be saved
    char section_folder[MAX_CONFIG_LINE];
    
    if (get_key(config_filename, "START", "directory", section_folder) != 0) {
        fprintf(stderr, "Could not get folder name for section. Exiting\n");
        return 1;
    }
    
    // Get the filenames associated with the structure
    char result[256][MAX_CONFIG_ITEM];
    int list_size;
    if (config_get_list(config_filename, "START", "filenames", result, &list_size, 256) != 0) {
        fprintf(stderr, "Error reading the config for a rules file. Exiting\n");
        return 1;
	}
    
    // Shouldn't have more than 1 file for base structures so do sanity check_encoding
    if (list_size != 1) {
        return 1;
    }
    
    // create the filename
    char filename[PATH_MAX];
    snprintf(filename, PATH_MAX, "%s%s%c%s", base_directory,section_folder,SLASH,result[0]);
    
    // Open the file to read in the base structures
    // Pointer to the open terminal file
    FILE *fp;
    fp= fopen(filename,"r");
    
    // Check to make sure the file opened correctly
    if (fp== NULL) {
        
        //Could not open the file. Print error and return an error
        fprintf(stderr, "Error. Could not read the file: %s\n",filename);
        return 1;
    }
    
    // Want to make sure we provision the first item so need to loop initially
    int started = 0;
    
    // The base structure we are currently working on
    base_structures = malloc(sizeof(PcfgBase));
    if (base_structures == NULL) {
        return 1;
    }
    PcfgBase *cur_pointer = base_structures;
    cur_pointer->prev = NULL; 
    cur_pointer->next = NULL;
    
    // Holds the current line in the config file
    char buff[MAX_CONFIG_LINE];
    
    // Start looping through the input file, processing the base_structures
    while (fgets(buff, MAX_CONFIG_LINE , (FILE*)fp)) {
    
        // The probability of the current item;
        double prob;
        
        // A temp holder for the string value in the file
        char value[MAX_CONFIG_LINE];
    
        // Break up the first line. Using previous_prob since this will be the
        // inital value for it when comparing against other items.
        if (split_value(buff, value, &prob) != 0) {
            return 1;
        }
        
        // Holds pointer for the items for processing the base structure
        // Will be malloc'd by split_base is successful
        BaseReplace *base_items;
        int size;
    
        switch (split_base(value, base_items, &size)) {
            case 0:
                // First base_structure
                if (started == 0) {
                   started = 1;
                }
                // Need to advance cur_pointer and allocate memory
                else {
                    cur_pointer->next = malloc(sizeof(PcfgBase));
                    if (cur_pointer->next == NULL) {
                        return 1;
                    }
                    cur_pointer->next->prev = cur_pointer;
                    cur_pointer = cur_pointer->next;
                    cur_pointer->next = NULL;     
                }
                   
                cur_pointer->prob = prob;
                cur_pointer->value = base_items;
                cur_pointer->size = size;       
                break;
            case 1:
                break;
            default:
                return 1;
        }
    
    }
    
    // Make sure at least one base_structure was processed
    if (started == 0) {
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
    snprintf(base_directory, FILENAME_MAX, "%sRules%c%s%c", exec_directory,  SLASH, program_info.rule_name, SLASH);
    
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
    
    // Read in the alpha terminals
    if (load_terminal(config_filename, base_directory, "BASE_A", pcfg->alpha) != 0) {
        fprintf(stderr, "Error reading the rules file. Exiting\n");
        return 1;
	}
    
    // Read in the capitalization masks
    if (load_terminal(config_filename, base_directory, "CAPITALIZATION", pcfg->capitalization) != 0) {
        fprintf(stderr, "Error reading the rules file. Exiting\n");
        return 1;
	}
    
    // Read in the digit terminals 
    if (load_terminal(config_filename, base_directory, "BASE_D", pcfg->digits) != 0) {
        fprintf(stderr, "Error reading the rules file. Exiting\n");
        return 1;
	}
    
    // Read in the years terminals 
    if (load_terminal(config_filename, base_directory, "BASE_Y", pcfg->years) != 0) {
        fprintf(stderr, "Error reading the rules file. Exiting\n");
        return 1;
	}
    
    // Read in the "other" terminals 
    if (load_terminal(config_filename, base_directory, "BASE_O", pcfg->other) != 0) {
        fprintf(stderr, "Error reading the rules file. Exiting\n");
        return 1;
	}
    
    // Read in the conteXt sensitive terminals
    if (load_terminal(config_filename, base_directory, "BASE_X", pcfg->x) != 0) {
        fprintf(stderr, "Error reading the rules file. Exiting\n");
        return 1;
	}
    
    // Read in the keyboard combo terminals
    if (load_terminal(config_filename, base_directory, "BASE_K", pcfg->keyboard) != 0) {
        fprintf(stderr, "Error reading the rules file. Exiting\n");
        return 1;
	}
    
    // Now read in the base structures. Note, this doesn't need to be done last
    // but depending on what enhancements are done in the future it's good
    // practice to process these at the end.
    if (load_base_structures(config_filename, base_directory, pcfg->base_structures) != 0) {
        fprintf(stderr, "Error reading the base_structure file in the rules. Exiting\n");
        return 1;
	}
    
    return 0;
}