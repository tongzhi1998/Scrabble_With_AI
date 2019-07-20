#include "../lib/Board.h"
#include "../lib/Tile.h"
#include "../lib/Square.h"
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;

/* constructor that initializes the board state with an empty board
    and the size and bonuses and start square based on the given file. */
Board::Board (std::string board_file_namey)
{
	init = false;
	adjacent = false;// initialize the adjacency to false
	ifstream ifile (board_file_namey);// read the file using ifstream
	unsigned int totalRow,totalColumn,startPR,startPC;// record the total number of rows, columns and starting position
	ifile >> totalRow>>totalColumn>>startPC>>startPR;
	row = totalRow;
	column = totalColumn;
	startPosRow = startPR;
	startPosCol = startPC;
	squareArray = new Square** [row];// dynamically allocate the Squarearray
	for (unsigned int i=0;i<row;i++)
	{
		squareArray[i] = new Square*[column];// dynamically allocate a 2D square array of pointers
	}
	for (unsigned int i=0; i<row;i++)// this whole for loop is aimed to read the board's format and dynamically allocate the 2D squareArray
	{
		for (unsigned int j=0; j<column; j++)
		{
			char indicator;
			ifile>>indicator;
			if (indicator=='.'&& !(i == startPosRow-1 && j == startPosCol -1))
			{
				squareArray[i][j] = new Square (1,1,false);
			}
			else if (indicator=='.'&& (i == startPosRow-1 && j == startPosCol -1))
			{
				squareArray[i][j] = new Square (1,1,true);
			}
			else if (indicator=='2'&& !(i == startPosRow-1 && j == startPosCol -1))
			{
				squareArray[i][j] = new Square (2,1,false);
			}
			else if (indicator=='2'&& (i == startPosRow-1 && j == startPosCol -1))
			{
				squareArray[i][j] = new Square (2,1,true);
			}
			else if (indicator=='3'&& !(i == startPosRow-1 && j == startPosCol -1))
			{
				squareArray[i][j] = new Square (3,1,false);
			}
			else if (indicator=='3'&& (i == startPosRow-1 && j == startPosCol -1))
			{
				squareArray[i][j] = new Square (3,1,true);
			}
			else if (indicator=='d'&& !(i == startPosRow-1 && j == startPosCol -1))
			{
				squareArray[i][j] = new Square (1,2,false);
			}
			else if (indicator=='d'&& (i == startPosRow-1 && j == startPosCol -1))
			{
				squareArray[i][j] = new Square (1,2,true);
			}
			else if (indicator=='t'&& !(i == startPosRow-1 && j == startPosCol -1))
			{
				squareArray[i][j] = new Square (1,3,false);
			}
			else 
			{
				squareArray[i][j] = new Square (1,3,true);
			}
		}
	}
}

/* Constructor for board given the initial state */
Board::Board(std::string board_file_namey,std::string init_state_namey)
{
	init = true;
	adjacent = false;// initialize the adjacency to false
	ifstream ifile (board_file_namey);// read the file using ifstream
	unsigned int totalRow,totalColumn,startPR,startPC;// record the total number of rows, columns and starting position
	ifile >> totalRow>>totalColumn>>startPC>>startPR;
	row = totalRow;
	column = totalColumn;
	startPosRow = startPR;
	startPosCol = startPC;
	squareArray = new Square** [row];// dynamically allocate the Squarearray
	for (unsigned int i=0;i<row;i++)
	{
		squareArray[i] = new Square*[column];// dynamically allocate a 2D square array of pointers
	}
	for (unsigned int i=0; i<row;i++)// this whole for loop is aimed to read the board's format and dynamically allocate the 2D squareArray
	{
		for (unsigned int j=0; j<column; j++)
		{
			char indicator;
			ifile>>indicator;
			if (indicator=='.')
			{
				squareArray[i][j] = new Square (1,1,false);
			}
			else if (indicator=='2')
			{
				squareArray[i][j] = new Square (2,1,false);
			}
			else if (indicator=='3')
			{
				squareArray[i][j] = new Square (3,1,false);
			}
			else if (indicator=='d')
			{
				squareArray[i][j] = new Square (1,2,false);
			}
			else if (indicator=='t')
			{
				squareArray[i][j] = new Square (1,3,false);
			}
		}
	}
	ifstream infile(init_state_namey);
	/* This nested for loop put all tiles in the init file to the board */
	for (unsigned int i=0;i<row;i++)
	{
		for (unsigned int j=0;j<3*column;j++)
		{
			char indicator;
			infile>>indicator;
			if (isalpha(indicator))
			{
				char point1,point2;
				infile>>point1>>point2;
				int actucal1 = point1-'0';
				int actucal2 = point2-'0';
				int totalPoints = actucal1*10+actucal2;
				Tile* currTile = new Tile(indicator,totalPoints);
				squareArray[i][j/3]->placeTile(currTile);
				j+=2;
			}
		}
	}
}


