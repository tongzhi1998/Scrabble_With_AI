
#ifndef GAME_H_
#define GAME_H_

#include "Move.h"
#include "Board.h"
#include "Player.h"
#include "Bag.h"
#include "Square.h"
#include "ConsolePrinter.h"
#include "Exceptions.h"
#include "Tile.h"
#include "Dictionary.h"
#include <stdexcept>


class Game
{
public:
	Game(std::string configuration_file_name, std::string names[], size_t totalPlayer);
	void oneRound (Player* player);
	void displayStatus (Player* player);
	bool isFinished () const;
	void finalResult () const;
	void playGame ();
	void finalRound();
	~Game();

private:
	Player** gamePlayer;
	Board* gameBoard;
	Bag* gameBag;
	Dictionary* gameDictionary;
	ConsolePrinter* gameConsolePrinter;
	size_t totalPlayer;
	int round;

};

#endif