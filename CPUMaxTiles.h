#ifndef CPUMAXTILES_H_
#define CPUMAXTILES_H_

#include "CPU.h"
#include "Move.h"
#include "Board.h"
#include <string>

class CPUMaxTiles: public CPU
{
public:
	CPUMaxTiles(std::string const & name, size_t maxTiles);
	~CPUMaxTiles();
	Move* getMove (Board& gameBoard, const Bag& gameBag);
	void horizontalHelper (int startRow, int startCol, int currRow, int currCol, std::string placed, std::string temp, std::string inHand, Board& gameBoard, const Bag& gameBag);
	void verticalHelper (int startRow, int startCol, int currRow, int currCol, std::string placed, std::string temp, std::string inHand, Board& gameBoard, const Bag& gameBag);
	std::string getName() const;
	void printAllMoves() ;
};

#endif