/* The Move class family encodes a move made by one player, including its type
   (PASS, EXCHANGE, PLACE), the tiles used (and use for blanks),
   start square, and direction (if needed).
   Call Move::parseMove() to turn an entered string into its subclass of
   Move, then call apply() on that returned move to have it execute.*/

#include "Move.h"
#include "Bag.h"
#include "Board.h"
#include "Dictionary.h"
#include "Tile.h"
#include "Player.h"
#include "Square.h"
#include <vector>
#include <set>
#include <map>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iterator>
#include <iostream>

using namespace std;

/* Parses the given move m, and constructs the corresponding move subclass.
   m is assumed to be in the format described on the assignment sheet for a move command.
   The player passed in is used to verify that the proposed tiles are
   in fact in the player's hand.
*/
Move * Move::parseMove(std::string moveString, Player &p)
{
	stringstream moveComb (moveString);// create a moveComb to store the moveString
	string moveType;// first get the type of the move
	moveComb>>moveType;
	for (unsigned int i = 0; i<moveType.length();i++)
	{
		moveType[i]=toupper(moveType[i]);// convert all string to uppercase
	}
	if (moveType=="PASS")// if it is a PASS move, then just return the pointer to a PassMove
	{
		return new PassMove(&p);
	}
	else if (moveType == "EXCHANGE")// if it is a exchange move
	{
		bool resolveBlanks = false;// we don't need to resolve Blanks in this case
		string playerMoveString;
		moveComb>>playerMoveString;// read the player's moveString
		if (p.hasTiles(playerMoveString,resolveBlanks)==false)// throw Exceptions here
		{
			 throw logic_error ("this is no such tile in your hand");
		}
		return new ExchangeMove (playerMoveString,&p);
	}
	else 
	{
		bool resolveBlanks = true;// we need to resolve blanks in this case
		string playerMoveString;
		bool horizontal;
		size_t row;
		size_t column;
		char direction;
		moveComb>>direction;// we first store the direction of the move
		if (direction == '-')
			horizontal = true;
		else
			horizontal = false;
		moveComb>>row;
		moveComb>>column;
		moveComb>>playerMoveString;// store all other information accordingly
		if (p.hasTiles(playerMoveString,resolveBlanks)==false)// if the player doesn't have certain tile in his hand, then throw error
			 throw logic_error ("There is no such tile in your hand");
		return new PlaceMove (row,column,horizontal,playerMoveString,&p);	
	}
	
}

/* Constructor for the Move class*/
Move::Move(Player * player)
{
	_player = player;
}

/* Destructor for Move class*/
Move::~Move()
{}


/* Constructs a pass move. */
PassMove::PassMove(Player* player): Move(player)
{
	_player = player;
}

/* execute the PassMove (which has no effect whatsoever)*/
void PassMove::execute(Board & board, Bag & bag, Dictionary & dictionary){}


/* Creates an EXCHANGE move, exchanging the tiles listed in the
   string  with new tiles from the bag.
*/
ExchangeMove::ExchangeMove (std::string tileString, Player*p): Move(p)
{
	_player = p;
	bool resolveBlanks = false;// we don't need to resolve blanks in this case
	exchangeTiles = p->takeTiles (tileString,resolveBlanks);// take tiles from player's hand
}

/* This fucntion returns the exhangeTiles vector */
std::vector<Tile*> const & ExchangeMove::tileVector () const
{
	return exchangeTiles;
}

/* Executes the exchange move */
void ExchangeMove::execute (Board & board, Bag & bag, Dictionary & dictionary)
{
	bag.addTiles(exchangeTiles);// add the tiles to the bag
	_player->addTiles(bag.drawTiles(exchangeTiles.size()));// draw the equal amount of tiles from the bag

	cout << "New tiles picked up: ";
	for (std::vector<Tile*>::iterator tileIter = exchangeTiles.begin();tileIter!=exchangeTiles.end();tileIter++)
	{
		cout << (*tileIter)->getLetter();
	}
	cout << endl;
}


/* Constructor for Placemove*/
PlaceMove::PlaceMove(size_t x, size_t y, bool horizontal, std::string tileString, Player * p): Move(p)
{
	_player = p;
	row = x;
	column = y;
	this->horizontal = horizontal;	
	this->_tileString = tileString;// initialize all the private members
}

/* Returns the vector of tiles associated with a PlaceMove*/
std::vector<Tile*> const & PlaceMove::tileVector () const
{
	return placeTiles;
}

/* Get the starting row of the place move*/
size_t PlaceMove::getStartRow () const
{
	return row;
}

/* Get the start column of the place move*/
size_t PlaceMove::getStartColumn () const
{
	return column;
}

