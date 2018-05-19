/****************************************************************
** Program Filename: hashMap.c
** Author: Chelsea Egan
** Date: 11/18/2017
** Description: Implements a hash map. The hash map uses a table
** of buckets, each containing a linked list of hash links. Each
** hash link stores the key-value pair and a pointer to the next
** link. HASH_FUNCTION is a macro that allows you to choose which
** hash function to use.
****************************************************************/

/*
* CS 261 Data Structures
* Assignment 5
* Name: Chelsea Egan
* Date: 11/11/2017
*/

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int hashFunction1(const char* key)
{
     int r = 0;
     for (int i = 0; key[i] != '\0'; i++)
     {
          r += key[i];
     }
     return r;
}

int hashFunction2(const char* key)
{
     int r = 0;
     for (int i = 0; key[i] != '\0'; i++)
     {
          r += (i + 1) * key[i];
     }
     return r;
}

/**
* Creates a new hash table link with a copy of the key string.
* @param key Key string to copy in the link.
* @param value Value to set in the link.
* @param next Pointer to set as the link's next.
* @return Hash table link allocated on the heap.
*/
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
     HashLink* link = malloc(sizeof(HashLink));
     link->key = malloc(sizeof(char) * (strlen(key) + 1));
     strcpy(link->key, key);
     link->value = value;
     link->next = next;
     return link;
}

/**
* Free the allocated memory for a hash table link created with hashLinkNew.
* @param link
*/
static void hashLinkDelete(HashLink* link)
{
     free(link->key);
     free(link);
}

/**
* Initializes a hash table map, allocating memory for a link pointer table with
* the given number of buckets.
* @param map
* @param capacity The number of table buckets.
*/
void hashMapInit(struct HashMap* map, int capacity)
{
     map->capacity = capacity;
     map->size = 0;
     map->table = malloc(sizeof(HashLink*) * capacity);
     for (int i = 0; i < capacity; i++)
     {
          map->table[i] = NULL;
     }
}

/**
* Removes all links in the map and frees all allocated memory. You can use
* hashLinkDelete to free the links.
* @param map
*/
void hashMapCleanUp(struct HashMap* map)
{
     // FIXME: implement
     assert(map != 0);

     /* Loop through dyn array */
     for (int i = 0; i < map->capacity; i++) {
          /* Get first link */
          struct HashLink * currentLink = map->table[i];
          map->table[i] = 0;
          /* If there are more links in this bucket */
          while (currentLink != 0) {
               /* Loop through linked list */
               struct HashLink * temp = currentLink;
               currentLink = currentLink->next;
               /* Free current link */
               hashLinkDelete(temp);
          }
          free(map->table[i]);
     }
     free(map->table);
}

/**
* Creates a hash table map, allocating memory for a link pointer table with
* the given number of buckets.
* @param capacity The number of buckets.
* @return The allocated map.
*/
struct HashMap* hashMapNew(int capacity)
{
     struct HashMap* map = malloc(sizeof(struct HashMap));
     hashMapInit(map, capacity);
     return map;
}

/**
* Removes all links in the map and frees all allocated memory, including the
* map itself.
* @param map
*/
void hashMapDelete(struct HashMap* map)
{
     hashMapCleanUp(map);
     free(map);
}

/**
* Returns a pointer to the value of the link with the given key. Returns NULL
* if no link with that key is in the table.
*
* Use HASH_FUNCTION(key) and the map's capacity to find the index of the
* correct linked list bucket. Also make sure to search the entire list.
*
* @param map
* @param key
* @return Link value or NULL if no matching link.
*/
int* hashMapGet(struct HashMap* map, const char* key)
{
     // FIXME: implement
     /* Confirm map is created */
     assert(map != 0);

     /* Get hash index for key */
     int hashIndex = HASH_FUNCTION(key) % (map->capacity);

     /* Get first link in bucket */
     struct HashLink *myNode = map->table[hashIndex];

     while (myNode != 0) {
          /* This link has a matching key */
          if (strcmp(myNode->key, key) == 0) {
               /* Return the value */
               return &(myNode->value);
          }
          /* Not a match */
          else {
               /* Move to next link */
               myNode = myNode->next;
          }
     }

     return 0;
}

