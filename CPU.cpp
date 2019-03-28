#include "CPU.h"
#include "Dictionary.h"
#include "Player.h"
#include "Move.h"
#include "Bag.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

CPU::CPU()
{

}

CPU::CPU(std::string const & name, size_t maxTiles)
{
	p_name = name;
	score = 0;
	handSize = maxTiles;
	isPass = false;
}

CPU::~CPU()
{

}

void CPU::constructTrie(Dictionary* gameDictionary)
{
	std::set<std::string> tempWords = gameDictionary->allWords();
	for (std::set<std::string>::iterator setIter = tempWords.begin();setIter!=tempWords.end();setIter++)
	{
		gameTrie.insert(*setIter);
	}
}