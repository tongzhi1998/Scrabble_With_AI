#include "CPUMaxTiles.h"
#include "Move.h"
#include "Tile.h"
#include "Board.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

/* Implementation of CPUL, which is pretty much the same as that of CPUS
So I just comment the most important and the different parts */

/* Constructor for CPUL */
CPUMaxTiles::CPUMaxTiles(std::string const & name, size_t maxTiles)
{
	p_name = name;
	score = 0;
	handSize = maxTiles;
	isPass = false;
}

/* Destructor for CPUL */
CPUMaxTiles::~CPUMaxTiles()
{

}

/* Returns a move of any type (Mostly placeMoves, rarely passMoves) */
Move* CPUMaxTiles::getMove (Board& gameBoard, const Bag& gameBag)
{
	legalMoves.clear();
	/* We constrain our search base a little bit by eliminating several start points */
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
	/* Turn the hand tiles to a string, which is easier to use later */
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
			verticalHelper(i,j,i,j,"","",handTiles,gameBoard,gameBag);
		}
	}
	if (legalMoves.size()==0){
		return new PassMove(this);
	}
	int maxLength = 0;
	for (vector<pair<PlaceMove,int>>::iterator pairIter = legalMoves.begin();pairIter!=legalMoves.end();pairIter++)
	{
		if (pairIter->second>maxLength)
			maxLength = pairIter->second;
	}
	for (vector<pair<PlaceMove,int>>::iterator pairIter = legalMoves.begin();pairIter!=legalMoves.end();pairIter++)
	{
		if (pairIter->second==maxLength)
		{
			PlaceMove* m = new PlaceMove(pairIter->first.getStartRow(),pairIter->first.getStartColumn(),pairIter->first.getDirection(),pairIter->first.getMoveString(),this);
			return m;
		}
	}
	return new PassMove(this);
}

