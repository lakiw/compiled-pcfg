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

#include "base_structure_io.h"


// Splits up a base structure string, and allocates an array of BaseReplace
//
// Aka turns A4D3 into C(4)->A(4)->D(3)
//
// Function returns a non-zero value if an error occurs
//     1 = Base structuer not supported
//     2 = Error occured processing the input
//
int split_base(char* input, BaseReplace **base, int *list_size) {
    
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
    (*base) = malloc(num_items * sizeof(BaseReplace));
    if ((*base) == NULL) {
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
            (*base)[num_items].type = malloc( ((i - start_pos) + 1) * sizeof(char));
            strncpy((*base)[num_items].type, input + start_pos, i - start_pos);
            (*base)[num_items].type[i-start_pos] = '\0';
            
            // Start a new item to processr
            start_pos = i;
            
            process_category = 0;
        }
        
        // If we need to switch from processing the id to a new category
        else if ((process_category == 0) && (isdigit(input[i]) == 0)) {
            
            // Save the id
            strncpy(temp_holder, input + start_pos, i - start_pos);
            temp_holder[i-start_pos] = '\0';
            
            (*base)[num_items].id = atoi(temp_holder);
            
            // One full item has been processed
            num_items++;
            
            // Start a new item to process
            start_pos = i; 

            process_category = 1;            
        }
        
    }
    
    // Need to process the last id
    strncpy(temp_holder, input + start_pos, MAX_CONFIG_LINE);
    (*base)[num_items].id = atoi(temp_holder);
 
    return 0;
}

// Loads the grammar for base structures
//
// Function returns a non-zero value if an error occurs
//     1 = problem opening the file or malformed ruleset
//
int load_base_structures(char *config_filename, char *base_directory, PcfgBase **base_structures) {
    
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
    (*base_structures) = malloc(sizeof(PcfgBase));
    if ((*base_structures) == NULL) {
        return 1;
    }
    PcfgBase *cur_pointer = (*base_structures);
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
    
        switch (split_base(value, &base_items, &size)) {
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
