/*
To compile: flex multiplefiles.lex
            g++ -o multiplefiles lex.yy.c -lfl

Flex can also use gcc or cc instead of g++
Takes in and out directories: ./multiplefiles <indir> <outdir>
*/
%{
#include <string.h>
#include <dirent.h>
#include <iostream>
#include <math.h>
extern int yylex(void);
using namespace std;
#include "hashtable.cpp"
#include "globalHashTable.cpp"
#include "highFreqHashTable.cpp"

#undef yywrap // safety measure in case using old flex 

//---------  INSERT ANY CODE CALLED BY THE LEX RULES HERE --------
const int NUM_UNIQUE_TOKENS = 50000;

// Document & Global Hash functions, as well as the high frequency hash table
HashTable DocHt(10000);
GlobalHashTable GlobalHt(NUM_UNIQUE_TOKENS);
HighFreqHashTable HighFreqHt(150);

// Global Tokens
int totalTokens;
int docTokens;
int docId = 0;

// Variables and function for writing fixed records for map file
const int FILENAME_SIZE = 10;
const int MAP_REC_SIZE = FILENAME_SIZE + 1;
int num_records = 0;

void writeMapRecord(ofstream &fpout, const string fileName) {    
   fpout << setw(FILENAME_SIZE) << fileName.substr(0, min((int)fileName.length(), FILENAME_SIZE)) << "\n";
   num_records += 1;
}

// Function to fill the High Frequency Hash Table
void fillHighFreq(){
   HighFreqHt.FillTable("stopwords.txt");
}

/*
Example function definition used by a lex rule
You probably don't want to do these couts in general
but okay for debugging a few small input files
*/
void insertIntoHash(const char Str[]){
   string downcased;
   for (int i = 0; i < (int)strlen(Str); i++) {
      char character = Str[i];

      if ('A' <= character && character <= 'Z') {
         character = 'a' + character - 'A';
      }

      downcased.push_back(character);
   } 

   if (downcased.length() != 1) {
      totalTokens++;
      docTokens++;

      // Don't insert, one of the high frequency words
      if (HighFreqHt.GetData(downcased) != 1) {
         DocHt.Insert(downcased, 1);
      }
   }
}

// function called to insert document hash table items into global inside yywrap 
void ProcessDocumentHashtable(const char OutFile[]) {
   unsigned long size = DocHt.GetSize();

   for (unsigned long i = 0; i < size; i++) {
       if (DocHt.GetKey(i) != "") {
         string key = DocHt.GetKey(i);
         float freq = DocHt.GetData(key);
         float rtf = freq / docTokens;
         // Inserting into global hash table
         GlobalHt.Insert(key, rtf, docId, freq);
         
       }
   }

   docId++; // Increasing the docId after writing to global hash table
}

%}

/*----------------------------------------------------------*/
/*Section of code which specifies lex substitution patterns */
/*----------------------------------------------------------*/

DIGIT			   [0-9]
LETTER			[A-Za-z]
UPPERCASE		[A-Z]
LOWERCASE		[a-z]
ALPHANUM		   [A-Za-z0-9]

WORD			   [A-Za-z][a-z]*
UPPERWORD		[A-Z0-9][A-Z0-9]*
HYPHENWORD		[A-Za-z]+(\-[A-Za-z]+)+
ABBREVIATION	[A-Za-z]+\.([A-Za-z]+\.)+

NUMBER 			[0-9]+(,[0-9]+)*
FLOAT 			[0-9]*\.[0-9]+
PHONENUMBER		[0-9]{3}-[0-9]{3}-[0-9]{4}
TIME			   [0-9]{1,2}(:[0-9]{2})+
VERSION			[0-9]+\.[0-9]+(\.[0-9]+)+

URL				((http)|(ftp))s?:\/\/[A-Za-z0-9]+([\-\.]{1}[A-Za-z0-9]+)*\.[A-Za-z0-9]{2,}(:[0-9]{1,})?(\/[A-Za-z0-9_~\.\-]*)*
EMAIL 			[A-Za-z0-9_\-\.]+@([A-Za-z0-9_\-]+\.)+[A-Za-z0-9_\-]{2,4}

