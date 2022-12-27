/*
The header file for a hash table of strings and ints
*/
using namespace std;
#include <string>

class HighFreqHashTable {
   public:
      HighFreqHashTable(const HighFreqHashTable& ht); // constructor for a copy
      HighFreqHashTable(const unsigned long NumKeys); // constructor of hashtable
      ~HighFreqHashTable(); // destructor
      void Print() const;
      void Insert(const string Key); 
      int GetData(const string Key); 
      void GetUsage(int &Used, int &Collisions, int &Lookups) const;
      void FillTable(const char *filename);
      void Init();
   protected:
      struct StringIntPair { // the datatype stored in the hashtable
         string key;
      };
      unsigned long Find(const string Key); // the index of the ddr in the hashtable
   private:
      StringIntPair *hashtable; // the hashtable array itself
      unsigned long size; // the hashtable size
      unsigned long used;
      unsigned long collisions;
      unsigned long lookups;
};
