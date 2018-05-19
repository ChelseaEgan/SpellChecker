/***** NOTE: I changed the max load to 10 *****/

/****************************************************************
** Program Filename: spellChecker.c
** Author: Chelsea Egan
** Date: 11/18/2017
** Description: Implements a spell checker using a hash map. It
** prompts the user for a word, confirms if spelled correctly,
** and, if incorrect, provides 6 suggestions. The program loops
** until the user types "quit". This spell checker is implemented
** using the Levenshtein Distance algorithm.
****************************************************************/

#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*This is a macro used in the Levenshtein distance algorithm */

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

#define HASH_FUNCTION hashFunction1

/**
* Allocates a string for the next word in the file and returns it. This string
* is null terminated. Returns NULL after reaching the end of the file.
* @param file
* @return Allocated string or NULL.
*/
char* nextWord(FILE* file)
{
     int maxLength = 16;
     int length = 0;
     char* word = malloc(sizeof(char) * maxLength);
     while (1)
     {
          char c = fgetc(file);
          if ((c >= '0' && c <= '9') ||
               (c >= 'A' && c <= 'Z') ||
               (c >= 'a' && c <= 'z') ||
               c == '\'')
          {
               if (length + 1 >= maxLength)
               {
                    maxLength *= 2;
                    word = realloc(word, maxLength);
               }
               word[length] = c;
               length++;
          }
          else if (length > 0 || c == EOF)
          {
               break;
          }
     }
     if (length == 0)
     {
          free(word);
          return NULL;
     }
     word[length] = '\0';
     return word;
}

/**
* Loads the contents of the file into the hash map.
* @param file
* @param map
*/
void loadDictionary(FILE* file, HashMap* map)
{
     // FIXME: implement
     char * key = nextWord(file);
     while (key != NULL) {
          hashMapPut(map, key, 1);
          free(key);
          key = nextWord(file);
     }
}

/**
* Calculates the Levenshtein distance between input word and dictionary words
* @param user's string
* @param string from dictionary
* @returns Levenshtein distance
*/
/* Source: https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C */
int levenshtein(char *s1, char *s2) {
     unsigned int s1len, s2len, x, y, lastdiag, olddiag;
     s1len = strlen(s1);
     s2len = strlen(s2);
     unsigned int * column = malloc((s1len + 1) * sizeof(int));
     for (y = 1; y <= s1len; y++)
          column[y] = y;
     for (x = 1; x <= s2len; x++) {
          column[0] = x;
          for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
               olddiag = column[y];
               column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (s1[y - 1] == s2[x - 1] ? 0 : 1));
               lastdiag = olddiag;
          }
     }
     int levDistance = column[s1len];
     free(column);
     return(levDistance);
}

/**
* Prints the concordance of the given file and performance information. Uses
* the file input1.txt by default or a file name specified as a command line
* argument.
* @param argc
* @param argv
* @return
*/
int main(int argc, const char** argv)
{
     // FIXME: implement
     HashMap* map = hashMapNew(1000);

     /* Open file */
     FILE* file = fopen("dictionary.txt", "r");
     /* Test that file opened correctly */
     if (file == NULL) {
          fprintf(stderr, "Cannot open dictionary.txt\n");
     }
     clock_t timer = clock();

     /* Load the dictionary into the hash map */
     loadDictionary(file, map);

     timer = clock() - timer;
     printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
     
     fclose(file);

     /* Buffer to hold user's string */
     char inputBuffer[256];
     /* Int to indicate when user wants to quit program */
     int quit = 0;
     /* Loop until user chooses to quit */
     while (!quit)
     {
          /* Prompt user for word */
          printf("Enter a word or \"quit\" to quit: ");
          /* Read in word */
          scanf("%s", inputBuffer);

          // Implement the spell checker code here..
          /* Check if user chose to quit */
          if (strcmp(inputBuffer, "quit") == 0)
          {
               quit = 1;
          }
          /* If not quit... */
          else {
               /* Check if word is spelled correctly */
               if (hashMapContainsKey(map, inputBuffer)) {
                    printf("%s is spelled correctly.\n", inputBuffer);
               }
               /* If incorrect */
               else {
                    /* Array to hold six suggestions */
                    char * suggestions[6];
                    /* Fill suggestions array */
                    for (int i = 0; i < 6; i++) {
                         suggestions[i] = '\0';
                    }
                    /* Index if word would be entered into hash map */
                    /* Allows algorithm to start at the best place */
                    int index = HASH_FUNCTION(inputBuffer);
                    /* Int to hold Levenshtein distance */
                    /* Initialize as -1 to indicate it has not been calculated */
                    int levDistance = -1;
                    /* Preferred Levenshtein distance is 1 */
                    /* If not enough words are found at this distance, it will be increased */
                    int prefDistance = 1;
                    /* Current link in hash map */
                    struct HashLink * currentLink = map->table[index];
                    /* Counter for number of suggestions */
                    int counter = 0;
                    /* Index for suggestions array (allows words to be overwritten with better ones) */
                    int suggestionsIndex = 0;

                    /* Loop through hash map until end or enough words have been found */
                    while (index < map->capacity && counter < 6) {
                         /* While not end of linked list */
                         while (currentLink != 0) {
                              /* Calculate Levenshtein distance between user's word and dictionary word */
                              levDistance = levenshtein(inputBuffer, currentLink->key);
                              /* If within preferred distance */
                              if (levDistance > 0 && levDistance <= prefDistance) {
                                   /* Check if word is already in array */
                                   int duplicate = 0;
                                   for (int i = 0; i < 6; i++) {
                                        if (suggestions[i] == currentLink->key) {
                                             duplicate = 1;
                                        }
                                   }
                                   /* If not a duplicate */
                                   if (!duplicate) {
                                        /* Add word to array */
                                        suggestions[suggestionsIndex] = currentLink->key;
                                        /* Increment suggestions index */
                                        suggestionsIndex++;
                                        /* If we have not hit enough words yet, increment */
                                        if (counter < 6) {
                                             counter++;
                                        }
                                        /* If end of array index, loop back to 0 */
                                        if (suggestionsIndex >= 6) {
                                             suggestionsIndex = 0;
                                        }
                                   }
                              }
                              /* Move to next link */
                              currentLink = currentLink->next;
                              /* Reset levenshtein difference */
                              levDistance = -1;
                         }

                         index++;
                         /* If end of hash map array, but still not enough words */
                         if (index >= map->capacity && counter < 6) {
                              /* Loop back to end of array */
                              index = 0;
                              /* Increase allowed levenshtein difference */
                              prefDistance++;
                         }
                         /* Move to next bucket in hash map array */
                         currentLink = map->table[index];
                    }

                    /* Print suggestions */
                    printf("Perhaps you meant...\n");
                    for (int i = 0; i < 6; i++) {
                              printf("%s\n", suggestions[i]);
                    }
               }
          }
     }

     hashMapDelete(map);
     return 0;
};