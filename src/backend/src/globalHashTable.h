/* Filename:  hashtable.h
 * Author:    Susan Gauch
 * Date:      2/25/10
 * Purpose:   The header file for a hash table of strings and ints. 
*/

using namespace std;
#include <string>
#include "linkedList.h"

class GlobalHashTable {
   public:
      GlobalHashTable(const GlobalHashTable& ht ); // constructor for a copy
      GlobalHashTable(const unsigned long NumKeys); // constructor of hashtable
      ~GlobalHashTable(); // destructor
      void Print(const char *filename) const;
      void WriteToFiles(const string dict, const string post, const int numDocs); //move to global hash class
      void Insert(const string Key, const float rtf, int docId, int freq); //document hash function
      int GetData(const string Key); 
      void GetUsage(int &Used, int &Collisions, int &Lookups) const;
      void Init();
      void WriteToDict(ofstream& fpout, const string token, const int docFreq, const int start);
   protected:
      struct StringIntPair { // the datatype stored in the hashtable
         string key;
         int docFreq;
         List <int> linkedList; // get rid of in document hash function
      };
      unsigned long Find(const string Key); // the index of the ddr in the hashtable
   private:
      StringIntPair *hashtable; // the hashtable array itself
      unsigned long size; // the hashtable size
      unsigned long used;
      unsigned long collisions;
      unsigned long lookups;
      const int TOKEN_SIZE = 14;
      const int DOC_FREQ_SIZE = 4;
      const int START_SIZE = 7;
      const int RECORD_SIZE = TOKEN_SIZE + DOC_FREQ_SIZE + START_SIZE + 3;
      int num_records = 0;
};
