#pragma once

#include <string>
#include <fstream>
#include <vector>

#include "defs.h"
#include "Checkers.h"

class checker;

class player
{
	std::string				 name;
	int						 score = 0;
	char					 gameKeys[KEYS_NUMBER];
	checker*				 firstChecker;
	checker*				 secondChecker;
	checker*				 thirdChecker;
	checker*				 flagPosition;
	types					 winningFlag;
	types					 currentChecker = BLANK;
	int						 numberOfCheckersLive = CHECKER_NUMBER;
	checker					 startingPosition[4];
	std::vector<std::string> movingFiles = {};
	std::ofstream			 movesFile;
	std::string				 movesFileName;

public:
	player(std::string& playerName, types flag) : 
		name(playerName),
		winningFlag(flag)
	{

	}
	
	void setKeys(char *keysToSet);
	bool pressesKey(checker**& gameBoard, char key, bool recordGame, std::ofstream& movesFile, bool quite);
	bool player::moveWithoutPressedKey(checker**& board, bool recordGame, std::ofstream& movesFile, bool quite);
	void setName(void);
	void printName(void) { std::cout << name << std::endl; };
	bool areAllDead(void);
	void addToScore(bool quite);
	checker *getChecker(int cheknum);
	void updatePlayer();
	void setPointersToCheckers(void);
	void setCheckersLogic(void);
	void resetScore(void) { score = 0; }
	void saveStartingPosition(void) 
	{ 
		startingPosition[0] = *firstChecker; 
		startingPosition[1] = *secondChecker; 
		startingPosition[2] = *thirdChecker; 
		startingPosition[3] = *flagPosition;
	};
	checker* getStartingPosition(void) { return startingPosition; }
	void resetPlayer(void);
	void setCheckers(types checkerNum, checker *checkerToSet);
	/*
	void setCheckers(checker* _firstChecker, checker* _secondChecker, checker* _thirdChecker) 
	{ 
		firstChecker = _firstChecker; 
		secondChecker = _secondChecker; 
		thirdChecker = _thirdChecker; 
		
	}
	*/
	void setMovesFileName(const char* fileName) { movesFileName = fileName; }
	//void reopenMovesFile(void);
	int getScore(void) { return score; }
	std::string getName(void) { return name; }
	checker** findCurrentChecker(void);
	bool readMovesFormTextFile(checker**& board, std::ifstream &movesFile, bool quite);
	std::ofstream& setFile() { return movesFile; };

private:
	bool moveFromFile(checker**& board, int keys[], bool quite);
	bool isCheckerKey(char key);
	void updateCheckerAddress(checker**& board, checker ** currentChecker);
	void writeToFile(checker * check, std::ofstream & movesFile);
};