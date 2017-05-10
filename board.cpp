#include <iostream>
#include <ctime>
#include <cstdlib>
#include <Windows.h>
#include <fstream>
#include <limits>

#include "board.h"
#include "gameManager.h"
#include "player.h"
#include "Utils.h"

#undef max

/********************************************************************************************************************************
Function Name:			isChecker
Return value:			bool
Description:			The function checks if the square needs to be reseted. Returns true if is one of the players checkers.
Dinamically allocated:	None
********************************************************************************************************************************/
bool isChecker(types check)
{
	if ((check == CHECK1) || (check == CHECK2) || (check == CHECK3) || (check == CHECK7) || (check == CHECK8) || (check == CHECK9))
		return true;

	return false;
}

/********************************************************************************************************************************
Function Name:			emptyBoard
Return value:			None
Description:			The function sets all the checkers to BLANK types (empty cells).
Dinamically allocated:	None
********************************************************************************************************************************/
void board::emptyBoard(checker**& gameBoard)
{
	int row = 0;
	int col = 0;

	for (row = 0; row < _boardSize + INITIAL_POINT; row++)
		for (col = 0; col < _boardSize + INITIAL_POINT; col++)
		{
			gameBoard[row][col].setCheckerType(BLANK);
			gameBoard[row][col].setPosition(col, row);
		}
}

/********************************************************************************************************************************
Function Name:			openfileForReading
Return value:			std::ifstream
Description:			The function opens a file and returns its' file descriptor.
Dinamically allocated:	None
********************************************************************************************************************************/
std::ifstream openfileForReading(const char* fileName)
{
	std::ifstream myfile; 
	myfile.open(fileName);
	return myfile;
}

/********************************************************************************************************************************
Function Name:			createBoardFromFile
Return value:			bool
Description:			The function reads from a file and builds a board accordingly. returns false if the input is illegal.
Dinamically allocated:	lineOfBoard(char *)
********************************************************************************************************************************/
bool board::createBoardFromFile(std::ifstream& file, player& playerA, player& playerB)
{
	int countRow = INITIAL_POINT;
	int countCol = INITIAL_POINT;
	std::string lineOfBoard;
	types typeOfChecker;

	emptyCheckLegalChecker();

	while (countRow < _boardSize + INITIAL_POINT)
	{
		getline(file, lineOfBoard, '\n');

		while (countCol < _boardSize + INITIAL_POINT)
		{
			if (isNewLine(lineOfBoard[countCol - INITIAL_POINT])) { fillWithBlanks(countRow, countCol); break; }
			typeOfChecker = determineCheckerType(lineOfBoard[countCol - INITIAL_POINT]);
			if (CHECKERERROR != typeOfChecker)
			{
				gameBoard[countRow][countCol].setCheckerType(typeOfChecker);
				updatePlayerIfNeeded(gameBoard[countRow][countCol].getType(), countRow, countCol, playerA, playerB);
				checkLegalChecker[typeOfChecker]++;
				countCol++;
			}
		}
		countRow++;
		countCol = INITIAL_POINT;
	}

	return true;
}

/********************************************************************************************************************************
Function Name:			determineCheckerType
Return value:			types
Description:			The function returns the type of checker base of the letter that has been read from the file.
Dinamically allocated:	None
********************************************************************************************************************************/
types board::determineCheckerType(char check)
{
	switch (check)
	{
		case (' '): return BLANK;
		case ('F'): return FR;
		case ('S'): return SEA;
		case ('A'): return FlgA;
		case ('B'): return FlgB;
		case ('1'): return CHECK1;
		case ('2'): return CHECK2;
		case ('3'): return CHECK3;
		case ('7'): return CHECK7;
		case ('8'): return CHECK8;
		case ('9'): return CHECK9;
		default:
			BoardErrors.pushErrorChar(check);
			return CHECKERERROR;
	}
}

/********************************************************************************************************************************
Function Name:			checkLegalLine
Return value:			bool
Description:			The function checks if a char is illigal. Returns true if legal and false if illegal.
Dinamically allocated:	None
********************************************************************************************************************************/
bool board::checkLegalChar(char let)
{
	if ((let == '1') || (let == '2') || (let == '3') || (let == '7') || (let == '8') || (let == '9') || (let == ' ') || (let == 'T') || (let == 'S') || (let == 'A') || (let == 'B'))
		return true;

	return false;
}