/* Destructor for board, delete the dynamically allocated 2D square array  */
Board::~Board ()
{
   for (unsigned int i = 0; i<row;i++)
   {
   		for (unsigned int j=0; j<column;j++)
   		{
   			delete squareArray[i][j];// delete the content
   		}
   		delete []squareArray[i];// delete the array of pointers
   }
   delete []squareArray;// delete the array itself
}

/* Returns a pointer to the Square object representing the
    (y,x) position of the board. */
Square* Board::getSquare (size_t r, size_t c) const
{
	return squareArray[c-1][r-1];
}

/*  Get the total number of rows in the board*/
size_t Board::getRows() const
{
	return row;
}

/* Get the total number of columns in the board*/
size_t Board::getColumns() const
{
	return column;
}

/* Get the starting row*/
size_t Board::getStartRow() const
{
	return startPosRow;
}

/* Get the starting column */
size_t Board::getStartCol() const
{
	return startPosCol;
}

/* Check whether the board is empty*/
bool Board::isEmpty() const
{
	for (unsigned int i=0;i<row;i++)
	{
		for (unsigned int j=0;j<column;j++)
		{
			if (squareArray[i][j]->isOccupied() == true)
				return false;
		}
	}
	return true;
}

/* Check if a specific square is occupied*/
bool Board::Occupied (size_t row1, size_t column1) const
{
	return (squareArray[row1-1][column1-1]->isOccupied());
}

/* Check whether the newly places tile is adjacent to previously placed tiles*/
bool Board::isAdjacent() const
{
	return adjacent;
}

/* Change the adjacency*/
void Board::changeAdj ()
{
	adjacent = true;
}

bool Board::hasInit () const
{
	return init;
}
/* returns a vector of all words that would be formed by executing the
    given move. The first element of the pair is a string containing the word
    formed, and the second element is the score that that word earns*/
