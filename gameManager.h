#pragma once
#include <iostream>
#include <vector>

#include "defs.h"
#include "primaryMenu.h"
#include "board.h"
#include "player.h"
#include "Checkers.h"
#include "Utils.h"

class primaryMenu;
class secondaryMenu;

std::ofstream openFileToWrite(const char* dirPath, const char * fileName, char * extantion);

class gameManager
{
	board						gameBoard;
	player						playerA = { (std::string)"A", FlgB };
	player						playerB = { (std::string)"B", FlgA };
	primaryMenu					_primarymenu;
	unsigned int				delay = 200;
	int							round = 0;
	bool						flipScore = false;
	bool						gameRestarted = false;
	bool						firstGame = true;
	bool						boardRandom = true;
	bool						movesKeyboard = true;
	bool						recordGame = false;
	bool						runFromFiles = false;
	bool						quite = false;
	bool						endGame = false;
	std::string					dirPath = "";
	std::vector<std::string>	boardFiles;
	std::vector<std::string>	playerAFiles;
	std::vector<std::string>	playerBFiles;

public: 
	gameManager()
	{
		playerA.setKeys("waxd123");
		playerB.setKeys("ijml789");
		setDefaultPath();
	}

	void manageGame(int argc, char** argv);
	void run();
	void setPlayersName(void);
	void resetScore(void);
	void restartGame(void);
	void ransomiseCheckers(void);
	void startAndStopRecordingGame(void) { recordGame = ((recordGame + 1) % 2); }
	void printPlayersScore(void)
	{
		std::cout << playerA.getName() << " score: " << playerA.getScore() << "   " << playerB.getName() << " score: " << playerB.getScore() <<std::endl;
	}
	void setReverseScore(bool reverse) { flipScore = reverse; }
	bool isGameRecording(void) const { return recordGame; }
	void clearFiles(void);

private:
	void setDefaultPath(void);
	void setPlayersAbilityToMove(void);
	void prepareForGame(void);
	void connectSecondaryMenu(secondaryMenu& sm);
	void restartBoard(void);
	void takeUserParams(int argc, char** argv);
	void turnOnFlags(std::string key, std::string value);
	void determineBoardType(std::string value) { if (value[0] == 'f') { boardRandom = false; } }
	void determineMovesType(std::string value) { if (value[0] == 'f') { movesKeyboard = false; runFromFiles = true; } }
	void openDirectory(void);
	void playKeboard();
	void finishTheGame(void);
	void keyboardGame();
	void gameManager::setGameLogic(void);
	void printFinish(std::string& playerAName, std::string& playerBName, int maxLen);
	void quiteModeSummery(int numberOfMoves,const std::string & winnerName);
	void playWithFiles(std::ifstream & playerAfile, std::ifstream& playerBfile);
	std::string cutEnding(const std::string& fileName);
	bool compareFilesName(int & pAFileIndex, int & boardFileIndex, std::vector<std::string>& playerFiles);
	void runGameWithFiles(void);
	void appendNew(std::string& boardFileName);
	void stripPathFromFileName(std::string& file);
	void bareFileName(std::string& file);
	void openFileForWrite(const char *boardName);
	void reopenMovesFiles(void);
	void insertToArray(std::vector<std::string>& vector, const std::string& boardFileName, const char *extantion);
	std::ofstream reopenRecordFiles(const char *fileName);
	bool checkIfQuiteIsPossibe(void)
	{
		if (!boardRandom && !movesKeyboard) { return true; }
		else								  return false;
	}

	friend class secondaryMenu;
	friend class directoryHandeling;
};