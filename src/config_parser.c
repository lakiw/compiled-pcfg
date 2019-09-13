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

#include "config_parser.h"


// Gets a list from a section/key combo from a config file and stores an array
// of strings in result. The number of strings found is passed back in
// the list_size int.
//
// Disclaimer, doesn't handle escaped """ quotes, as that wasn't necessary
// for my use-case
//
// Function returns a non-zero value if an error occurs
//     1 = problem opening the file
//     2 = could not find the key
//     3 = key not a list
int config_get_list(char *filename, char *section, char *key, char result[][MAX_CONFIG_ITEM], int *list_size, int max_items) {
    
    // Holds the full unformatted string result of looking up the section/key
    char raw_result[MAX_CONFIG_LINE];

    // Grab the unformatted string from the section/key combo
    int ret_value = get_key(filename, section, key, raw_result);
    
    // Check to make sure we were able to grab the data
    // If not, return the error value
    if (ret_value != 0) {
        return ret_value;
    }
    
    // Get the length of the buffer we will be parsing
    int raw_len = strnlen(raw_result, MAX_CONFIG_LINE);
    
    // Quick exit if the item is too small to be a list
    if (raw_len < 2) {
        return 3;
    }
    
    // Quick exit if the item is not enclosed in brackets
    if (raw_result[0] != '[' || raw_result[raw_len-1] != ']') {
        return 3;
    }
    
    //the start of the raw string we'll be saving. -1 if not started
    int raw_string_start = -1;

    // Start parsing the raw results for strings
    // Skipping the brackets at the start/end of the list
    for (int raw_pos = 1; raw_pos < raw_len -1; raw_pos++) {
        
        // It is either the start or end of a new item
        if (raw_result[raw_pos] == '"') {
            // It is the start of an item
            if (raw_string_start == -1) {
                // Making it +1 so we don't copy the "
                raw_string_start = raw_pos + 1;
            }
            // It is the end of an item
            else {
                
                // Check to make sure there is space to copy the next item
                if ((*list_size) == max_items) {
                    return 3;
                }
                // Copy the string to the position in the array
                strncpy(result[(*list_size)], raw_result + raw_string_start, raw_pos - raw_string_start);
                
                //Reset the pointer to the start to say we are not processing
                //any items now
                raw_string_start = -1;
                
                //Increse the list size to point to the next free spot
                (*list_size) = (*list_size) + 1;
            }
        }
    }       
    
    // Sanity check to make sure the last item wasn't an unescaped "
    if (raw_string_start != -1) {
        return 3;
    }
    
    return 0;
}    


// Gets the value from a key and returns it as a string in result var
// Function returns a non-zero value if an error occurs
//     1 = problem opening the file
//     2 = could not find the key
//
int get_key(char *filename, char *section, char *key, char *result) {
    
    // Pointer to the open config file
    FILE *config;
    
    // Open the config file
    config = fopen(filename,"r");
    
    // Check to make sure the file opened correctly
    if (config == NULL) {
        
        //Could not open the file. Print error and return an error
        fprintf(stderr, "Error. Could not read the file: %s\n",filename);
        return 1;
    }
    
    // Format the section name to make matching easier
    // Basically add the brackets around it []
    //
    // I know, I could just pass in the section names properly formatted and
    // skipthis step, but I figure this makes the code a bit easier to read
    //
    int section_len = strnlen(section,50) + 2;
    char section_name[section_len];
    strncpy(section_name, "[", section_len);
    strncat(section_name, section, section_len);
    strncat(section_name, "]", section_len);
    
    // Also get the length of the key for comparision, and format it
    // with the trailing " = "
    int key_len = strnlen(key,50) + 3;
    char key_name[key_len];
    strncpy(key_name, key, key_len);
    strncat(key_name, " = ", key_len);
    
    // Holds the current line in the config file
    char buff[MAX_CONFIG_LINE];
    
    // Tells us if we are in the target section or not
    int found_section = 0;
    
    // Loop through the config file
    while (fgets(buff, MAX_CONFIG_LINE , (FILE*)config)) {
        
        // If we are not currently in the target section
        if (found_section == 0){
            
            // We found the section header
            //
            // Note, limiting it the the length of the compare header so 
            // we don't have to deal with different types of newlines
            if (strncmp(section_name, buff, section_len) == 0) {
                found_section = 1;
            }            
        }
        
        // We are looking for the key_value
        else {
            
            // Found the key name
            if (strncmp(key_name, buff, key_len) == 0) {
              
                //Copy the result minus the key name to result
                strncpy(result, buff + key_len, MAX_CONFIG_LINE);
                
                //strip off the trailing newlines and return success
                int result_len = strnlen(result,MAX_CONFIG_LINE);
                result[result_len-2] = '\0';
                fclose(config);
                return 0;
            }
        }
    }
    
    // Did not find the key
    fclose(config);
    return 2;
}