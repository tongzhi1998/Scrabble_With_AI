#include "Move.h"
#include "Board.h"
#include "Player.h"
#include "Bag.h"
#include "Square.h"
#include "ConsolePrinter.h"
#include "Exceptions.h"
#include "Tile.h"
#include "Game.h"
#include "Dictionary.h"
#include "CPU.h"
#include "CPUMaxTiles.h"
#include "CPUMaxScore.h"
#include "Trie.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <set>
#include <vector>

using namespace std;

/* Constructor for Game, taking the configuration file, names of players and number of players as parameters
Will create dynamically allocated gamePlayers, gameBag, gameBoard and gameDictionary accordingly*/
Game::Game (std::string configuration_file_name, std::string names[], size_t totalPlayer)
{
	round = 0;//set the number of played round to 0
	this->totalPlayer = totalPlayer;
	gamePlayer = new Player*[totalPlayer];//dynamically allocate the gamePlayer array
	ifstream ifile (configuration_file_name);
	string line;
	string bagFilename;
	string boardFilename;
	string initState;
	bool hasInit = false;
	uint32_t seed;
	size_t maxsize;
	while (getline(ifile,line))// read all the stuff in the configuration file, and then generate all the required stuff accordingly
	{
		stringstream ssline(line);
		string Type;
		ssline>>Type;
		if (Type=="HANDSIZE:")// dynamically allocate gamePlayer
		{
			ssline>>maxsize;
			for (unsigned int i=0;i<(this->totalPlayer);i++)
			{
				string firstFour;
				for (unsigned int j=0;j<4;j++)
				{
					firstFour+=toupper(names[i][j]);// get the first four letter input from the user
				}
				if (firstFour == "CPUL")
				{
					gamePlayer[i] = new CPUMaxTiles(names[i],maxsize);// create a CPUL player
				}
				else if (firstFour=="CPUS")
				{
					gamePlayer[i] = new CPUMaxScore(names[i],maxsize);// create a CPUS player
				}
				else
					gamePlayer[i] = new Player(names[i],maxsize);
			}
		}
		else if (Type=="BOARD:")// dynamically allocate gameBoard
		{
			ssline>>boardFilename;
		}
		else if (Type=="TILES:")// read the bag's filename
		{
			ssline>>bagFilename;
		}
		else if (Type=="DICTIONARY:")// dynamically allocate the dictionary
		{
			string filename;
			ssline>>filename;
			gameDictionary = new Dictionary (filename);
		}
		else if (Type=="INIT:")// try to read the initialization file
		{
			hasInit = true;
			ssline>>initState;
		}
		else if (Type=="SEED:")// read the random seed
		{
			ssline>>seed;
		}
	}
	if (hasInit==true)
		gameBoard = new Board(boardFilename,initState);//dynamically allocate the board with init given
	else
		gameBoard = new Board(boardFilename);// dynamically allocate the board without init
	gameBag = new Bag (bagFilename,seed);// dynamically allocate the bag
	for (unsigned int i=0;i<this->totalPlayer;i++)// Let each player draw tiles
	{
		gamePlayer[i]->addTiles(gameBag->drawTiles(maxsize));
	}
	/* Initialize the part Trie for both CPUL and CPUS */
	for (unsigned int i=0;i<(this->totalPlayer);i++)
	{
		if (gamePlayer[i]->getName() == "CPUL")
		{
			static_cast<CPUMaxTiles*>(gamePlayer[i])->constructTrie(gameDictionary);
		}
		if (gamePlayer[i]->getName() == "CPUS")
		{
			static_cast<CPUMaxTiles*>(gamePlayer[i])->constructTrie(gameDictionary);
		}
	}
}

/* This function simulates a single round of game*/
void Game::oneRound (Player* player)
{
	round++;// increase round by 1
	cout << player->getName() << "'s turn now!!" << endl;
	cout << "This is the status of game board now" << endl;
	gameConsolePrinter->printBoard(*gameBoard);
	cout << "These are the available tiles in your hand" << endl;
	gameConsolePrinter->printHand(*player);
	cout << "This is the list of all players' current scores" << endl;// print all necessary information
	for (unsigned int i=0;i<totalPlayer;i++)
	{
		cout << gamePlayer[i]->getName() << ": " << gamePlayer[i]->getScore() << endl;
	}
	size_t currScore = player->getScore();
	string move;// this the the player's move string
	Move *m;
	bool success = false;
	while(success==false)
	{
		if (player->getName()=="CPUL") // if the player is CPUL
		{
			m = static_cast<CPUMaxTiles*>(player)->getMove(*gameBoard,*gameBag);
			if (m->isPass()==true)
				player->setPass(true);
			else
				player->setPass(false);
			m->execute(*gameBoard,*gameBag,*gameDictionary);
			success = true;
		}
		else if (player->getName()=="CPUS") // if the player is CPUS
		{
			m = static_cast<CPUMaxTiles*>(player)->getMove(*gameBoard,*gameBag);
			if (m->isPass()==true)
				player->setPass(true);
			else
				player->setPass(false);
			m->execute(*gameBoard,*gameBag,*gameDictionary);
			success = true;
		}
		else
		{
			cout << player->getName()<<" Please enter your desired move:" << endl;
			getline(cin,move);// read the move string
			try// if we cannot create a Move pointer, meaning there is no required tiles in player's hand, we go to the second catch
			{
				m = m->parseMove (move, *player);
				success = true;
				if (m->isPass()==true)
					player->setPass(true);
				else
					player->setPass(false);
				try// try to execute the move to the board
				{
					m->execute (*gameBoard,*gameBag,*gameDictionary);
					success=true;// if we can do it successfully, then set success to true
				}
				catch (logic_error &e)// if we cannot execute, we output the error message, set success tp false and delete the move string
				{
					cout << "Error: " << e.what() << endl;
					success = false;
					delete m;
				}
			}
			catch(logic_error &e)// we output the error message and set success to false 
			{
				cout << "Error: " << e.what() << endl;
				success = false;
			}
		}
	}
	size_t finalScore = player->getScore();
	size_t pointsEarned = finalScore-currScore;
	cout << "This is the status of the board after your move" << endl;
	gameConsolePrinter->printBoard(*gameBoard);
	cout << "These are your current hand tiles:" << endl;
	gameConsolePrinter->printHand(*player);
	cout << "You earned "<< pointsEarned << " in this turn!" << endl;// other necessary printouts
	cout << endl;
	delete m;
	if (isFinished()==false)// if the game is not finished, we wait the user to press enter to resume the game
		cin.ignore (256,'\n');
}