std::vector<std::pair<std::string, unsigned int>> Board::getPlaceMoveResults(const PlaceMove &m) 
{
	adjacent = false;
	std::vector<std::pair<std::string, unsigned int>> wordsFormed; // this is the vector of pairs that need to be returned in the end
	const size_t rowTrack = m.getStartRow();// track the row
	const size_t columnTrack = m.getStartColumn(); // track the column
	vector<Tile*> placeTiles = m.tileVector();// use placeTiles to record m's tilevector
	unsigned int totalMultiplier = 1; 
	unsigned int lastWordScore = 0;
	if (m.getDirection()==true)// if it is a horizontal move
	{
		size_t columnIndicator = m.getStartColumn();// indicate which column it is in now
		for (std::vector<Tile*>::iterator tileIter = placeTiles.begin();tileIter!=placeTiles.end();tileIter++)
		{
			while(squareArray[rowTrack-1][columnIndicator-1]->isOccupied()==true)
			{
				columnIndicator++;// move to an unoccupied position
				adjacent = true;
			}
			string word;// this is the word formed by our move of one tile
			unsigned int thisWordScore = 0; // initialize the score of this word to be 0
			size_t rowUpSearch = rowTrack-1;// search up the column to get word
			if (rowUpSearch>0)
			{
				while(squareArray[rowUpSearch-1][columnIndicator-1]->isOccupied()==true)
				{
					rowUpSearch--;// Search up the row to see the the uppermost square with a tile in it
					adjacent = true;// change the adjacency
					if (rowUpSearch==0) break;
				}
			}
			rowUpSearch++;// this is the uppermost position with a tile in it
			size_t rowDownSearch = rowTrack+1; // search down the column to get word
			if (rowDownSearch<=row)
			{
				while(squareArray[rowDownSearch-1][columnIndicator-1]->isOccupied()==true)
				{
					rowDownSearch++;
					adjacent = true;
					if (rowDownSearch>row) break;// if we go out of the scope of the board, then we break
				}
			}
			rowDownSearch--;// this is the downmost position with a tile in it
			lastWordScore+=squareArray[rowTrack-1][columnIndicator-1]->getLMult()*(*tileIter)->getPoints();// updates the lastWordScore
			if (rowUpSearch==rowDownSearch)// if the newly placed tile has no adjacent tile 
			{
				totalMultiplier = totalMultiplier*squareArray[rowTrack-1][columnIndicator-1]->getWMult();// updates the total multiplier
			}
			else // if the newly placed tile has adjacent tiles 
			{
				totalMultiplier*=squareArray[rowTrack-1][columnIndicator-1]->getWMult();//updates the totalMultiplier
				unsigned int thisWordMult = squareArray[rowTrack-1][columnIndicator-1]->getWMult();// get a variable called thisWordMult to store the multiplier for this word
				for (unsigned int i=rowUpSearch;i<rowTrack;i++)// create the left part of the word and the according score
				{
					word+= squareArray[i-1][columnIndicator-1]->getLetter();
					thisWordScore+=squareArray[i-1][columnIndicator-1]->getScore();
				}
				word += (*tileIter)->getUse();// count for itself
				thisWordScore+=(*tileIter)->getPoints() * squareArray[rowTrack-1][columnIndicator-1]->getLMult();// update this word's score
				for (unsigned int i=rowTrack+1;i<=rowDownSearch;i++)// create the right part of the word and the according score
				{
					word += squareArray[i-1][columnIndicator-1]->getLetter();
					thisWordScore+=squareArray[i-1][columnIndicator-1]->getScore();
				}
				pair <std::string, unsigned int> newWord;//create a pair that has the new formed word and its score
				newWord.first = word;// the first element in the pair is the word formed
				newWord.second = thisWordScore*thisWordMult; // the second element in the word is the score of the word
				wordsFormed.push_back(newWord);// push back to the vector
			}
			columnIndicator++;
		}
		columnIndicator--;
		string LastWord;// this is the word formed in the main direction (the last word as required by the assignment)
		std::vector<Tile*>::iterator tileIter= placeTiles.begin();//create an iterator pointing at the beginning of the vector
		unsigned int columnLeftSearch = columnTrack-1;// search the left of the initial letter
		if (columnLeftSearch>0)// search for the left part of the word
		{
			while (squareArray[rowTrack-1][columnLeftSearch-1]->isOccupied()==true)// if it is occupied, then search further to the left
			{
				columnLeftSearch--;
				adjacent = true;
				if (columnLeftSearch<=0) break;
			}
		}
		columnLeftSearch++;// get the leftmost column with a tile in it
		unsigned int columnRightSearch = columnTrack;
		for (vector<Tile*>::iterator TileIter = placeTiles.begin();TileIter!=placeTiles.end();TileIter++)
		{
			if (columnRightSearch>column) break;
			while(squareArray[rowTrack-1][columnRightSearch-1]->isOccupied()==true)// search the right of the last letter
			{
				columnRightSearch++;
				adjacent = true;
				if (columnRightSearch>column) break;
			}
			columnRightSearch++;
			if(columnRightSearch<=column)
			{		
				while(squareArray[rowTrack-1][columnRightSearch-1]->isOccupied()==true)// search the right of the last letter
				{
					columnRightSearch++;
					adjacent = true;
					if (columnRightSearch>column) break;
				}
			}
		}
		columnRightSearch--;// get the rightmost column with a tile in it
		if (columnRightSearch>columnLeftSearch)// we now form the new word
		{
			for (unsigned int i=columnLeftSearch;i<=columnRightSearch;i++)
			{
				while(squareArray[rowTrack-1][i-1]->isOccupied()==false && tileIter!=placeTiles.end())
				{
					LastWord += (*tileIter)->getUse();// if it is not occupied, we get the letter from tile vector
					i++;
					if (i>columnRightSearch) break;
					tileIter++;
				}
				if (i>columnRightSearch) break;// if search goes out of the scope of board
				LastWord += squareArray[rowTrack-1][i-1]->getLetter();
				lastWordScore+=squareArray[rowTrack-1][i-1]->getScore();
			}
			std::pair <std::string,unsigned int> lastWord;// this is the last word pair
			lastWord.first = LastWord;// the first element is the word string
			lastWord.second = lastWordScore*totalMultiplier;// the second element is the total points earned
			wordsFormed.push_back(lastWord);// push back the new pair to the vector
		}
	}
    /* This is just the exact copy of previous one with row = column and column = row ! Unrevised*/
	else
	{
		size_t rowIndicator = m.getStartRow();
		for (std::vector<Tile*>::iterator tileIter = placeTiles.begin();tileIter!=placeTiles.end();tileIter++)
		{
			while(squareArray[rowIndicator-1][columnTrack-1]->isOccupied()==true)
			{
				rowIndicator++;
				adjacent = true;
			}
			string word;
			unsigned int thisWordScore = 0; 
			size_t columnLeftSearch = columnTrack-1;
			if (columnLeftSearch>0)
			{
				while(squareArray[rowIndicator-1][columnLeftSearch-1]->isOccupied()==true)
				{
					columnLeftSearch--;
					adjacent = true;
					if (columnLeftSearch<=0) break;
				}
			}
			columnLeftSearch++;
			size_t columnRightSearch = columnTrack+1;
			if (columnRightSearch<=column)
			{
				while(squareArray[rowIndicator-1][columnRightSearch-1]->isOccupied()==true)
				{
					columnRightSearch++;
					adjacent = true;
					if (columnRightSearch>column) break;
				}
			}
			columnRightSearch--;
			lastWordScore+=squareArray[rowIndicator-1][columnTrack-1]->getLMult()*(*tileIter)->getPoints();
			if (columnLeftSearch==columnRightSearch) 
				totalMultiplier = totalMultiplier*squareArray[rowIndicator-1][columnTrack-1]->getWMult();
			else
			{
				totalMultiplier*=squareArray[rowIndicator-1][columnTrack-1]->getWMult();
				unsigned int thisWordMult = squareArray[rowIndicator-1][columnTrack-1]->getWMult();
				for (unsigned int i=columnLeftSearch;i<columnTrack;i++)
				{
					word += squareArray[rowIndicator-1][i-1]->getLetter();
					thisWordScore+=squareArray[rowIndicator-1][i-1]->getScore();
				}
				word += (*tileIter)->getUse();
				thisWordScore+=(*tileIter)->getPoints() * squareArray[rowIndicator-1][columnTrack-1]->getLMult();
				for (unsigned int i=columnTrack+1;i<=columnRightSearch;i++)
				{
					word += squareArray[rowIndicator-1][i-1]->getLetter();
					thisWordScore+=squareArray[rowIndicator-1][i-1]->getScore();
				}
				pair <std::string, unsigned int> newWord;
				newWord.first = word;
				newWord.second = thisWordScore*thisWordMult; 
				wordsFormed.push_back(newWord);
			}
			rowIndicator++;
		}
		string LastWord;
		std::vector<Tile*>::iterator tileIter= placeTiles.begin();
		unsigned int rowUpSearch = rowTrack-1;
		if (rowUpSearch>0)
		{
			while (squareArray[rowUpSearch-1][columnTrack-1]->isOccupied()==true)
			{
				rowUpSearch--;
				adjacent = true;
				if (rowUpSearch<=0) break;
			}
		}
		rowUpSearch++;
		unsigned int rowDownSearch = rowTrack;
		for (vector<Tile*>::iterator TileIter = placeTiles.begin();TileIter!=placeTiles.end();TileIter++)
		{
			if (rowDownSearch>row) break;
			while(squareArray[rowDownSearch-1][columnTrack-1]->isOccupied()==true)
			{
				rowDownSearch++;
				adjacent = true;
				if (rowDownSearch>row) break;
			}
			rowDownSearch++;
			if (rowDownSearch<=row)
			{
				while(squareArray[rowDownSearch-1][columnTrack-1]->isOccupied()==true)
				{
					rowDownSearch++;
					adjacent = true;
					if (rowDownSearch>row) break;
				}
			}
		}
		rowDownSearch--;
		if (rowDownSearch>rowUpSearch)
		{
			for (unsigned int i=rowUpSearch;i<=rowDownSearch;i++)
			{
				while(squareArray[i-1][columnTrack-1]->isOccupied()==false && tileIter!=placeTiles.end())
				{
					LastWord += (*tileIter)->getUse();
					i++;
					if (i>rowDownSearch) break;
					tileIter++;
				}
				if (i>rowDownSearch) break;
				LastWord += squareArray[i-1][columnTrack-1]->getLetter();
				lastWordScore+=squareArray[i-1][columnTrack-1]->getScore();
			}
			std::pair <std::string,unsigned int> lastWord;// this is the last word pair
			lastWord.first = LastWord;// the first element is the word string
			lastWord.second = lastWordScore*totalMultiplier;// the second element is the total points earned
			wordsFormed.push_back(lastWord);// push back the new pair to the vector
			//cerr << "Last Word is: " << lastWord.first << endl;
		}

	}

	return wordsFormed;
}

