/* 
Purpose:   The implementation file for a hash table of words and numbers
*/
#include <assert.h>
#include <iostream>
#include <fstream>
#include "hashtable.h"

using namespace std;

/*-------------------------- Constructors/Destructors ----------------------*/

/**
 * @brief Construct a new Hash Table:: Hash Table object
 * 
 * Parameters:  ht: the hashtable to copy
 * Purpose:     copy a hashtable 
 *              NOTE:  this is crucial to include since it is invoked
 *              by call-by-value parameter passing
 * Returns:     nothing
 * 
 * @param ht 
 */
HashTable::HashTable(const HashTable &ht) {
   size = ht.size; // set the size of the array
   used = ht.used; // set the size of the array

   if ((hashtable = new StringIntPair[size]) == NULL) {
      cout << "Out of memory at HashTable::HashTable(const HashTable)\n";
   }

   assert(hashtable != 0);

   for (unsigned long i = 0; i < size; i++) { // make a _copy_ of the array elements
      hashtable[i].key = ht.hashtable[i].key;
      hashtable[i].data = ht.hashtable[i].data;
      // hashtable[i].linkedList = ht.hashtable[i].linkedList;
   }
}

/**
 * @brief Construct a new Hash Table:: Hash Table object
 * 
 * Parameters:  none
 * Purpose:     allocate a hashtable for an expected number of keys
 *              initializes all values to null (0)
 * Returns:     pointer to the created HashTable or 0 if out of memory
 * 
 * @param NumKeys 
 */
HashTable::HashTable(const unsigned long NumKeys) {
   // allocate space for the table, init to null key
   size = NumKeys * 3; // we want the hash table to be 2/3 empty
   used = 0;
   collisions = 0;
   lookups = 0;

   if ((hashtable = new StringIntPair[size]) == NULL) {
      cout << "Out of memory at HashTable::HashTable(unsigned long)\n";
   }

   assert(hashtable != 0);

   // initialize the hashtable
   for (unsigned long i = 0; i < size; i++) {
      hashtable[i].key = "";
      hashtable[i].data = 0;
      // hashtable[i].linkedList;
   }
}

/**
 * @brief Destroy the Hash Table:: Hash Table object
 * 
 * Parameters:  none
 * Purpose:     deallocate a hash table
 * Returns:     nothing
 * 
 */
HashTable::~HashTable() {
   delete [] hashtable;
}

/*-------------------------- Accessors ------------------------------------*/

unsigned long HashTable::GetSize() {
   return size;
}

string HashTable::GetKey(unsigned long i) {
   return hashtable[i].key;
}

/**
 * @brief 
 * 
 * Parameters:  a fileptr
 * Purpose:     print the contents of the hash table
 *              currently, only prints non-null entries
 * Returns:     nothing
 * 
 * @param filename 
 */
void HashTable::Print(const char *filename) const {
   ofstream fpout(filename); 

   // Print out the non-zero contents of the hashtable
   for (unsigned long i = 0; i < size; i++) {  
      if (hashtable[i].key != "") {
         fpout << hashtable[i].key << " " << hashtable[i].data << "\n";
      }
   }

   fpout.close();
   // cout << "Collisions: " << collisions << ", Used: " << used <<  ", Lookups: " << lookups << "\n";
}

/**
 * @brief 
 * 
 * Parameter:
 * 		key : The target of context words to be stored
 * 		frequency: Total frequency count
 * Purpose: 	insert or add a word with its frequency count in hashtable
 * Return:	nothing
 * 
 * @param Key 
 * @param Data 
 */
void HashTable::Insert(const string Key, const int Data) {
   if (used >= size) {
      cerr << "The hashtable is full; cannot insert.\n";
      return;
   }

   unsigned long Index = Find(Key);

   // If not already in the table, insert it
   if (hashtable[Index].key == "") {
      hashtable[Index].key = Key;
      hashtable[Index].data = Data;
      used++;
   }
   // else do nothing
   else {
      hashtable[Index].data++;
   }
}

/**
 * @brief 
 * 
 * Parameters:  Set the hashtable to so that each bucket contains "", 0
 * Purpose:     initialize the hashtable for use
 * Returns:     nothing
 * 
 */
void HashTable::Init() {
   collisions = 0;
   lookups = 0;
   used = 0;

   // initialize the hashtable
   for (unsigned long i = 0; i < size; i++) {
      hashtable[i].key = "";
      hashtable[i].data = 0;
   }
}

/**
 * @brief 
 * 
 * Parameters:	key: the string
 * Purpose:	return the data or -1 if Key is not found
 * Return:	return an int
 * 
 * @param Key 
 * @return int 
 */
int HashTable::GetData(const string Key) {
   unsigned long Index;

   lookups++;
   Index = Find(Key);

   if (hashtable[Index].key == "") {
      return -1;
   }
   
   return hashtable[Index].data;
}

/**
 * @brief 
 * 
 * Parameters:	None
 * Purpose:	return the number of collisions
 * Return:	return a char *
 * 
 * @param Used 
 * @param Collisions 
 * @param Lookups 
 */
void HashTable::GetUsage(int &Used, int &Collisions, int &Lookups) const {
   Used = used;
   Collisions = collisions;
   Lookups = lookups;
}

/*-------------------------- Private Functions ----------------------------*/

/**
 * @brief 
 * 
 * Parameters:  key: the word to be located
 * Purpose:     return the index of the word in the table, or
 *              the index of the free space in which to store the word
 * Returns:     index of the word's actual or desired location
 * 
 * @param Key 
 * @return unsigned long 
 */
unsigned long HashTable::Find(const string Key) {
   unsigned long Sum = 0;
   unsigned long Index;

   // add all the characters of the key together
   for (long unsigned i = 0; i < Key.length(); i++) {
      Sum = (Sum * 19) + Key[i]; // Mult sum by 19, add byte value of char
   }

   Index = Sum % size;

   // Check to see if word is in that location
   // If not there, do linear probing until word found or empty location found
   while (hashtable[Index].key != Key && hashtable[Index].key != "") {
      Index = (Index + 1) % size;
      collisions++;
   }
   
   return Index;
}
