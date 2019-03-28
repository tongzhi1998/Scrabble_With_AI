#include "CPUMaxScore.h"
#include "Move.h"
#include "Board.h"
#include "Bag.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

/* Constructor for CPUS */
CPUMaxScore::CPUMaxScore(std::string const & name, size_t maxTiles)
{
	p_name = name;
	score = 0;
	handSize = maxTiles;
	isPass = false;
}

/* Destructor for CPUS */
CPUMaxScore::~CPUMaxScore()
{

}

/* Returns a move of any type (Mostly placeMoves) */
Move* CPUMaxScore::getMove (Board& gameBoard, const Bag& gameBag)
{
	legalMoves.clear();// clear all legalMoves formed by previous step 
	/* This part is used to check for the possible range to make the first move */
	recordUpRow=1;
	recordDownRow=int(gameBoard.getRows());
	recordLeftColumn=1;
	recordRightColumn=int(gameBoard.getColumns());
	for (int i=1;i<=int(gameBoard.getRows());i++)
	{
		bool found = false;
		for ( int j=1;j<=int(gameBoard.getColumns());j++)
		{
			if (gameBoard.Occupied(i,j)==true){
				recordUpRow = i;
				found = true;
				break;
			}
		}
		if (found==true)
			break;
	}
	for (int i=int(gameBoard.getRows());i>=1;i--)
	{
		bool found = false;
		for (int j=int(gameBoard.getRows());j>=1;j--)
		{
			if (gameBoard.Occupied(i,j)==true){
				recordDownRow = i;
				found = true;
				break;
			}
		}
		if (found ==true)
			break;
	}
	for (int j=1;j<=int(gameBoard.getRows());j++)
	{
		bool found = false;
		for (int i=1;i<=int(gameBoard.getColumns());i++)
		{
			if (gameBoard.Occupied(i,j)==true){
				recordLeftColumn = j;
				found = true;
				break;
			}
		}
		if (found == true)
			break;
	}
	for (int j=int(gameBoard.getRows());j>=1;j--)
	{
		bool found = false;
		for (int i=int(gameBoard.getRows());i>=1;i--)
		{
			if (gameBoard.Occupied(i,j)==true){
				recordRightColumn = j;
				found = true;
				break;
			}
		}
		if (found == true)
			break;
	}
	/* Create a handTile string which is easier to pass in to the helper functions */
	string handTiles;
	for (set<Tile*>::iterator tileIter = hand.begin();tileIter!=hand.end();tileIter++)
	{
		handTiles+=(*tileIter)->getLetter();
	}
	for (unsigned int i=1;i<=gameBoard.getRows();i++)
	{
		for (unsigned int j=1;j<=gameBoard.getColumns();j++)
		{
			horizontalHelper(i,j,i,j,"","",handTiles,gameBoard,gameBag);
			verticalHelper(i,j,i,j,"","",handTiles,gameBoard,gameBag);// calling vertical and horizontal helper on each square
		}
	}
	if (legalMoves.size()==0)
	{
		return new PassMove(this);// if we cannot get any legal moves, we return a passmove
	}
	int maxScore = 0;
	/* Otherwise we get the move with the highest score and return it */
	for (vector<pair<PlaceMove,int>>::iterator pairIter = legalMoves.begin();pairIter!=legalMoves.end();pairIter++)
	{
		if (pairIter->second>maxScore)
			maxScore = pairIter->second;
	}
	for (vector<pair<PlaceMove,int>>::iterator pairIter = legalMoves.begin();pairIter!=legalMoves.end();pairIter++)
	{
		if (pairIter->second==maxScore)
		{
			PlaceMove* m= new PlaceMove(pairIter->first.getStartRow(),pairIter->first.getStartColumn(),pairIter->first.getDirection(),pairIter->first.getMoveString(),this);
			//std::cerr << m->getStartRow() << " " << m->getStartColumn() << " " << m->getDirection() << " " << m->getMoveString()  << std::endl;
			return m;
		}
	}
	return new PassMove(this);// this line of code will never trigger yet to avoid compiling with error I have to add it
}