/********************************************************************************************************************************
Function Name:			checkLegalAmountOfCheckers
Return value:			bool
Description:			The function checks if there is only one checker of each kind per player. Returns true if the amount of the unique checker is legit.
Dinamically allocated:	None
********************************************************************************************************************************/
bool board::checkLegalAmountOfCheckers(int startingChecker)
{
	int i = 0;

	for (; i < CHECKER_NUMBER; i++)
		if (checkLegalChecker[startingChecker + i] != 1)
			return false;

	return true;
}

/********************************************************************************************************************************
Function Name:			checkLegalLine
Return value:			bool
Description:			The function sets the rest of the row to BLANK checkers.
Dinamically allocated:	None
********************************************************************************************************************************/
void board::fillWithBlanks(int row, int colsSoFar)
{
	for (; colsSoFar < _boardSize; colsSoFar++)
		gameBoard[row][colsSoFar].setCheckerType(BLANK);
}

void board::printErrorsOfBoard(void)
{

}

/********************************************************************************************************************************
Function Name:			messageAnError
Return value:			None
Description:			The function prints an appropriate message if an error accoured while loading a board from a text file.
Dinamically allocated:	None
********************************************************************************************************************************/
void board::messageErrorPlayers(const char *fileName, bool legalAmountPlayerA, bool legalAmountPlayerB, bool creationOfBoard)
{
	if (!legalAmountPlayerA)
		std::cout << "Wrong settings for player A tools in file " << fileName << std::endl;

	if (!legalAmountPlayerB)
		std::cout << "Wrong settings for player B tools in file " << fileName << std::endl;

	if (!creationOfBoard)
		printErrorsOfBoard();
}

void board::updatePlayerIfNeeded(types checkerType, int row, int col, player & playerA, player & playerB)
{
	if (isChecker(checkerType))
	{
		if ((checkerType == CHECK1) || (checkerType == CHECK2) || (checkerType == CHECK3)) 
			playerA.setCheckers(checkerType, &(gameBoard[row][col]));
		else 
			playerB.setCheckers(checkerType, &(gameBoard[row][col]));
		gameBoard[row][col].setPosition(col, row);
	}
}

/********************************************************************************************************************************
Function Name:			checkLegalLine
Return value:			bool
Description:			The function checks if the line is illigal. Returns true if legal and false if illegal.
Dinamically allocated:	None
********************************************************************************************************************************/
bool board::checkLegalLine(char * line)
{
	int i = 0;

	for (; i < _boardSize; i++)
		if (!checkLegalChar(line[i])) { return false; };

	return true;
}

/********************************************************************************************************************************
Function Name:			setFlags
Return value:			None
Description:			The function sets both flags on the board - default mode (!)
Dinamically allocated:	None
********************************************************************************************************************************/
static void setFlags(checker**& gameBoard) 
{	
	gameBoard[12 + INITIAL_POINT][1 + INITIAL_POINT].setCheckerType(FlgB);
	gameBoard[12 + INITIAL_POINT][1 + INITIAL_POINT].setPosition(1 + INITIAL_POINT, 12 + INITIAL_POINT);
	gameBoard[0 + INITIAL_POINT][10 + INITIAL_POINT].setCheckerType(FlgA);
	gameBoard[0 + INITIAL_POINT][10 + INITIAL_POINT].setPosition(10 + INITIAL_POINT, 0 + INITIAL_POINT);
};

/********************************************************************************************************************************
Function Name:			setForestAndSeaAux
Return value:			None
Description:			Auxilary function to set forest and sea.
Dinamically allocated:	None
********************************************************************************************************************************/
static void setForestAndSeaAux(checker**& gameBoard, int row, int col, types type)
{
	gameBoard[row][col].setCheckerType(type);
	gameBoard[row][col].setPosition(col, row);
}

/********************************************************************************************************************************
Function Name:			setForest
Return value:			None
Description:			The function sets the forest squars - default mode (!)
Dinamically allocated:	None
********************************************************************************************************************************/
static void setForest(checker**& gameBoard)
{
	setForestAndSeaAux(gameBoard, 3 + INITIAL_POINT, 2 + INITIAL_POINT, FR);
	setForestAndSeaAux(gameBoard, 4 + INITIAL_POINT, 2 + INITIAL_POINT, FR);
	setForestAndSeaAux(gameBoard, 5 + INITIAL_POINT, 2 + INITIAL_POINT, FR);
	setForestAndSeaAux(gameBoard, 5 + INITIAL_POINT, 3 + INITIAL_POINT, FR);

	for (int i = 0; i < 4; i++) { setForestAndSeaAux(gameBoard, 6 + INITIAL_POINT, i + INITIAL_POINT, FR); };
	setForestAndSeaAux(gameBoard, 7 + INITIAL_POINT, 0 + INITIAL_POINT, FR);
	setForestAndSeaAux(gameBoard, 7 + INITIAL_POINT, 3 + INITIAL_POINT, FR);
	setForestAndSeaAux(gameBoard, 8 + INITIAL_POINT, 3 + INITIAL_POINT, FR);
}