/**
* Resizes the hash table to have a number of buckets equal to the given
* capacity. After allocating the new table, all of the links need to be
* rehashed into it because the capacity has changed.
*
* Remember to free the old table and any old links if you use hashMapPut to
* rehash them.
*
* @param map
* @param capacity The new number of buckets.
*/
/* Source: https://github.com/moogalin/CS261-HashTables/blob/master/hashMap.c */
/* Source: http://dl.interfaceware.com/oss/git/v2.6.3/hashmap.c */
void resizeTable(struct HashMap* map, int capacity)
{
     struct HashLink ** h = map->table;
     int i;

     int oldSize = map->capacity;

     hashMapInit(map, capacity);

     for (i = 0; i < oldSize; i++) {
          struct HashLink * currentLink = h[i];

          while (currentLink) {
               hashMapPut(map, currentLink->key, currentLink->value);
               currentLink = currentLink->next;
          }
     }

     /* Clean up hash map */
     for (int i = 0; i < oldSize; i++) {
          struct HashLink * garbage = h[i];
          h[i] = 0;

          while (garbage != 0) {
               struct HashLink * temp = garbage;
               garbage = garbage->next;
               hashLinkDelete(temp);
          }

          free(h[i]);
     }

     free(h);
}

/**
* Updates the given key-value pair in the hash table. If a link with the given
* key already exists, this will just update the value. Otherwise, it will
* create a new link with the given key and value and add it to the table
* bucket's linked list. You can use hashLinkNew to create the link.
*
* Use HASH_FUNCTION(key) and the map's capacity to find the index of the
* correct linked list bucket. Also make sure to search the entire list.
*
* @param map
* @param key
* @param value
*/
/* Source: https://github.com/100timesforever/CS261-assignment6/blob/master/hashMap.c */
void hashMapPut(struct HashMap* map, const char* key, int value)
{
     // FIXME: implement
     /* Confirm map is implemented */
     assert(map != 0);

     /* Make sure the hash map is not overloaded */
     if (hashMapTableLoad(map) >= MAX_TABLE_LOAD) {
          /* If overloaded - resize */
          resizeTable(map, 2 * (map->capacity));
     }

     /* Get index location of hash map */
     int hashIndex = HASH_FUNCTION(key) % (map->capacity);
     if (hashIndex < 0) {
          hashIndex += map->capacity;
     }

     /* If bucket is not empty */
     if (map->table[hashIndex] != 0) {
          struct HashLink * currentLink = map->table[hashIndex];
          /* If not a match, move to next */
          while (strcmp(currentLink->key, key) != 0 && currentLink->next != 0) {
               currentLink = currentLink->next;
          }
          /* No match */
          if (currentLink->next == 0 && strcmp(currentLink->key, key) != 0) {
               /* Create new node */
               struct HashLink * newLink = hashLinkNew(key, value, map->table[hashIndex]);
               map->table[hashIndex] = newLink;
               map->size++;
          }
          /* Match */
          else {
               currentLink->value = value;
          }
     }

     /* Bucket is empty */
     else {
          /* Create new node */
          struct HashLink * newLink = hashLinkNew(key, value, 0);
          map->table[hashIndex] = newLink;
          map->size++;
     }
}