/* Searching for valid horizontal moves given a starting point */
void CPUMaxTiles::horizontalHelper(int startRow, int startCol, int currRow, int currCol, string placed, string temp, string inHand,  Board& gameBoard, const Bag& gameBag)
{
	if (gameBoard.hasInit()==false&&gameBoard.isEmpty()==true)
	{
		if (startRow!=int(gameBoard.getStartRow()))
			return;
	}
	if (startRow<recordUpRow-1||startRow>recordDownRow+1)
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
		for (unsigned int i=0;i<temp.length();i++)
		{
			newTemp+=temp[i];
		}	
		while (gameBoard.Occupied(currRow,currCol)==true)
		{
			newTemp+=gameBoard.getSquare(currCol,currRow)->getLetter();
			currCol++;
			if (currCol>int(gameBoard.getColumns()))
				break;
		}
		currCol--;
		string newPlaced = placed;
		string newInHand = inHand;
		if (gameTrie.prefix(newTemp)==NULL)
			return;
		else if (gameTrie.prefix(newTemp)->inset==true&&newPlaced.length()!=0)
		{
			PlaceMove m(startRow,startCol,true,newPlaced,this);
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
				PlaceMove m(startRow,startCol,true,newPlaced,this);
				unsigned int length = newPlaced.length();
				for (unsigned int i=0;i<newPlaced.length();i++)
				{
					if (newPlaced[i]=='?')
						length--;
				}
				legalMoves.push_back(make_pair(m,length));
			}
		}
		if (currCol+1>int(gameBoard.getColumns()))
			return;
		else
			horizontalHelper(startRow,startCol,currRow,currCol+1,newPlaced,newTemp,newInHand,gameBoard,gameBag);
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
					newInHand+=inHand[j];
				}
				if (gameTrie.prefix(newTemp)==NULL){
					continue;
				}
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
							unsigned int length = newPlaced.length();
							for (unsigned int i=0;i<newPlaced.length();i++)
							{
								if (newPlaced[i]=='?')
									length--;
							}
							legalMoves.push_back(make_pair(m,length));
						}
					}
					else if (gameBoard.isEmpty()==true&&gameBoard.hasInit()==false)
					{
						if (m.getStartRow()==gameBoard.getStartRow())
						{
							if ((m.getStartColumn()==gameBoard.getStartCol())||(m.getStartColumn()<gameBoard.getStartCol()&&m.getStartColumn()+newTemp.length()>gameBoard.getStartCol()))
							{
								PlaceMove m(startRow,startCol,true,newPlaced,this);
								legalMoves.push_back(make_pair(m,newPlaced.length()));
							}
						}
					}
					else if (gameBoard.isEmpty()==true&&gameBoard.hasInit()==true)
					{
						unsigned int length = newPlaced.length();
						for (unsigned int i=0;i<newPlaced.length();i++)
						{
							if (newPlaced[i]=='?')
								length--;
						}
						legalMoves.push_back(make_pair(m,length));
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
								PlaceMove m(startRow,startCol,true,newPlaced,this);
								unsigned int length = newPlaced.length();
								for (unsigned int i=0;i<newPlaced.length();i++)
								{
									if (newPlaced[i]=='?')
										length--;
								}
								legalMoves.push_back(make_pair(m,length));
							}
						}
						else if (gameBoard.isEmpty()==true)
						{
							if (m.getStartRow()==gameBoard.getStartRow())
							{
								if ((m.getStartColumn()==gameBoard.getStartCol())||(m.getStartColumn()<gameBoard.getStartCol()&&m.getStartColumn()+newTemp.length()>gameBoard.getStartCol()))
								{
									PlaceMove m(startRow,startCol,true,newPlaced,this);
									unsigned int length = newPlaced.length();
									for (unsigned int i=0;i<newPlaced.length();i++)
									{
										if (newPlaced[i]=='?')
											length--;
									}
									legalMoves.push_back(make_pair(m,length));
								}
							}
						}
						else if (gameBoard.isEmpty()==true&&gameBoard.hasInit()==true)
						{
							unsigned int length = newPlaced.length();
							for (unsigned int i=0;i<newPlaced.length();i++)
							{
								if (newPlaced[i]=='?')
									length--;
							}
							legalMoves.push_back(make_pair(m,length));
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

/* Searching for valid vertical moves given a starting point */
void CPUMaxTiles::verticalHelper (int startRow, int startCol, int currRow, int currCol, string placed, string temp, string inHand, Board& gameBoard, const Bag& gameBag)
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
		while (gameBoard.Occupied(currRow,currCol)==true)
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
		else if (gameTrie.prefix(newTemp)->inset==true&&newPlaced.length()!=0)
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
				unsigned int length = newPlaced.length();
				for (unsigned int i=0;i<newPlaced.length();i++)
				{
					if (newPlaced[i]=='?')
						length--;
				}
				legalMoves.push_back(make_pair(m,length));
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
							unsigned int length = newPlaced.length();
							for (unsigned int i=0;i<newPlaced.length();i++)
							{
								if (newPlaced[i]=='?')
									length--;
							}
							legalMoves.push_back(make_pair(m,length));
						}
					}
					else if (gameBoard.isEmpty()==true)
					{
						if (m.getStartColumn()==gameBoard.getStartCol())
						{
							if ((m.getStartRow()==gameBoard.getStartRow())||(m.getStartRow()<gameBoard.getStartRow()&&m.getStartRow()+newTemp.length()>gameBoard.getStartRow()))
							{
								PlaceMove m(startRow,startCol,false,newPlaced,this);
								unsigned int length = newPlaced.length();
								for (unsigned int i=0;i<newPlaced.length();i++)
								{
									if (newPlaced[i]=='?')
										length--;
								}
								legalMoves.push_back(make_pair(m,length));
							}
						}
					}
					else if (gameBoard.isEmpty()==true&&gameBoard.hasInit()==true)
					{
						legalMoves.push_back(make_pair(m,newPlaced.length()));
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
								unsigned int length = newPlaced.length();
								for (unsigned int i=0;i<newPlaced.length();i++)
								{
									if (newPlaced[i]=='?')
										length--;
								}
								legalMoves.push_back(make_pair(m,length));
							}
						}
						else if (gameBoard.isEmpty()==true)
						{
							if (m.getStartColumn()==gameBoard.getStartCol())
							{
								if ((m.getStartRow()==gameBoard.getStartRow())||(m.getStartRow()<gameBoard.getStartRow()&&m.getStartRow()+newTemp.length()>gameBoard.getStartRow()))
								{
									PlaceMove m(startRow,startCol,false,newPlaced,this);
									unsigned int length = newPlaced.length();
									for (unsigned int i=0;i<newPlaced.length();i++)
									{
										if (newPlaced[i]=='?')
											length--;
									}
									legalMoves.push_back(make_pair(m,length));
								}
							}
						}
						else if (gameBoard.isEmpty()==true&&gameBoard.hasInit()==true)
						{
							legalMoves.push_back(make_pair(m,newPlaced.length()));
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

/* Return the name for CPUL */
string CPUMaxTiles::getName() const
{
	return "CPUL";
}

void CPUMaxTiles::printAllMoves() 
{
	for (vector<pair<PlaceMove,int>>::iterator pairIter = legalMoves.begin();pairIter!=legalMoves.end();pairIter++)
	{
		cerr << "place " << boolalpha << pairIter->first.getDirection() << " "<<pairIter->first.getStartRow() << " " <<
		pairIter->first.getStartColumn() << " " <<pairIter->first.getMoveString() << endl;
	}
}
