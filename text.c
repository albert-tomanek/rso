/*
 * An assortment of functions to do with strings and char arrays...
 */

#ifndef TEXT
  #include "text.h"
#endif


#include <string.h>
#include <ctype.h>   // for toupper();
#include <stdlib.h>  // for malloc(); free(); and sizeof();

#define TEXT_VERSION 1


/*
char splitstrings[5][10];
splitstrings[2][3] => <char>
splitstrings[2] => *char
splitstrings => **char
*/

void text_splitstr(char *string, char sep, char splitstrings[STRLEN][MAX_WORDS])  // *splitstrings[][] is allocated previously
{
    /* 
     * This splits a string at every 'sep' character, and then writes the strings into a two dimentional array given to it.
     */
    
    int string_no = 0;
    int char_no   = 0;
    
    for (int i = 0; i < strlen(string); i++)
    {
        if (string[i] != sep)
        {
            splitstrings[string_no][char_no] = string[i];
            
            char_no++ ;
        }
        else            // i.e. It IS the separator
        {
            splitstrings[string_no][char_no] = '\0';
            
            string_no++ ;
            char_no = 0;
        }
    }
}

char *text_uppercase(char *in) // this is a pointer to the 1st char of 'in', so we won't need to add the * next time we use it in this function.
{						 // 'in' won't be copied, it's original memory address will be used.
	
	//char out[STRLEN];
	//strncpy(out, in, STRLEN); 		// copy 'in' into 'out', so we don't overwrite the original...
	
	for (int i = 0; i < STRLEN; i++) {			// Loop over each of the characters,
		if (in[i] == '\0') break;				// <- No need for strcmp as it is only one characer
		else in[i] = toupper(in[i]); 			// writing the uppercace of each character into OUT.
	}
	
	return in;  	
}
char *text_lowercase(char *in) 	// this is a pointer to the 1st char of 'in', so we won't need to add the * next time we use it in this function.
{						 		// 'in' won't be copied, it's original memory address will be used.
	
	//char out[STRLEN];
	//strncpy(out, in, STRLEN); 		// copy 'in' into 'out', so we don't overwrite the original...
	
	for (int i = 0; i < STRLEN; i++) {			// Loop over each of the characters,
		if (in[i] == '\0') break;				// <- No need for strcmp as it is only one characer
		else in[i] = tolower(in[i]); 			// writing the uppercace of each character into OUT.
	}
	
	return in;  	
}


char *text_multiplychar(char chr, int times)
{
	char *outstr = malloc( sizeof(char) * times +1 ); // +1 for the null-terminator
	
	for (int i = 0; i < times; i++)
	{
		outstr[i] = chr;
	}
	
	outstr[times] = '\0';
	
	return outstr;
	//strncpy(outstr, out, times);
}

int text_countchar(char chr, char *string)
{
	int stringlen = strlen(string);
	int char_count = 0;
	
	for (int i = 0; i < stringlen; i++)
	{
		if (string[i] == chr)
		{
			char_count++;
		}
	}
	
	return char_count;
}

int text_contains(char *string, char *strings[], int stringslen)   	// Hand this command *argv[] , and it will find you an argument in it.
{
	int found_it = 0; 
	for (int i = 0; i < stringslen; i++) {
		if (! strcmp(string, strings[i]))
		{ 
			found_it = 1;
		}						// We do not need to break, as found_it is never set back to 0.
	}
	
	return found_it;
}

/*int text_multiLineInput(char lines[MAX_LINES][STRLEN])
{
	int loopCount = 0;
	
	while (1)
	{
		fgets(lines[loopCount], STRLEN, stdin);
		
		if ( strcmp(lines[loopCount], ".\n" ) == 0) { 	// They want to finish
			break;
		} 
		else {
			loopCount++ ;	
		}
	}
	
	return loopCount;
}*/
 
int text_version()          // Note: This is not a 1's and 0's matter, other numbers will be returned too.
{
    return TEXT_VERSION;
}