#ifndef CPUMAXSCORE_H_
#define CPUMAXSCORE_H_

#include "CPU.h"
#include "Move.h"
#include "Board.h"
#include <string>

/* This is CPUS, which publicly inherits from CPU*/
class CPUMaxScore: public CPU
{
public:
	CPUMaxScore(std::string const & name, size_t maxTiles);
	~CPUMaxScore();
	virtual Move* getMove (Board& gameBoard, const Bag& gameBag);
	void horizontalHelper (int startRow, int startCol, int currRow, int currCol, std::string placed, std::string temp, std::string inHand, Board& gameBoard, const Bag& gameBag);
	void verticalHelper (int startRow, int startCol, int currRow, int currCol, std::string placed, std::string temp, std::string inHand, Board& gameBoard, const Bag& gameBag);
	std::string getName() const;
	void printAllMoves();
};

#endif