/* This function is used to judge whether the game has finished or not*/
bool Game::isFinished () const
{
	if (gameBag->tilesRemaining()==0)// if there is no tiles in the bag and at least one player's hand is empty, the game finishes
	{
		for (unsigned int i=0;i<totalPlayer;i++)
		{
			if (gamePlayer[i]->isEmpty()==true){
				return true;
			}
		}
	}
	unsigned int count = 0;
	for (unsigned int i=0;i<totalPlayer;i++)// if all players choose to pass, the game finishes
	{
		if (gamePlayer[i]->getPass()==true)
		{
			count++;
		}
	}
	if (count>=totalPlayer){
		return true;
	}
	return false;
}

/* This function is used to decide the final winner and print all players' scores out*/
void Game::finalResult () const
{
	size_t maxScore = gamePlayer[0]->getScore();
	size_t posRecord[totalPlayer];
	posRecord[0] = 0;
	size_t count = 1;
	for (unsigned int i=1; i<totalPlayer;i++)// we use this for loop to find the highest scores and the players
	{
		if (gamePlayer[i]->getScore()>int(maxScore))
		{
			count = 1;
			posRecord[0] = i;
			maxScore = gamePlayer[i]->getScore();
		}
		else if (gamePlayer[i]->getScore()==int(maxScore))
		{
			count++;
			posRecord[count-1]=i;
		}
	}
	cout << "Winners: ";
	for (unsigned int i=0;i<count;i++)
	{
		size_t temp = posRecord[i];
		cout << gamePlayer[temp]->getName() << "  ";
	}
	cout << endl;
	cout << "All players' scores are listed below" << endl;// do the necessary output
	for (unsigned int i=0;i<totalPlayer;i++)
	{
		cout << gamePlayer[i]->getName() << ": " << gamePlayer[i]->getScore() << endl;
	}
}

/* This function is used to calculate the score for the final round*/
void Game::finalRound ()
{
	unsigned int totalPoint = 0;// set the totalpoint that the player with no tile is going to add to 0
	unsigned int count;
	bool entered = false;
	for (unsigned int i=0;i<totalPlayer;i++)
	{
		if (gamePlayer[i]->getTilesRemaining()==0)// if one player has emptyed his hand
		{
			entered = true;
			count = i;
		}
		set<Tile*> temp = gamePlayer[i]->getHandTiles();
		int scoreToSubstract = 0;// this is the score that player with hand tiles need to substract 
		for (set<Tile*>::iterator tileIter = temp.begin();tileIter!=temp.end();tileIter++)
		{
			scoreToSubstract+=(*tileIter)->getPoints();
		}
		if (gamePlayer[i]->getScore()>scoreToSubstract)
		{
			gamePlayer[i]->addScore (-scoreToSubstract);
		}
		else
		{
			gamePlayer[i]->addScore(-(gamePlayer[i]->getScore()));
		}
		totalPoint+=scoreToSubstract;
	}
	if (entered == true)// if a player has no tiles, we add the total amount of substracted score to his score
		gamePlayer[count]->addScore(int(totalPoint));
}

/* This function is used to play the game until it is finished*/
void Game::playGame ()
{
	while (isFinished()==false)// while the game is not finished, we play it one round after another
	{
		for (unsigned int i=0;i<totalPlayer;i++)
		{
			oneRound(gamePlayer[i]);
			if (isFinished()==true)
			{
				finalRound();
				finalResult();
				cout << "This is the end of the game!" << endl;
				break; 
			}
		}
	}
}

/* Destructor for Game, delete all the dynamically allocated stuff*/
Game::~Game()
{
	delete gameDictionary;
	delete gameBag;
	delete gameBoard;
	for (unsigned int i=0; i<totalPlayer;i++)
	{
		delete gamePlayer[i];
	} 
	delete []gamePlayer;
}