/* Searching for valid horizontal moves given a starting point */
void CPUMaxScore::horizontalHelper(int startRow, int startCol, int currRow, int currCol, string placed, string temp, string inHand,  Board& gameBoard, const Bag& gameBag)
{
	if (gameBoard.hasInit()==false&&gameBoard.isEmpty()==true) // if there is nothing in the board and no initial file
	{
		if (startRow!=int(gameBoard.getStartRow()))
			return;
	}
	if (startRow<recordUpRow-1||startRow>recordDownRow+1) // if the search scope is out of possible boundary, we just return
		return;
	if (gameBoard.Occupied(startRow,startCol)==true)// if the start position has a tile, we skip that position
		return;
	if (inHand.length()==0&&gameBoard.Occupied(currRow,currCol)==false)// if the CPU runs out of tiles and there is no more to go
		return;
	if (gameBoard.Occupied(currRow,currCol)==true)// if the current square is occupied, we move forward to an unoccupied square and record everything accordingly
	{
		string newTemp="";
		/* searching forward if this is the first tile to be placed */
		if (temp.length()==0)
		{
			bool entered = false;
			if (currCol>1)
			{
				int record = currCol-1;
				while(gameBoard.Occupied(currRow,record)==true)
				{
					record--;
					entered = true;
					if (record<=0)
						break;
				}
				if (entered == true)
				{
					record++;
					for (int i=record;i<currCol;i++)
					{
						newTemp+=gameBoard.getSquare(i,currRow)->getLetter();
					}
				}
			}
		}
		/* Create the temp by also including tiles in the board */
		for (unsigned int k=0;k<temp.length();k++)
		{
			newTemp+=temp[k];
		}
		while (gameBoard.Occupied(currRow,currCol)==true)
		{
			newTemp+=gameBoard.getSquare(currCol,currRow)->getLetter();
			currCol++;
			if (currCol>int(gameBoard.getColumns()))
				break;
		}
		currCol--;
		string newPlaced = placed; // create newplaced accordingly
		string newInHand = inHand; // create newInhand accordingly
		if (gameTrie.prefix(newTemp)==NULL)// if there is no such prefix, we just return
			return;
		else if (gameTrie.prefix(newTemp)->inset==true&&newPlaced.length()!=0) // if the move creates a legal word in its mean direction
		{
			PlaceMove m(startRow,startCol,true,newPlaced,this);
			m.initiateTiles();// we create a PlaceMove and finish all initialization
			vector<pair<string,unsigned int>> results = gameBoard.getPlaceMoveResults(m);// call the getPlaceMoveResult function to get all words and check against the trie
			bool valid = true;
			for (vector<pair<string,unsigned int>>::iterator pairIter = results.begin();pairIter!=results.end();pairIter++)
			{
				if (gameTrie.prefix(pairIter->first)==NULL||gameTrie.prefix(pairIter->first)->inset == false)
				{
					valid = false;
					break;
				}
			}
			/* If all the words are valid, we generate a new PlaceMove and push it to our vector */
			if (valid == true)
			{
				PlaceMove m(startRow,startCol,true,newPlaced,this);
				m.initiateTiles();
				vector<pair<string,unsigned int>> finalResult = gameBoard.getPlaceMoveResults(m);
				unsigned int finalScore = 0;
				for (vector<pair<string,unsigned int>>::iterator pairIter = finalResult.begin();pairIter!=finalResult.end();pairIter++)
				{
					finalScore+=pairIter->second;
				}
				legalMoves.push_back(make_pair(m,finalScore));
			}
		}
		if (currCol+1>int(gameBoard.getColumns())) // if we are out of bound, just return
			return;
		else // recursively doing the backtracking on next square
			horizontalHelper(startRow,startCol,currRow,currCol+1,newPlaced,newTemp,newInHand,gameBoard,gameBag);
	}
	else // if we need to place a hand tile onto the board
	{
		for (unsigned int i=0;i<inHand.length();i++)
		{
			if (isalpha(inHand[i]))
			{
				string newTemp;
				if (temp.length()==0)
				{
					bool entered = false;
					if (currCol>1)
					{
						int record = currCol-1;
						while(gameBoard.Occupied(currRow,record)==true)
						{
							record--;
							entered = true;
							if (record<=0)
								break;
						}
						if (entered == true)
						{
							record++;
							for (int i=record;i<currCol;i++)
							{
								newTemp+=gameBoard.getSquare(i,currRow)->getLetter();
							}
						}
					}
				}
				string newPlaced = placed+inHand[i];
				for (unsigned int k=0;k<temp.length();k++)
				{
					newTemp+=temp[k];
				}
				newTemp+=inHand[i];
				string newInHand; 
				for (unsigned int j=0;j<i;j++)
				{
					newInHand+=inHand[j];
				}
				for (unsigned int j=i+1;j<inHand.length();j++)
				{
					newInHand+=inHand[j]; // we set all variables accordingly (same as the previous case)
				}
				if (gameTrie.prefix(newTemp)==NULL)// if the new formed prefix is not legal, just continue to the next tile in hand
					continue;
				/* All the rest stuff is the same as the previous case*/
				else if (gameTrie.prefix(newTemp)->inset==true)
				{
					PlaceMove m(startRow,startCol,true,newPlaced,this);
					m.initiateTiles();
					vector<pair<string,unsigned int>> results = gameBoard.getPlaceMoveResults(m);
					if (gameBoard.isAdjacent()==true)
					{
						bool valid = true;
						for (vector<pair<string,unsigned int>>::iterator pairIter = results.begin();pairIter!=results.end();pairIter++)
						{
							if (gameTrie.prefix(pairIter->first)==NULL||gameTrie.prefix(pairIter->first)->inset == false)
							{
								valid = false;
								break;
							}
						}
						if (valid == true)
						{
							PlaceMove m(startRow,startCol,true,newPlaced,this);
							m.initiateTiles();
							vector<pair<string,unsigned int>> finalResult = gameBoard.getPlaceMoveResults(m);
							unsigned int finalScore = 0;
							for (vector<pair<string,unsigned int>>::iterator pairIter = finalResult.begin();pairIter!=finalResult.end();pairIter++)
							{
								finalScore+=pairIter->second;
							}
							if (newInHand.length()==0&&gameBag.tilesRemaining()!=0)
								finalScore+=50;
							legalMoves.push_back(make_pair(m,finalScore));
						}
					}
					/* If the board is empty and the gameBoard doesn't has initial file */
					else if (gameBoard.isEmpty()==true&&gameBoard.hasInit()==false)
					{
						if (m.getStartRow()==gameBoard.getStartRow())
						{
							if ((m.getStartColumn()==gameBoard.getStartCol())||(m.getStartColumn()<gameBoard.getStartCol()&&m.getStartColumn()+newTemp.length()>gameBoard.getStartCol()))
							{
								PlaceMove m(startRow,startCol,true,newPlaced,this);
								m.initiateTiles();
								vector<pair<string,unsigned int>> finalResult = gameBoard.getPlaceMoveResults(m);
								unsigned int finalScore = 0;
								for (vector<pair<string,unsigned int>>::iterator pairIter = finalResult.begin();pairIter!=finalResult.end();pairIter++)
								{
									finalScore+=pairIter->second;
								}
								if (newInHand.length()==0&&gameBag.tilesRemaining()!=0)
									finalScore+=50;
								legalMoves.push_back(make_pair(m,finalScore));
							}
						}
					}
					/* If the board has initial file */
					else if (gameBoard.isEmpty()==true&&gameBoard.hasInit()==true)
					{
						unsigned int finalScore = 0;
						for (vector<pair<string,unsigned int>>::iterator pairIter = results.begin();pairIter!=results.end();pairIter++)
						{
							finalScore+=pairIter->second;
						}
						if (newInHand.length()==0&&gameBag.tilesRemaining()!=0)
							finalScore+=50;
						legalMoves.push_back(make_pair(m,finalScore));
					}
				}
				if (currCol+1>int(gameBoard.getColumns()))
					continue;
				else
					horizontalHelper(startRow,startCol,currRow,currCol+1,newPlaced,newTemp,newInHand,gameBoard,gameBag);
			}
			/* When one of the tiles is "?"	*/
			else
			{
				for (char x = 'a';x<='z';x++)
				{
					string newTemp;
					if (temp.length()==0)
					{
						bool entered = false;
						if (currCol>1)
						{
							int record = currCol-1;
							while(gameBoard.Occupied(currRow,record)==true)
							{
								record--;
								entered = true;
								if (record<=0)
									break;
							}
							if (entered == true)
							{
								record++;
								for (int i=record;i<currCol;i++)
								{
									newTemp+=gameBoard.getSquare(i,currRow)->getLetter();
								}
							}
						}
					}
					string newPlaced = placed+'?'+x;
					for (unsigned int k=0;k<temp.length();k++)
					{
						newTemp+=temp[k];
					}
					newTemp+=x;
					string newInHand;
					for (unsigned int j=0;j<i;j++)
					{
						newInHand+=inHand[j];
					}
					for (unsigned int j=i+1;j<inHand.length();j++)
					{
						newInHand+=inHand[j];
					}
					if (gameTrie.prefix(newTemp)==NULL)
						continue;
					else if (gameTrie.prefix(newTemp)->inset==true)
					{
						PlaceMove m(startRow,startCol,true,newPlaced,this);
						m.initiateTiles();
						vector<pair<string,unsigned int>> results = gameBoard.getPlaceMoveResults(m);
						if (gameBoard.isAdjacent()==true)
						{
							bool valid = true;
							for (vector<pair<string,unsigned int>>::iterator pairIter = results.begin();pairIter!=results.end();pairIter++)
							{
								if (gameTrie.prefix(pairIter->first)==NULL||gameTrie.prefix(pairIter->first)->inset == false)
								{
									valid = false;
									break;
								}
							}
							if (valid == true)
							{
								PlaceMove m (startRow,startCol,true,newPlaced,this);
								m.initiateTiles();
								vector<pair<string,unsigned int>> finalResult = gameBoard.getPlaceMoveResults(m);
								unsigned int finalScore = 0;
								for (vector<pair<string,unsigned int>>::iterator pairIter = finalResult.begin();pairIter!=finalResult.end();pairIter++)
								{
									finalScore+=pairIter->second;
								}
								if (newInHand.length()==0&&gameBag.tilesRemaining()!=0)
									finalScore+=50;
								legalMoves.push_back(make_pair(m,finalScore));
							}
						}
						else if (gameBoard.isEmpty()==true&&gameBoard.hasInit()==false)
						{
							if (m.getStartRow()==gameBoard.getStartRow())
							{
								if ((m.getStartColumn()==gameBoard.getStartCol())||(m.getStartColumn()<gameBoard.getStartCol()&&m.getStartColumn()+newTemp.length()>gameBoard.getStartCol()))
								{
									PlaceMove m(startRow,startCol,true,newPlaced,this);
									m.initiateTiles();
									vector<pair<string,unsigned int>> finalResult = gameBoard.getPlaceMoveResults(m);
									unsigned int finalScore = 0;
									for (vector<pair<string,unsigned int>>::iterator pairIter = finalResult.begin();pairIter!=finalResult.end();pairIter++)
									{
										finalScore+=pairIter->second;
									}
									if (newInHand.length()==0&&gameBag.tilesRemaining()!=0)
										finalScore+=50;
									legalMoves.push_back(make_pair(m,finalScore));
								}
							}
						}
						else if (gameBoard.isEmpty()==true&&gameBoard.hasInit()==true)
						{
							unsigned int finalScore = 0;
							for (vector<pair<string,unsigned int>>::iterator pairIter = results.begin();pairIter!=results.end();pairIter++)
							{
								finalScore+=pairIter->second;
							}
							if (newInHand.length()==0&&gameBag.tilesRemaining()!=0)
								finalScore+=50;
							legalMoves.push_back(make_pair(m,finalScore));
						}
					}
					if (currCol+1>int(gameBoard.getColumns()))
						continue;
					else{
						horizontalHelper(startRow,startCol,currRow,currCol+1,newPlaced,newTemp,newInHand,gameBoard,gameBag);
					}
				}
			}
		}
	}
}

