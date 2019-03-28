#ifndef CPU_H_
#define CPU_H_

#include "Player.h"
#include "Dictionary.h"
#include "Board.h"
#include "Move.h"
#include "Trie.h"
#include "Tile.h"
#include "Bag.h"
#include <string>

/* CPU class, an abstract class that publicly inherits from Player */
class CPU: public Player
{
public:
	CPU();
	CPU(std::string const & name, size_t maxTiles);
	~CPU();
	void constructTrie(Dictionary* gameDictionary);
	virtual Move* getMove (Board& gameBoard, const Bag& gameBag) = 0;

protected:
	TrieSet gameTrie;
	std::vector<std::pair<PlaceMove, int>> legalMoves;
	int recordUpRow;
	int recordDownRow;
	int recordLeftColumn;
	int recordRightColumn;

};

#endif