/**
* Removes and frees the link with the given key from the table. If no such link
* exists, this does nothing. Remember to search the entire linked list at the
* bucket. You can use hashLinkDelete to free the link.
* @param map
* @param key
*/
/* Source: http://www.sanfoundry.com/c-program-implement-hash-tables-chaining-with-singly-linked-lists/ */
void hashMapRemove(struct HashMap* map, const char* key)
{
     // FIXME: implement
     /* Confirm map is not empty */
     assert(map != 0);
     assert(map->size > 0);

     /* Find hash index for key */
     int hashIndex = HASH_FUNCTION(key) % map->capacity;

     /* Get head link of bucket */
     struct HashLink * currentLink, *headLink, *hashLink;
     headLink = hashLink = currentLink = map->table[hashIndex];

     /* Loop through list until end or found key */
     while (currentLink != 0) {
          if (strcmp(currentLink->key, key) == 0) {
               /* If head of list */
               if (headLink == currentLink) {
                    /* Change head of list */
                    headLink = currentLink->next;
                    map->table[hashIndex] = headLink;
               }
               else {
                    /* Unlink */
                    hashLink->next = currentLink->next;
               }

               /* Free memory */
               hashLinkDelete(currentLink);
               /* Decrement size of map */
               map->size--;
               currentLink = 0;
          }
          /* Key was not a match */
          else {
               /* Move to next node */
               hashLink = currentLink;
               currentLink = currentLink->next;
          }
     }
}

/**
* Returns 1 if a link with the given key is in the table and 0 otherwise.
*
* Use HASH_FUNCTION(key) and the map's capacity to find the index of the
* correct linked list bucket. Also make sure to search the entire list.
*
* @param map
* @param key
* @return 1 if the key is found, 0 otherwise.
*/
int hashMapContainsKey(struct HashMap* map, const char* key)
{
     // FIXME: implement
     /* Confirm map is not empty */
     assert(map != 0);

     /* Get hash index of key */
     int hashIndex = HASH_FUNCTION(key) % (map->capacity);

     /* Get pointer to head of list */
     struct HashLink * currentLink = map->table[hashIndex];

     /* List is empty */
     if (!currentLink) {
          return 0;
     }

     /* Loop through list */
     while (currentLink != 0) {
          /* Check if match */
          if (strcmp(currentLink->key, key) == 0) {
               return 1;
          }
          /* Move to next link */
          else {
               currentLink = currentLink->next;
          }
     }

     /* Key was not found in list */
     return 0;
}

/**
* Returns the number of links in the table.
* @param map
* @return Number of links in the table.
*/
int hashMapSize(struct HashMap* map)
{
     // FIXME: implement
     assert(map != 0);
     return(map->size);
}

/**
* Returns the number of buckets in the table.
* @param map
* @return Number of buckets in the table.
*/
int hashMapCapacity(struct HashMap* map)
{
     // FIXME: implement
     assert(map != 0);
     return(map->capacity);
}

/**
* Returns the number of table buckets without any links.
* @param map
* @return Number of empty buckets.
*/
int hashMapEmptyBuckets(struct HashMap* map)
{
     // FIXME: implement
     /* Confirm map is implemented */
     assert(map != 0);

     int bucketCounter = 0;

     for (int i = 0; i < map->capacity; i++) {
          /* If bucket is empty */
          if (map->table[i] == 0) {
               bucketCounter++;
          }
     }

     return bucketCounter;
}

/**
* Returns the ratio of (number of links) / (number of buckets) in the table.
* Remember that the buckets are linked lists, so this ratio tells you nothing
* about the number of empty buckets. Remember also that the load is a floating
* point number, so don't do integer division.
* @param map
* @return Table load.
*/
float hashMapTableLoad(struct HashMap* map)
{
     // FIXME: implement
     /* Confirm map is implemented */
     assert(map != 0);

     /* Get number of buckets and number of links */
     float buckets = (float)hashMapCapacity(map);
     float links = (float)hashMapSize(map);

     /* Get ratio of links to buckets */
     float load = links / buckets;

     return load;
}

/**
* Prints all the links in each of the buckets in the table.
* @param map
*/
void hashMapPrint(struct HashMap* map)
{
     for (int i = 0; i < map->capacity; i++)
     {
          HashLink* link = map->table[i];
          if (link != 0)
          {
               printf("\nBucket %i ->", i);
               while (link != 0)
               {
                    printf(" (%s, %d) ->", link->key, link->value);
                    link = link->next;
               }
          }
     }
     printf("\n");
}