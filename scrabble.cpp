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
#include <stdexcept>
#include <iostream>
#include <fstream>

using namespace std;

int main(int agrc, char* agrv[])
{
	if (agrc!=2)
	{
		cout << "Please provide the right number of arguments" << endl;
		return -1;
	}
	size_t playerNum;
	cout << "Please enter the number of players in the game:" << endl;
	cin>>playerNum;
	string names[playerNum];
	std::cin.ignore(256,'\n');
	for (unsigned int i=0;i<playerNum;i++)
	{
		cout << "Player " << i+1 << " please enter your name" << endl;
		getline(cin,names[i]);
	}
	//std::cin.ignore(256,'\n');
	Game myGame(agrv[1],names,playerNum);
    myGame.playGame();
	return 0;
}