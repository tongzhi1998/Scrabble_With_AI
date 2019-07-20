#include "../lib/Trie.h"
#include <vector>
#include <iostream>
#include <cstring>

using namespace std;

/* Constructor for TrieNode */
TrieNode::TrieNode(char val, bool indic, TrieNode* parent)
{
	this->parent = parent;
	this->value = val;
	this->inset = indic;
	for (int i=0;i<26;i++)
	{
		children[i]=NULL;
	}
}

/* A helper function that is used to set the children of a specific TrieNode */
void TrieNode::setChildren(TrieNode* child, int pos)
{
	this->children[pos] = child;
}

/* Constructor for TrieSet */
TrieSet::TrieSet()
{
	this->start = new TrieNode (' ',false,NULL);
}

/* Destructor for TrieSet */
TrieSet::~TrieSet()
{
	clear (start);
}

/* Helper function used in the destructor */
void TrieSet::clear(TrieNode* curr)
{
	if (curr==NULL)
		return;
	for (int i=0;i<26;i++)
	{
		clear (curr->children[i]);
	}
	delete curr;
}

/* Insert function for trie */
void TrieSet::insert(std::string input)
{
	unsigned int i=0;
	TrieNode* temp = start;
	TrieNode* tempParent = NULL;
	/* While we haven't traversed down to the bottom or haven't consumed all the input */
	while (temp!=NULL && input[i]!='\0')
	{
		tempParent = temp;
		temp = temp->children[input[i]-'a'];
		i++;// we move forward
	}
	i--;
	if (temp!=NULL){
		temp->inset = true;
		return;
	} 
	while (i<input.length())
	{
		if (i==input.length()-1) // insert the word node
			tempParent->setChildren(new TrieNode(input[i],true,tempParent),input[i]-'a');
		else // insert the intermediate nodes
			tempParent->setChildren(new TrieNode(input[i],false,tempParent),input[i]-'a');
		tempParent = tempParent->children[input[i]-'a'];
		i++;
	}
}

/* Remove function: remove the given word from the trie */
void TrieSet::remove(std::string input)
{
	if (prefix(input)==NULL) // if the given word doesn't exist, we return
		return;
	TrieNode * temp = prefix(input);
	TrieNode* tempRecord = temp;
	bool proceed = true;
	/* we delete all the way up the trie until reaching a node that has more than 1 children */
	while (proceed)
	{
		for (unsigned int j=0;j<26;j++)
		{
			if (temp->children[j]!=NULL)
			{
				if (temp->inset==true&&temp==tempRecord){
					temp->inset = false;
				}
				proceed = false;
				break;
			}

		}
		if (proceed == false)
			break;
		TrieNode* tempParent = temp->parent;
		if (tempParent==NULL)
			break;
		tempParent->setChildren(NULL,temp->value-'a');
		delete temp;
		temp = tempParent;
	}
}

/* This function returns the node of the prefix */
TrieNode* TrieSet::prefix(std::string px)
{
	TrieNode* temp = start;
	unsigned int i=0;
	while (temp!=NULL&&px[i]!='\0')
	{
		temp = temp->children[tolower(px[i])-'a'];
		i++;
	}
	if (temp!=NULL)
		return temp;
	else
		return NULL;
}
