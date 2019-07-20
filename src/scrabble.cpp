#include "../lib/Move.h"
#include "../lib/Board.h"
#include "../lib/Player.h"
#include "../lib/Bag.h"
#include "../lib/Square.h"
#include "../lib/ConsolePrinter.h"
#include "../lib/Exceptions.h"
#include "../lib/Tile.h"
#include "../lib/Game.h"
#include "../lib/Dictionary.h"
#include "../lib/CPU.h"
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