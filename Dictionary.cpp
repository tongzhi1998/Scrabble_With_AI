#include "Dictionary.h"
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

/* The constructor gets passed the name of the file from which
   to read the word list. */
Dictionary::Dictionary (std::string dictionary_file_name)
{	
	std::ifstream ifile(dictionary_file_name);
	if (ifile.fail())
	{
		cout << "Cannot open the file" << endl;
	}
	string word;
	while (ifile>>word)
	{
		for (unsigned int i=0;i<word.length();i++)// turn every word to lowercase and store them in dictionary
		{
			word[i]=tolower(word[i]);
		}
		dicWords.insert(word);
	}
}

/* We just use the default destructor*/
Dictionary::~Dictionary()
{}

/* Checks whether the given word is in the dictionary, and returns true if so. */
bool Dictionary::isLegalWord (std::string const & word) const
{
	set<string>::iterator strIter = dicWords.begin();// Use STL iterator to search (Should run in log(n))
	string realWord;
	for (unsigned int i=0;i<word.length();i++)// turn the read word into lowercase too to compare with words in the dictionary
	{
		realWord+=tolower(word[i]);
	}
	strIter = dicWords.find(realWord);// Use STL find function
	if (strIter==dicWords.end())// return false if not found, true if found
		return false;
	else
		return true;
}

set<string> Dictionary::allWords() 
{
	return dicWords;
}