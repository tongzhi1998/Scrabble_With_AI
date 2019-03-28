#ifndef TRIE_H
#define TRIE_H
#include <string>

struct TrieNode
{
	char value;
	bool inset;
	TrieNode* parent;
	TrieNode* children[26];
	TrieNode(char val, bool indic, TrieNode* parent);
	void setChildren(TrieNode* child, int pos);
};

class TrieSet
{
public:
	TrieSet();
	~TrieSet();
	void insert(std::string input);
	void remove(std::string input);
	TrieNode* prefix(std::string px);
	void clear(TrieNode* curr);

private:
	TrieNode* start;
};

#endif