/* Searching for valid vertical moves given a starting point, the same as horizontal helper */
void CPUMaxScore::verticalHelper (int startRow, int startCol, int currRow, int currCol, string placed, string temp, string inHand, Board& gameBoard, const Bag& gameBag)
{
	if (gameBoard.hasInit()==false&&gameBoard.isEmpty()==true)
	{
		if (startCol!=int(gameBoard.getStartCol()))
			return;
	}
	if (startCol<recordLeftColumn-1||startCol>recordRightColumn+1)
		return;
	if (gameBoard.Occupied(startRow,startCol)==true)
		return;
	if (inHand.length()==0&&gameBoard.Occupied(currRow,currCol)==false)
		return;
	if (gameBoard.Occupied(currRow,currCol)==true)
	{
		string newTemp;
		if (temp.length()==0)
		{
			bool entered = false;
			if (currRow>1)
			{
				int record = currRow-1;
				while(gameBoard.Occupied(record,currCol)==true)
				{
					record--;
					entered = true;
					if (record<=0)
						break;
				}
				if (entered == true)
				{
					record++;
					for (int i=record;i<currRow;i++)
					{
						newTemp+=gameBoard.getSquare(currCol,i)->getLetter();
					}
				}
			}
		}
		for (unsigned int i=0;i<temp.length();i++)
		{
			newTemp+=temp[i];
		}
		while(gameBoard.Occupied(currRow,currCol)==true)
		{
			newTemp+=gameBoard.getSquare(currCol,currRow)->getLetter();
			currRow++;
			if (currRow>int(gameBoard.getRows()))
				break;
		}
		currRow--;
		string newPlaced = placed;
		string newInHand = inHand;
		if (gameTrie.prefix(newTemp)==NULL)
			return;
		else if (gameTrie.prefix(newTemp)->inset==true)
		{
			PlaceMove m(startRow,startCol,false,newPlaced,this);
			m.initiateTiles();
			vector<pair<string,unsigned int>> results = gameBoard.getPlaceMoveResults(m);
			bool valid = true;
			for (vector<pair<string,unsigned int>>::iterator pairIter = results.begin();pairIter!=results.end();pairIter++)
			{
				if (gameTrie.prefix(pairIter->first)==NULL||gameTrie.prefix(pairIter->first)->inset == false)
				{
					valid = false;
					break;
				}
			}
			if (valid == true)
			{
				PlaceMove m(startRow,startCol,false,newPlaced,this);
				m.initiateTiles();
				vector<pair<string,unsigned int>> finalResult = gameBoard.getPlaceMoveResults(m);
				unsigned int finalScore = 0;
				for (vector<pair<string,unsigned int>>::iterator pairIter = finalResult.begin();pairIter!=finalResult.end();pairIter++)
				{
					finalScore+=pairIter->second;
				}
				if (newInHand.length()==0&&gameBag.tilesRemaining()!=0)
					finalScore+=50;
				legalMoves.push_back(make_pair(m,finalScore));
			}
		}
		if (currRow+1>int(gameBoard.getRows()))
			return;
		else
			verticalHelper(startRow,startCol,currRow+1,currCol,newPlaced,newTemp,newInHand,gameBoard,gameBag);
	}
	else
	{
		for (unsigned int i=0;i<inHand.length();i++)
		{
			if (isalpha(inHand[i]))
			{
				string newTemp;
				if (temp.length()==0)
				{
					bool entered = false;
					if (currRow>1)
					{
						int record = currRow-1;
						while(gameBoard.Occupied(record,currCol)==true)
						{
							record--;
							entered = true;
							if (record<=0)
								break;
						}
						if (entered == true)
						{
							record++;
							for (int i=record;i<currRow;i++)
							{
								newTemp+=gameBoard.getSquare(currCol,i)->getLetter();
							}
						}
					}
				}
				string newPlaced = placed+inHand[i];
				for (unsigned int k=0;k<temp.length();k++)
				{
					newTemp+=temp[k];
				}
				newTemp+=inHand[i];
				string newInHand;
				for (unsigned int j=0;j<i;j++)
				{
					newInHand+=inHand[j];
				}
				for (unsigned int j=i+1;j<inHand.length();j++)
				{
					newInHand+=inHand[j];
				}
				if (gameTrie.prefix(newTemp)==NULL){
					continue;
				}
				else if (gameTrie.prefix(newTemp)->inset==true)
				{
					PlaceMove m(startRow,startCol,false,newPlaced,this);
					m.initiateTiles();
					vector<pair<string,unsigned int>> results = gameBoard.getPlaceMoveResults(m);
					if (gameBoard.isAdjacent()==true)
					{
						bool valid = true;
						for (vector<pair<string,unsigned int>>::iterator pairIter = results.begin();pairIter!=results.end();pairIter++)
						{
							if (gameTrie.prefix(pairIter->first)==NULL||gameTrie.prefix(pairIter->first)->inset == false)
							{
								valid = false;
								break;
							}
						}
						if (valid == true)
						{
							PlaceMove m(startRow,startCol,false,newPlaced,this);
							m.initiateTiles();
							vector<pair<string,unsigned int>> finalResult = gameBoard.getPlaceMoveResults(m);
							unsigned int finalScore = 0;
							for (vector<pair<string,unsigned int>>::iterator pairIter = finalResult.begin();pairIter!=finalResult.end();pairIter++)
							{
								finalScore+=pairIter->second;
							}
							if (newInHand.length()==0&&gameBag.tilesRemaining()!=0)
								finalScore+=50;
							legalMoves.push_back(make_pair(m,finalScore));
						}
					}
					else if (gameBoard.isEmpty()==true&&gameBoard.hasInit()==false)
					{
						if (m.getStartColumn()==gameBoard.getStartCol())
						{
							if ((m.getStartRow()==gameBoard.getStartRow())||(m.getStartRow()<gameBoard.getStartRow()&&m.getStartRow()+newTemp.length()>gameBoard.getStartRow()))
							{
								PlaceMove m(startRow,startCol,false,newPlaced,this);
								m.initiateTiles();
								vector<pair<string,unsigned int>> finalResult = gameBoard.getPlaceMoveResults(m);
								unsigned int finalScore = 0;
								for (vector<pair<string,unsigned int>>::iterator pairIter = finalResult.begin();pairIter!=finalResult.end();pairIter++)
								{
									finalScore+=pairIter->second;
								}
								if (newInHand.length()==0&&gameBag.tilesRemaining()!=0)
									finalScore+=50;
								legalMoves.push_back(make_pair(m,finalScore));
							}
						}
					}
					else if (gameBoard.isEmpty()==true&&gameBoard.hasInit()==true)
					{
						unsigned int finalScore = 0;
						for (vector<pair<string,unsigned int>>::iterator pairIter = results.begin();pairIter!=results.end();pairIter++)
						{
							finalScore+=pairIter->second;
						}
						if (newInHand.length()==0&&gameBag.tilesRemaining()!=0)
							finalScore+=50;
						legalMoves.push_back(make_pair(m,finalScore));
					}

				}
				if (currRow+1>int(gameBoard.getRows()))
					continue;
				else
					verticalHelper(startRow,startCol,currRow+1,currCol,newPlaced,newTemp,newInHand,gameBoard,gameBag);
			}
			/* When one of the tiles is "?"	*/
			else
			{
				for (char x = 'a';x<='z';x++)
				{
					string newTemp;
					if (temp.length()==0)
					{
						bool entered = false;
						if (currRow>1)
						{
							int record = currRow-1;
							while(gameBoard.Occupied(record,currCol)==true)
							{
								record--;
								entered = true;
								if (record<=0)
									break;
							}
							if (entered == true)
							{
								record++;
								for (int i=record;i<currRow;i++)
								{
									newTemp+=gameBoard.getSquare(currCol,i)->getLetter();
								}
							}
						}
					}
					string newPlaced = placed+'?'+x;
					for (unsigned int k=0;k<temp.length();k++)
					{
						newTemp+=temp[k];
					}
					newTemp+=x;
					string newInHand;
					for (unsigned int j=0;j<i;j++)
					{
						newInHand+=inHand[j];
					}
					for (unsigned int j=i+1;j<inHand.length();j++)
					{
						newInHand+=inHand[j];
					}
					if (gameTrie.prefix(newTemp)==NULL)
						continue;
					else if (gameTrie.prefix(newTemp)->inset==true)
					{
						PlaceMove m(startRow,startCol,false,newPlaced,this);
						m.initiateTiles();
						vector<pair<string,unsigned int>> results = gameBoard.getPlaceMoveResults(m);
						if (gameBoard.isAdjacent()==true)
						{
							bool valid = true;
							for (vector<pair<string,unsigned int>>::iterator pairIter = results.begin();pairIter!=results.end();pairIter++)
							{
								if (gameTrie.prefix(pairIter->first)==NULL||gameTrie.prefix(pairIter->first)->inset == false)
								{
									valid = false;
									break;
								}
							}
							if (valid == true)
							{
								PlaceMove m(startRow,startCol,false,newPlaced,this);
								m.initiateTiles();
								vector<pair<string,unsigned int>> finalResult = gameBoard.getPlaceMoveResults(m);
								unsigned int finalScore = 0;
								for (vector<pair<string,unsigned int>>::iterator pairIter = finalResult.begin();pairIter!=finalResult.end();pairIter++)
								{
									finalScore+=pairIter->second;
								}
								if (newInHand.length()==0&&gameBag.tilesRemaining()!=0)
									finalScore+=50;
								legalMoves.push_back(make_pair(m,finalScore));
							}
						}
						else if (gameBoard.isEmpty()==true&&gameBoard.hasInit()==false)
						{
							if (m.getStartColumn()==gameBoard.getStartCol())
							{
								if ((m.getStartRow()==gameBoard.getStartRow())||(m.getStartRow()<gameBoard.getStartRow()&&m.getStartRow()+newTemp.length()>gameBoard.getStartRow()))
								{
									PlaceMove m(startRow,startCol,false,newPlaced,this);
									m.initiateTiles();
									vector<pair<string,unsigned int>> finalResult = gameBoard.getPlaceMoveResults(m);
									unsigned int finalScore = 0;
									for (vector<pair<string,unsigned int>>::iterator pairIter = finalResult.begin();pairIter!=finalResult.end();pairIter++)
									{
										finalScore+=pairIter->second;
									}
									if (newInHand.length()==0&&gameBag.tilesRemaining()!=0)
										finalScore+=50;
									legalMoves.push_back(make_pair(m,finalScore));
								}
							}
						}
						else if (gameBoard.isEmpty()==true&&gameBoard.hasInit()==true)
						{
							unsigned int finalScore = 0;
							for (vector<pair<string,unsigned int>>::iterator pairIter = results.begin();pairIter!=results.end();pairIter++)
							{
								finalScore+=pairIter->second;
							}
							if (newInHand.length()==0&&gameBag.tilesRemaining()!=0)
								finalScore+=50;
							legalMoves.push_back(make_pair(m,finalScore));
						}
					}
					if (currRow+1>int(gameBoard.getRows()))
						continue;
					else
						verticalHelper(startRow,startCol,currRow+1,currCol,newPlaced,newTemp,newInHand,gameBoard,gameBag);
				}
			}
		}
	}
}

/* Return the name of CPUS */
string CPUMaxScore::getName() const
{
	return "CPUS";
}

/* Helper function for debugging the CPU */
void CPUMaxScore::printAllMoves() 
{
	for (vector<pair<PlaceMove,int>>::iterator pairIter = legalMoves.begin();pairIter!=legalMoves.end();pairIter++)
	{
		cerr << "place " << boolalpha << pairIter->first.getDirection() << " "<<pairIter->first.getStartRow() << " " <<
		pairIter->first.getStartColumn() << " " <<pairIter->first.getMoveString() << endl;
	}
}