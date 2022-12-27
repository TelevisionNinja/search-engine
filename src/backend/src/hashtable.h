/* Filename:  hashtable.h
 * Author:    Susan Gauch
 * Date:      2/25/10
 * Purpose:   The header file for a hash table of strings and ints. 
*/

using namespace std;
#include <string>
//#include "globalHashTable.cpp"
//#include "linkedList.h"

class HashTable {
   public:
      HashTable(const HashTable& ht ); // constructor for a copy
      HashTable(const unsigned long NumKeys); // constructor of hashtable
      ~HashTable(); // destructor
      void Print(const char *filename) const;
      void Insert(const string Key, const int Data);
      int GetData(const string Key);
      void GetUsage(int &Used, int &Collisions, int &Lookups) const;
      unsigned long GetSize();
      string GetKey(unsigned long i);
      void Init();
   protected:
      struct StringIntPair { // the datatype stored in the hashtable
         string key;
         int data;
      };
      unsigned long Find(const string Key); // the index of the ddr in the hashtable
   private:
      StringIntPair *hashtable; // the hashtable array itself
      unsigned long size; // the hashtable size
      unsigned long used;
      unsigned long collisions;
      unsigned long lookups;
};
