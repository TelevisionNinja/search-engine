/*
Purpose:   The implementation file for a hash table of words and numbers
*/
#include <assert.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <cmath>
#include <algorithm>
#include <iomanip>

#include "globalHashTable.h"

using namespace std;

/*-------------------------- Constructors/Destructors ----------------------*/

/**
 * @brief Construct a new Global Hash Table:: Global Hash Table object
 * 
 * Parameters:  ht: the hashtable to copy
 * Purpose:     copy a hashtable 
 *              NOTE:  this is crucial to include since it is invoked
 *              by call-by-value parameter passing
 * Returns:     nothing
 * 
 * @param ht 
 */
GlobalHashTable::GlobalHashTable(const GlobalHashTable &ht) {
   size = ht.size; // set the size of the array
   used = ht.used; // set the size of the array
   if ((hashtable = new StringIntPair[size]) == NULL) {
      cout << "Out of memory at HashTable::HashTable(const HashTable)\n";
   }

   assert(hashtable != 0);

   for (unsigned long i = 0; i < size; i++) { // make a _copy_ of the array elements
      hashtable[i].key = ht.hashtable[i].key;
      hashtable[i].docFreq = ht.hashtable[i].docFreq;
      hashtable[i].linkedList = ht.hashtable[i].linkedList;
   }
}
           
/**
 * @brief Construct a new Global Hash Table:: Global Hash Table object
 * 
 * Parameters:  none
 * Purpose:     allocate a hashtable for an expected number of keys
 *              initializes all values to null (0)
 * Returns:     pointer to the created HashTable or 0 if out of memory
 * 
 * @param NumKeys 
 */
GlobalHashTable::GlobalHashTable(const unsigned long NumKeys) {
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
      hashtable[i].docFreq = 0;
      hashtable[i].linkedList;
   }
}

/**
 * @brief Destroy the Global Hash Table:: Global Hash Table object
 * 
 * Parameters:  none
 * Purpose:     deallocate a hash table
 * Returns:     nothing
 * 
 */
GlobalHashTable::~GlobalHashTable() {
   delete [] hashtable;
}

/*-------------------------- Accessors ------------------------------------*/

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
void GlobalHashTable::Print(const char *filename) const {
   ofstream fpout(filename); 

   // Print out the non-zero contents of the hashtable
   for (unsigned long i = 0; i < size; i++) {  
      if (hashtable[i].key != "") {
         fpout << hashtable[i].key << " " << hashtable[i].docFreq << "\n";
      }
   }

   fpout.close();
}

/**
 * @brief Function to write to the dict file, creating fixed file sizes
 * 
 * @param fpout 
 * @param token 
 * @param docFreq 
 * @param start 
 */
void GlobalHashTable::WriteToDict(ofstream& fpout, string token, int docFreq, int start) {
   // Line to write the token  
   fpout << setw(TOKEN_SIZE) << token.substr(0, min((int)token.length(), TOKEN_SIZE)) << " ";
    
   // Lines to write the docFreq param
   int tempfreq = min (docFreq, (int)(pow(10, DOC_FREQ_SIZE) - 1));
   fpout << setw(DOC_FREQ_SIZE) << tempfreq << " ";    

   // Lines to write the start param
   int tempstart = min (start, (int)(pow(10, START_SIZE) - 1));
   fpout << setw(START_SIZE) << tempstart << "\n";

   num_records++;
}

/**
 * @brief Function to call specific file write functions
 * 
 * @param dict 
 * @param post 
 * @param numDocs 
 */
void GlobalHashTable::WriteToFiles(const string dict, const string post, const int numDocs) {
   ofstream dictout(dict.c_str());
   ofstream postout(post.c_str());
   int start = 0;

   for (unsigned long i = 0; i < size; i++) {
      if (hashtable[i].key == "") {
         // Low frequency
         if (hashtable[i].docFreq == 1 && hashtable[i].linkedList.IsLowFreq()) {
            WriteToDict(dictout, "empty", -1, -1);
         }
         else {
            // Writing to dict file
            WriteToDict(dictout, hashtable[i].key, hashtable[i].docFreq, start);
            start += hashtable[i].docFreq;

            // Calculate idf, add param for idf in write to post, chance 4 to variable, change .data to doc freq
            double idf = 1 + log10(numDocs / hashtable[i].docFreq);

            // Write to post file
            hashtable[i].linkedList.WriteToPost(postout, idf); // hashtable[i].key
         }
      }
      else {
         WriteToDict(dictout, "empty", -1, -1);
      }
   }

   dictout.close();
   postout.close();
}

/**
 * @brief The insert function for the global hash table, which handles the linked lists //current Data is the rtf
 * 
 * Parameter:
 * 		key : The target of context words to be stored
 * 		frequency: Total frequency count
 * Purpose: 	insert or add a word with its frequency count in hashtable
 * Return:	nothing
 * 
 * @param Key 
 * @param rtf 
 * @param docId 
 * @param freq 
 */
void GlobalHashTable::Insert(const string Key, const float rtf, int docId, int freq) {
   unsigned long Index;

   if (used >= size) {
      cerr << "The hashtable is full; cannot insert.\n";
      return;
   }
   
   Index = Find(Key);

   // If not already in the table, insert it
   if (hashtable[Index].key == "") {
      hashtable[Index].key = Key;
      hashtable[Index].docFreq = 1; //document , change name
      hashtable[Index].linkedList.AddToEnd(docId, rtf, freq);
      used++;
   }
   // else do nothing ** change this comment to adding to linked list
   else {
      hashtable[Index].docFreq++; // increase num documents, need to change var names
      hashtable[Index].linkedList.AddToEnd(docId, rtf, freq);
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
void GlobalHashTable::Init() {
   collisions = 0;
   lookups = 0;
   used = 0;

   // initialize the hashtable
   for (unsigned long i = 0; i < size; i++) {
      hashtable[i].key = "";
      hashtable[i].docFreq = 0;
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
int GlobalHashTable::GetData(const string Key) {
   lookups++; 
   unsigned long Index = Find(Key);

   if (hashtable[Index].key == "") {
      return -1;
   }
   
   return hashtable[Index].docFreq;
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
void GlobalHashTable::GetUsage(int &Used, int &Collisions, int &Lookups) const {
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
unsigned long GlobalHashTable::Find(const string Key) {
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