/********************************************************************************************************************************
Function Name:			setSea
Return value:			None
Description:			The function sets the sea squars - default mode (!)
Dinamically allocated:	None
********************************************************************************************************************************/
static void setSea(checker**& gameBoard)
{
	setForestAndSeaAux(gameBoard, 3 + INITIAL_POINT, 9 + INITIAL_POINT, SEA);
	setForestAndSeaAux(gameBoard, 4 + INITIAL_POINT, 8 + INITIAL_POINT, SEA);
	setForestAndSeaAux(gameBoard, 4 + INITIAL_POINT, 9 + INITIAL_POINT, SEA);

	for (int i = 0; i < 3; i++) { setForestAndSeaAux(gameBoard, 5 + INITIAL_POINT, 7 + i + INITIAL_POINT, SEA); };
	setForestAndSeaAux(gameBoard, 6 + INITIAL_POINT, 9 + INITIAL_POINT, SEA);
	setForestAndSeaAux(gameBoard, 6 + INITIAL_POINT, 10 + INITIAL_POINT, SEA);

	for (int i = 0; i < 3; i++) { setForestAndSeaAux(gameBoard, 7 + INITIAL_POINT, 9 + i + INITIAL_POINT, SEA); };
	setForestAndSeaAux(gameBoard, 8 + INITIAL_POINT, 9 + INITIAL_POINT, SEA);
	setForestAndSeaAux(gameBoard, 8 + INITIAL_POINT, 10 + INITIAL_POINT, SEA);
	setForestAndSeaAux(gameBoard, 9 + INITIAL_POINT, 9 + INITIAL_POINT, SEA);
}

/********************************************************************************************************************************
Function Name:			setForestAndSeaAux
Return value:			checker*
Description:			The function randomises checkers on a ready board.
Dinamically allocated:	None
********************************************************************************************************************************/
checker* board::randomisePlayerChecker(types checkerToRandom)
{
	int offset = 0;
	int randVal = 0;
	char col = 0;
	char row = 0;
	std::srand(std::time(0));

	if (checkerToRandom > FlgA) { offset = 8; };

	while (true)
	{
		randVal = std::rand();
		row = (randVal % 5) + INITIAL_POINT + offset;
		col = (randVal % _boardSize) + INITIAL_POINT;

		if ((gameBoard[row][col].getType()) == BLANK)
		{
			gameBoard[row][col].setCheckerType(checkerToRandom);
			gameBoard[row][col].setPosition(col, row);
			return &gameBoard[row][col];
		}
	}
}

/********************************************************************************************************************************
Function Name:			loadBoardFromTextFile
Return value:			None
Description:			The function loads a board from file.
Dinamically allocated:	None
********************************************************************************************************************************/
bool board::loadBoardFromTextFile(const char *fileName, player& playerA, player& playerB)
{	
	bool creationOfBoard = false;
	bool legalAmountPlayerA = false;
	bool legalAmountPlayerB = false;


	std::ifstream file = openfileForReading(fileName);
	if (!(file.is_open())) { std::cout << "Could not open " << fileName << std::endl; exit(-2); return false; } // TODO: exit with no exit
	
	creationOfBoard = createBoardFromFile(file, playerA, playerB);
	//printBoard();
	legalAmountPlayerA = checkLegalAmountOfCheckers((int)CHECK1);
	legalAmountPlayerB = checkLegalAmountOfCheckers((int)CHECK7);

	if (!(legalAmountPlayerA && legalAmountPlayerB && creationOfBoard))
	{
		messageErrorPlayers(fileName, legalAmountPlayerA, legalAmountPlayerB, creationOfBoard);
		return false;
	}


	file.close();

	return true;
}

void board::drawBoardLine(int row)
{
	int col = 0;

	for (col = 0; col < _boardSize + INITIAL_POINT; col++)
	{
		if (gameBoard[row][col].getType() == BLANK)
		{
			if (col == 0 && row > 0 && row <= 9)
				std::cout << " " << row << "  ";
			else if (col == 0 && row > 0 && row > 9)
				std::cout << " " << row << " ";
			else if (col > 0 && row == 0)
				std::cout << " " << (char)('A' + col - 1) << "  ";
			else
				std::cout << "    ";
		}
		else
			drawWithColor(gameBoard[row][col].getType(), BLACK);

		setTextColor(WHITE);  std::cout << "|";
	}
	std::cout << std::endl;
}

