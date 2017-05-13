#pragma once
#include <iostream>
#include <fstream>
#include <vector>

#include "Checkers.h"
#include "defs.h"
#include "Errors.h"

bool isChecker(types check);
void drawWithColor(types type, int backroundColor);
std::ifstream openfileForReading(char* fileName);

class player;

class board
{
	int _boardSize;
	int						 checkLegalChecker[_typesSize];
	bool					 _filesBoard = false;
	checker**				 gameBoard = new checker*[_boardSize + INITIAL_POINT];
	std::string				 boardName = "";
	Errors					 BoardErrors;

public:
	board(bool filesBoard = false, int boardSize = SIZE) : _boardSize(boardSize), _filesBoard(filesBoard)
	{
		for (int i = 0; i < _boardSize +INITIAL_POINT ; i++)
			gameBoard[i] = new checker[_boardSize +INITIAL_POINT];

		emptyBoard(gameBoard);
		if (!(_filesBoard)) { createBoard(); }
	}

	~board()
	{
		for (int i = 0; i < _boardSize + INITIAL_POINT; i++)
			delete[] gameBoard[i];

		delete[] gameBoard;
	}

	checker**& getBoard(void) { return gameBoard; }
	checker* randomisePlayerChecker(types playerChecker);
	bool loadBoardFromTextFile(const char *fileName, player& playerA, player& playerB);
	void printBoard(void);
	void resetBoard(void);
	void clearBoard(void);
	void emptyCheckLegalChecker(void);
	void saveBoardToFile(const char *path, std::string& fileName);
	void setBoardName(const std::string& _boardName) { boardName = _boardName; }
	const char* getBoardName(void) const { return boardName.c_str(); }

private:
	void createBoard(void);
	void emptyBoard(checker**& gameBoard);
	void fillWithBlanks(int row, int colsSoFar);
	void messageErrorPlayers(const char *fileName, bool legalAmountPlayerA, bool legalAmountPlayerB, bool creationOfBoard);
	void updatePlayerIfNeeded(types checkerType, int row, int col, player& playerA, player& playerB);
	void drawBoardLine(int row);
	void drawLineSeperators(void);
	void printErrorsOfBoard(const char *fileName) { BoardErrors.printErrors(fileName); }
	char letterOfType(types checkerType);
	bool createBoardFromFile(const char* fileName, std::ifstream& file, player& playerA, player& playerB);
	types determineCheckerType(char check);
	bool checkLegalLine(char *line);
	bool checkLegalChar(char let);
	bool checkLegalAmountOfCheckers(int startingChecker);
	bool isNewLine(char charOfBoard)
	{
		if (charOfBoard == '\n') { return true; };
		
		return false;
	}
};