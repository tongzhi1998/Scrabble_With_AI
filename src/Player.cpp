#include "../lib/Player.h"
#include <set>
#include <vector>
#include <string>
#include <cstring>

using namespace std;

Player::Player()
{

}

/* Constructor for Player, set the name, maxhandsize and the initial tile of the player */
Player::Player (std::string const & name, size_t maxTiles)
{
	p_name = name;
	score = 0;
	handSize = maxTiles;
	isPass = false;// construct the Player's class, initialize all the private members
}

/* Destructor for Player, delete the set of tile pointers that the player has*/
Player::~Player ()
{
	for (std::set <Tile*>::iterator tileIter = hand.begin(); tileIter!=hand.end();tileIter++)
	{
		delete *tileIter;
	}
}

/* return the set of tile pointers in the player's hand*/
std::set <Tile*> Player::getHandTiles () const
{
	return hand;
}

/* check if the player has a specific string of tiles or not*/
bool Player::hasTiles(std::string const & move, bool resolveBlanks) const
{
	set <Tile*> temp;
	for (set<Tile*>::iterator tileIter = hand.begin();tileIter!=hand.end();tileIter++)
	{
		temp.insert(*tileIter);//deep copy the set of tiles to temp
	}
	if (resolveBlanks==true) // if the move is a Placemove
	{
		for (unsigned int i=0;i<move.length();i++) // loop over the string
		{
			bool found = false;
			for (std::set<Tile*>::iterator tileIter = temp.begin();tileIter!=temp.end();tileIter++)// use the tile iterator to search for player's hand
			{
				if (tolower((*tileIter)->getLetter())==move[i]||toupper((*tileIter)->getLetter())==move[i])// if we find the letter in the player's hand
				{
					if (move[i]=='?') // if it is a blank
					{
						i++;// move forward one position
						if (i>=move.length())// if ? is the last letter in the placemove, we return false
							return false;
						//tileIter->useAs(move[i]);// record what the blank is used as
					}
					found = true;// set found = true
					temp.erase(tileIter);
					break;
				}
			}
			if (found==false)// if we cannot find that specific letter in the player's hand
				return false;// return false
		}
		return true;// if we successfully find all letters in the string in the player's hand, then return true
	}
	else // if the move is a Exchange move
	{
		for (unsigned int i=0;i<move.length();i++)
		{
			bool found = false;
			for (std::set<Tile*>::iterator tileIter = temp.begin();tileIter!=temp.end();tileIter++)
			{
				if (tolower((*tileIter)->getLetter())==move[i]||toupper((*tileIter)->getLetter())==move[i])
				{
					found = true;
					temp.erase(tileIter);
					break;
				}
			}
			if (found==false)
				return false;
		}
		return true;
	}
}

/* Reads a move string, finds the corresponding tiles in the player's hand, and
   removes and returns those tiles in the order they appear in the move string.*/
std::vector<Tile*> Player::takeTiles (std::string const & move, bool resolveBlanks)
{
	std::vector<Tile*> tilesTaken;// this is the vector that is supposed to be returned
	if (resolveBlanks==true)
	{ 
		for (unsigned int i=0;i<move.length();i++)
		{
			for (set<Tile*>::iterator tileIter = hand.begin();tileIter!=hand.end();tileIter++)// create a set iterator
			{
				if (tolower((*tileIter)->getLetter())==move[i]||toupper((*tileIter)->getLetter())==move[i])// if the tile in the hand matches the tile required by the move
				{
					if (move[i]=='?')// if its a blank tile, we updates the true value it holds
					{
						i++;
						(*tileIter)->useAs(move[i]);
					}
					tilesTaken.push_back(*tileIter);// add the tile to the vector
					hand.erase(tileIter);// erase the tile from the player's hand
					break;
				}
			}
		}
	}
	else // if it is an exchange move (pretty much the same)
	{
		for (unsigned int i=0;i<move.length();i++)
		{
			for (set<Tile*>::iterator tileIter = hand.begin();tileIter!=hand.end();tileIter++)
			{
				if (tolower((*tileIter)->getLetter())==move[i]||toupper((*tileIter)->getLetter())==move[i])
				{
					tilesTaken.push_back(*tileIter);
					hand.erase(tileIter);
					break;
				}
			}
		}
	}
	return tilesTaken;// return the formed vector
}

/* Adds all the tiles in the vector to the player's hand */
void Player::addTiles (std::vector<Tile*> const & tilesToAdd)
{
	vector<Tile*> temp = tilesToAdd;
	for (std::vector<Tile*>::iterator tileIter = temp.begin(); tileIter!=temp.end(); tileIter++)
	{
		hand.insert(*tileIter);
	}
}

/* Updates the player's score*/
void Player::addScore ( int add)
{
	score+=add;
}

/* Check to see whether a player's hand is empty*/
bool Player::isEmpty() const
{
	if (hand.empty()==true)
		return true;
	return false;
}

/* Get player's score*/
int Player::getScore() const
{
	if (score>=0)
		return score;
	else
		return 0;
}

/* Get Player's name*/
std::string Player::getName () const
{
	return p_name;
}

/* Get the maximum number of tiles that a player can have*/
size_t Player::getMaxTiles () const
{
	return handSize;
}

/* Get the number of remaining tiles in a player's hand*/
size_t Player::getTilesRemaining() const
{
	return hand.size();
}

/* Change to pass condition*/
void Player::setPass (bool indicator)
{
	isPass = indicator;
}

/* Indicate whether the player has just made a pass move or not*/
bool Player::getPass () const
{
	return isPass;
}