void board::drawLineSeperators(void)
{
	int col = 0;

	for (; col <= (_boardSize + INITIAL_POINT) * 5; col++)
		std::cout << '-';
}

/********************************************************************************************************************************
Function Name:			printBoard
Return value:			None
Description:			The function prints the board.
Dinamically allocated:	None
********************************************************************************************************************************/
void board::printBoard()
{
	int row = 0;
	int col = 0;

	for (; row < _boardSize+INITIAL_POINT; row++)
	{
		drawLineSeperators();
		std::cout << std::endl;
		std::cout << '|';

		drawBoardLine(row);
	}

	drawLineSeperators();
	std::cout << std::endl;
}

/********************************************************************************************************************************
Function Name:			resetBoard
Return value:			None
Description:			The function resets the board before the next game.
Dinamically allocated:	None
********************************************************************************************************************************/
void board::resetBoard(void)
{
	int row = 0;
	int col = 0;

	for (; row <= _boardSize; row++)
	{
		for (col = 0; col <= _boardSize; col++)
		{
			if (isChecker(gameBoard[row][col].getType()))
				gameBoard[row][col].resetChecker(_boardSize);
		}
	}
}

void board::clearBoard(void)
{
	checker clearedChecker;
	int row = 0, col = 0;

	for (; row < _boardSize + INITIAL_POINT; ++row)
	{
		for (col = 0; col < _boardSize + INITIAL_POINT; ++col)
			gameBoard[row][col] = clearedChecker;
	}
}

/********************************************************************************************************************************
Function Name:			emptyCheckLegalChecker
Return value:			None
Description:			The function resets the buckets of the checkers.
Dinamically allocated:	None
********************************************************************************************************************************/
void board::emptyCheckLegalChecker(void)
{
	int i = 0;

	for (; i < _typesSize; i++)
		checkLegalChecker[i] = 0;
}

char board::letterOfType(types checkerType)
{
	if (checkerType == BLANK)
		return ' ';
	else if (checkerType == FR)
		return 'F';
	else if (checkerType == SEA)
		return 'S';
	else if (checkerType == FlgA)
		return 'A';
	else if (checkerType == FlgB)
		return 'B';
	else
		return ((char)((int)checkerType + '0'));
}

void board::saveBoardToFile(const char * fileName)
{
	std::ofstream boardFile = openFileToWrite(fileName, ".gboard");

	for (int i = INITIAL_POINT; i < _boardSize + INITIAL_POINT; i++)
	{
		for (int j = INITIAL_POINT; j < _boardSize + INITIAL_POINT; j++)
		{
			char checkerSign = letterOfType(gameBoard[i][j].getType());
			boardFile << checkerSign;
		}

		boardFile << "# " << i;
		boardFile << std::endl;
	}
	boardFile << "-------------" << std::endl;
	boardFile << "ABCDEFGHIJKLM";
	boardFile.close();
}

/********************************************************************************************************************************
Function Name:			createBoard
Return value:			None
Description:			The function creates the board by calling all the checkers setters.
Dinamically allocated:	None
********************************************************************************************************************************/
void board::createBoard(void)
{
	setFlags(gameBoard);
	setForest(gameBoard);
	setSea(gameBoard);
}

/********************************************************************************************************************************
Function Name:			drawWithColor
Return value:			None
Description:			The function draws the checkers with the correct color. Used mostly by the print board function.
Dinamically allocated:	None
********************************************************************************************************************************/
void drawWithColor(types type, int backroundColor)
{
	if (type == BLANK) { setTextColor(BLACK); std::cout << "    ";}
	else if (type == FlgA) { setTextColor(DARK_CYAN); std::cout << ' ' << (char) 223 << (char)186 << ' '; }
	else if (type == FlgB) { setTextColor(RED); std::cout << ' ' << (char)223 << (char)186 << ' '; }
	else if (type == FR) { setTextColor(GREEN, DARK_GREEN); std::cout << " ^^ "; }
	else if (type == SEA) { setTextColor(BLUE, DARK_BLUE);  std::cout << " ~~ "; }
	else
	{
		if ((type == CHECK1) || (type == CHECK2) || (type == CHECK3))
		{
			setTextColor(DARK_CYAN, backroundColor);
			std::cout << " " << type << "  ";
		}
		else
		{
			setTextColor(RED, backroundColor);
			std::cout << " " << type << "  ";
		}
	}
}