/* Get the direction of the place move*/
bool PlaceMove::getDirection() const
{
	return horizontal;
}
/* Executes the PlaceMove and throw the five remaining exceptions*/
void PlaceMove::execute(Board & board, Bag & bag, Dictionary & dictionary)
{
	bool resolveBlanks = true;
	if (!_player->hasTiles(_tileString,resolveBlanks))
	{
		throw logic_error("There is no such tile in your hand");
	}
	placeTiles = _player->takeTiles(_tileString,resolveBlanks);
	if (board.Occupied(row,column)==true)
	{
		_player->addTiles(placeTiles);
		throw logic_error ("Error: the starting place has already had a tile!");// if the starting position has already had a tile, we throw a logic error
	}
	if (horizontal)// if the direction is horizontal
	{
		const size_t rowTrack = row; // we track the row
		size_t columnIndicator = column; // we indicate the current column
		if (board.isEmpty()==true&&board.hasInit()==false)
	    {
	    	bool startPos = false;
	    	size_t columnPos = column;
	    	if (row!=board.getStartRow())// if the first PlaceMove is not in the start square, we throw exception
	    	{
	    		_player->addTiles(placeTiles);
	    		throw logic_error ("First move is not on the start square!");
	    	}
			for (std::vector<Tile*>::iterator tileIter = placeTiles.begin();tileIter!=placeTiles.end();tileIter++)
			{
				if (columnPos==board.getStartCol())
				{
					startPos = true;
					break;
				}
				columnPos++;
			}
			if (startPos==false)// if the start column is not in the starting position, we throw exception
			{
				_player->addTiles(placeTiles);
				throw logic_error ("First move is not on the start square!");
			}
		}
		for (std::vector<Tile*>::iterator tileIter = placeTiles.begin();tileIter!=placeTiles.end();tileIter++)
		{
			while(board.getSquare(columnIndicator,rowTrack)->isOccupied()==true)// we increase column indicator by one in the current square is occupied
			{
				columnIndicator++;
				if (columnIndicator>board.getColumns()+1)
					break;
			}
			columnIndicator++;// we increase column by 1 as we place 1 more tile from hand
			if (columnIndicator>board.getColumns()+1)
				break;
		}
		if (columnIndicator>board.getColumns()+1)// if the tile go beyond the scope of the board, we throw an exception
		{
			_player->addTiles(placeTiles);
			throw logic_error ("Tiles out of the scope of board");
		}
	}

	/* This is the same as if except for changing the row and column*/
	else
	{
		size_t rowIndicator = row;
		const size_t columnTrack = column;
		if (board.isEmpty()==true&&board.hasInit()==false)
		{
		    bool startPos = false;
		    size_t rowPos = row;
		    if (column!=board.getStartCol())
		    {
		    	_player->addTiles(placeTiles);
		    	throw logic_error ("First move is not on the start square!");
		    }
			for (std::vector<Tile*>::iterator tileIter = placeTiles.begin();tileIter!=placeTiles.end();tileIter++)
			{
				if (rowPos==board.getStartRow())
				{
					startPos = true;
					break;
				}
				rowPos++;
			}
			if (startPos==false)
			{
				_player->addTiles(placeTiles);
				throw logic_error ("First move is not on the start square!");
			}
		}
		for (std::vector<Tile*>::iterator tileIter = placeTiles.begin();tileIter!=placeTiles.end();tileIter++)
		{
			while(board.getSquare(columnTrack,rowIndicator)->isOccupied()==true)
			{
				rowIndicator++;
				if (rowIndicator>board.getRows()+1)
					break;
			}
			rowIndicator++;
			if (rowIndicator>board.getRows()+1)
				break;
		}
		if (rowIndicator>board.getRows()+1)
		{
			_player->addTiles(placeTiles);
			throw logic_error ("Tiles out of the scope of board");
		}
	}

	std::vector<std::pair<std::string, unsigned int> > results= board.getPlaceMoveResults(*this);//we get the pairs returned from the board
	if (board.isAdjacent()==false&&board.isEmpty()==false)// if the new move has no tile adjacent to the previous tiles on the board, then we throw exception
	{
		_player->addTiles(placeTiles);
		throw logic_error ("Error: your now move has no tile adjacent to the previously placed tiles");
	}
	for (std::vector<std::pair<std::string, unsigned int> >::iterator pairIter = results.begin(); pairIter!=results.end();pairIter++)
	{
		if (dictionary.isLegalWord(pairIter->first)==false)// if there exists a word that is not legal, we throw an exception
		{
			_player->addTiles(placeTiles);
			throw logic_error ("Error: there is no such word in dictionary");
		}
	}
	for (std::vector<std::pair<std::string, unsigned int> >::iterator pairIter = results.begin(); pairIter!=results.end();pairIter++)
	{
		_player->addScore (pairIter->second);// if there is no logic error thrown, we update the player's score
	}
	board.executePlaceMove(*this);// we execute this move on the board
	if (_player->isEmpty()==true)// if the player's hand is empty after a place move, his score increases by another 50
	{
		if (bag.tilesRemaining()!=0)
		{
			_player->addScore(50);
			cout << "50 Extra points for using all tiles!" << endl;
		}
	}
	vector <Tile*> addedTiles;
	if (bag.tilesRemaining()>=placeTiles.size())// if the bag has more than enough tiles
	{
		addedTiles = bag.drawTiles(placeTiles.size());
		_player->addTiles(addedTiles);
	}
	else// if we have not enough tile to draw, then just empty the bag 
	{
		addedTiles = bag.drawTiles(bag.tilesRemaining());
		_player->addTiles(addedTiles);
	}
	cout << "Words Formed by your move and their points:" << endl;// output necessary information
	for (std::vector<std::pair<std::string, unsigned int> >::iterator pairIter = results.begin(); pairIter!=results.end();pairIter++)
	{
		cout << pairIter->first << " " << pairIter->second << endl;
	}
	cout << "New tiles picked up: ";
	for (std::vector<Tile*>::iterator tileIter = addedTiles.begin();tileIter!=addedTiles.end();tileIter++)
	{
		cout << (*tileIter)->getLetter();
	}
	cout << endl;
}

string PlaceMove::getMoveString() const
{
	return _tileString;
}

void PlaceMove::initiateTiles()
{
	placeTiles = _player->takeTiles(_tileString,true);
	_player->addTiles(placeTiles);
}