ATTRIBUTE 		[ \n\t]+(([A-Za-z\-_]+)?[ \n\t]*=?[ \n\t]*((\"[^\"]*\")|([A-Za-z0-9]+)|({URL}))[ \n\t]*)+[ \n\t]*
STARTTAG 		<!?[A-Za-z0-9]+{ATTRIBUTE}*[\/]?>
ENDTAG 			<[\/][A-Za-z0-9]+>

%%
{EMAIL}			insertIntoHash(yytext); 
{URL}				insertIntoHash(yytext);
{PHONENUMBER}	insertIntoHash(yytext);
{FLOAT}			; 
{NUMBER}			; 
{TIME}			insertIntoHash(yytext); 
{VERSION}		insertIntoHash(yytext); 
{STARTTAG}		;
{ENDTAG}			;
{UPPERWORD}		insertIntoHash(yytext); 
{ABBREVIATION}	insertIntoHash(yytext); 
{HYPHENWORD}	insertIntoHash(yytext); 
{WORD}			insertIntoHash(yytext); 
[\n\t ]			;
.				   ;
%%

// global variables for the input directory pointer
// and output directory name for use in yywrap
DIR *InputDirPtr = NULL;
string InDirname;
string OutDirname;
bool OutputFileIsOpen = false;
string globInFilename;
ofstream mapFile;

// This is called once per file.
int yywrap() {
   struct dirent* InputDirEntryPtr;
   string InFilename;
   string OutFilename;

   // if there was an input file open 
   // close it to get ready for the next file 
   if (yyin != NULL) {
      fclose (yyin);
      yyin = NULL;
      //***  Just closed the file
      
      //Redo naming scheme here
      OutFilename = OutDirname + "/" + globInFilename[0] + ".txt";
      const char * charFile = OutFilename.c_str();

      //Write current file name to the map file
      writeMapRecord(mapFile, globInFilename);

      //Calling function to insert into global hash table
      ProcessDocumentHashtable(charFile);
      docTokens = 0;
   }

   // if there was an output file open
   // close it to get ready for the next file 
   if (OutputFileIsOpen) {
      fclose(yyout);
      OutputFileIsOpen = false;
   }

   //Reset Document hashtable
   DocHt.Init();

   // skip over the hidden filenames that begin with dot
   do {
      InputDirEntryPtr = readdir(InputDirPtr);
   }
   while (InputDirEntryPtr != NULL && InputDirEntryPtr->d_name[0] == '.');

   // if there are still files to process
   if (InputDirEntryPtr != NULL) {
      // open the next file in the list as yyin
      InFilename = InDirname + '/'+ InputDirEntryPtr->d_name;
      //cout << "\nProcessing file " << InFilename << "\n";
      globInFilename = InputDirEntryPtr->d_name;
      yyin = fopen (InFilename.c_str(), "r");

      // if file open failed, print an error
      if (yyin == NULL) {
         perror(InFilename.c_str());
      }
   }
   // if yyin is NULL, return 1, else return 0
   return yyin == NULL;
}

int main(int argc, char **argv) {
   fillHighFreq();
   DIR *OutputDirPtr = NULL;

   if (argc != 3) {
      cerr << "Incorrect number of arguments.\n";
      return 1;
   }

   InDirname = argv[1];
   OutDirname = argv[2];
   InputDirPtr = opendir(InDirname.c_str());

   // open the input directory
   if (!InputDirPtr) {
      cerr << "Unable to open input directory: " << InDirname << "\n";
   }
   else {
      // open or create the output directory
      OutputDirPtr = opendir (OutDirname.c_str());
      if (!OutputDirPtr) {
         string Command = "mkdir -p " + OutDirname;
         system(Command.c_str());
      }

      // initialize the input and output file pointers
      yyin = NULL;
      yyout = NULL;

      // set up the inverted files
      string map = OutDirname + "/map.txt";
      string dict = OutDirname + "/dict.txt";
      string post = OutDirname + "/post.txt";
      mapFile.open(map);

      // call yywrap and yylex to process the files
      yywrap();
      yylex();

      //cout << "Done tokenizing.  Good place to write the dict and post files.\n";
      cout << "Total Tokens: " << totalTokens << "\n";
      mapFile.close();

      GlobalHt.WriteToFiles(dict, post, docId);

      // close the input and output directories
      (void) closedir(InputDirPtr);
      (void) closedir(OutputDirPtr);
   }
}