/* Executes the given move by taking tiles and placing them on the board.*/
void Board::executePlaceMove (const PlaceMove&m)
{
	vector <Tile*> placeTiles = m.tileVector();// we create a placeTiles Vector to sore m's tileVector
	if (m.getDirection()==true)// if it is a horizontal mvoe
	{
		const size_t rowTrack = m.getStartRow();// we track the row
		size_t columnIndicator = m.getStartColumn();
		for (std::vector<Tile*>::iterator tileIter = placeTiles.begin();tileIter!=placeTiles.end();tileIter++)
		{
			while(squareArray[rowTrack-1][columnIndicator-1]->isOccupied()==true)
			{
				columnIndicator++;// when the current square is being occupied, we move one position forward
			}
			squareArray[rowTrack-1][columnIndicator-1]->placeTile(*tileIter);// we place the tile in the empty square
			columnIndicator++;
		}
	}
	/* This is the same as if except for switching row and column */
	else
	{
		size_t rowIndicator = m.getStartRow();
		const size_t columnTrack = m.getStartColumn();
		for (std::vector<Tile*>::iterator tileIter = placeTiles.begin();tileIter!=placeTiles.end();tileIter++)
		{
			while(squareArray[rowIndicator-1][columnTrack-1]->isOccupied()==true)
			{
				rowIndicator++;
			}
			squareArray[rowIndicator-1][columnTrack-1]->placeTile(*tileIter);
			rowIndicator++;
		}
	}
}
