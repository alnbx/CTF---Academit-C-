#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <time.h>
#include <cstdlib>
#include <direct.h>
#include <string>

#include "gameManager.h"
#include "secondaryMenu.h"
#include "directoryHandle.h"
#include "Utils.h"

#define	ESC_KEY	27
#define MATCH	0

/********************************************************************************************************************************
Function Name:			activateSecondaryMenu
Return value:			bool
Description:			prints the secondary menu modul.
Dinamically allocated:	None
********************************************************************************************************************************/
static bool activateSecondaryMenu(secondaryMenu& sm)
{
	setTextColor(WHITE, BLACK);
	clearScreen();
	return (sm.printMenuAndTakeUserChoice());
}

/********************************************************************************************************************************
Function Name:			connectSecondaryMenu
Return value:			None
Description:			Connects the secondary menu module with the game board module and the primary menu module.
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::connectSecondaryMenu(secondaryMenu& sm)
{
	sm.setGameManager(this);
}

/********************************************************************************************************************************
Function Name:			bareFileName
Return value:			None
Description:			strips the path and extention from the name of the file.
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::bareFileName(std::string& fullName)
{
	stripPathFromFileName(fullName);
	fullName = cutEnding(fullName);
}

/********************************************************************************************************************************
Function Name:			createFullName
Return value:			None
Description:			Creates full name including path and extantion.
Dinamically allocated:	None
********************************************************************************************************************************/
void createFullName(const char* dirPath, std::string& fileName, char *extantion)
{
	std::string tmpFileName = dirPath;
	tmpFileName.append("\\");
	tmpFileName.append(fileName);
	tmpFileName.append(extantion);

	fileName = tmpFileName;
}

/********************************************************************************************************************************
Function Name:			reopenRecordFiles
Return value:			std::ofstream
Description:			reopens and clear the content of a file.
Dinamically allocated:	None
********************************************************************************************************************************/
std::ofstream gameManager::reopenRecordFiles(const char *fileName)
{
	playerA.setFile().close();
	playerB.setFile().close();

	std::ofstream myFile(fileName, std::ofstream::out | std::ofstream::trunc);

	return myFile;
}

/********************************************************************************************************************************
Function Name:			openFileToWrite
Return value:			None
Description:			Opens file for writing.
Dinamically allocated:	None
********************************************************************************************************************************/
std::ofstream openFileToWrite(const char* dirPath, const char *fileName, char * extantion)
{
	std::string fullFileName = fileName;
	createFullName(dirPath, fullFileName, extantion);

	std::ofstream myFile(fullFileName.c_str());

	return myFile;
}

/********************************************************************************************************************************
Function Name:			run
Return value:			None
Description:			Runs the game.
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::run()
{
	round++;

	std::string boardFileName = "random_" + std::to_string(round);
	prepareForGame();
	if (recordGame)
	{
		if (boardRandom)
		{
			appendNew(boardFileName);
			gameBoard.saveBoardToFile(dirPath.c_str(), boardFileName);
			gameBoard.setBoardName(boardFileName);
			insertToArray(boardFiles, boardFileName, ".gboard");
		}

		boardFileName = gameBoard.getBoardName();
		openFileForWrite(boardFileName.c_str());
		insertToArray(playerAFiles, boardFileName, ".moves-a_full");
		insertToArray(playerBFiles, boardFileName, ".moves-b_full");
	}

	playKeboard();

	if (recordGame)
	{
		playerA.setFile().close();
		playerB.setFile().close();
	}
}

/********************************************************************************************************************************
Function Name:			openFileForWrite
Return value:			None
Description:			Opens files for writing.
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::openFileForWrite(const char *boardName)
{
	std::string playerAFile = boardName;
	std::string playerBFile = boardName;

	createFullName(dirPath.c_str(), playerAFile, ".moves-a_full");
	createFullName(dirPath.c_str(), playerBFile, ".moves-b_full");
	
	playerA.setMovesFileName(playerAFile.c_str());
	playerB.setMovesFileName(playerBFile.c_str());
	playerA.setFile() = openFileToWrite(dirPath.c_str() ,boardName, ".moves-a_full");
	playerB.setFile() = openFileToWrite(dirPath.c_str(), boardName, ".moves-b_full");
}

/********************************************************************************************************************************
Function Name:			reopenMovesFiles
Return value:			None
Description:			reopens file.
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::reopenMovesFiles(void)
{
	std::string fileAName = gameBoard.getBoardName();
	std::string fileBName = gameBoard.getBoardName();

	createFullName(dirPath.c_str(), fileAName, ".moves-a_full");
	createFullName(dirPath.c_str(), fileBName, ".moves-a_full");

	playerA.setFile() = reopenRecordFiles(fileAName.c_str());
	playerA.setFile() = reopenRecordFiles(fileBName.c_str());
}

/********************************************************************************************************************************
Function Name:			insertToArray
Return value:			None
Description:			insert a string to an array. Used for inserting board and movement files into the files vectors.
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::insertToArray(std::vector<std::string>& vector, const std::string & boardFileName, const char * extantion)
{
	int i = 0;
	int size = vector.size();
	std::vector<std::string>::iterator it = vector.begin();

	while (i < size)
	{
		if (vector[i].compare(boardFileName) >= 0) { break; }

		i++;
	}

	vector.insert(it + i,dirPath + boardFileName + extantion);
}

/********************************************************************************************************************************
Function Name:			playKeboard
Return value:			None
Description:			Play games with keyboard.
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::playKeboard()
{
	std::ofstream& playerAMovesFile = playerA.setFile();
	std::ofstream& playerBMovesFile = playerB.setFile();
	char			choice;
	bool			breakGame = false;
	time_t			i = time(NULL);
	checker**&		myGameBoard = gameBoard.getBoard();
	secondaryMenu	sm;

	connectSecondaryMenu(sm);

	while (true)
	{
		Sleep(delay);
		if (_kbhit())
		{
			choice = tolower(_getch());
			if (choice == ESC_KEY) { if (breakGame = activateSecondaryMenu(sm)) { break; } };
			if (playerA.pressesKey(myGameBoard, choice, recordGame, playerAMovesFile, quite)) { flipScore ? playerB.addToScore(quite) : playerA.addToScore(quite); break; }
			if (playerB.pressesKey(myGameBoard, choice, recordGame, playerBMovesFile, quite)) { flipScore ? playerA.addToScore(quite) : playerB.addToScore(quite); break; }
		}
		else
		{
			if (((i++) % 2) == 0)
			{
				if (playerA.moveWithoutPressedKey(myGameBoard, recordGame, playerAMovesFile, quite)) { flipScore ? playerB.addToScore(quite) : playerA.addToScore(quite); break; }
			}
			else
			{
				if (playerB.moveWithoutPressedKey(myGameBoard, recordGame, playerBMovesFile, quite)) { flipScore ? playerA.addToScore(quite) : playerB.addToScore(quite); break; }
			}
		}

		if (playerA.areAllDead()) { flipScore ? playerA.addToScore(quite) : playerB.addToScore(quite); break; }
		else if (playerB.areAllDead()) { flipScore ? playerB.addToScore(quite) : playerA.addToScore(quite); break; }
	}
}

/********************************************************************************************************************************
Function Name:			printEqualSigns
Return value:			None
Description:			Print equal signs
Dinamically allocated:	None
********************************************************************************************************************************/
static void printEqualSigns(int len)
{
	int i = 0;

	for (; i < len; i++) { std::cout << "="; }
	std::cout << "|" << std::endl;
}

/********************************************************************************************************************************
Function Name:			printClosingFrame
Return value:			None
Description:			Print closing signs: |
Dinamically allocated:	None
********************************************************************************************************************************/
static void printClosingFrame(int maxLen, int printedSoFar)
{
	int spacesToPrint = maxLen - printedSoFar - 1;
	int i = 0;

	for (; i < spacesToPrint; i++) { std::cout << ' '; };
	std::cout << '|' << std::endl;
}

/********************************************************************************************************************************
Function Name:			printSpaces
Return value:			None
Description:			Print spaces
Dinamically allocated:	None
********************************************************************************************************************************/
static void printSpaces(int size)
{
	int i = 0;

	for (; i < size; i++) { std::cout << " "; }
}

/********************************************************************************************************************************
Function Name:			calcDigits
Return value:			None
Description:			calculate how many digits in a number.
Dinamically allocated:	None
********************************************************************************************************************************/
static int calcDigits(int score)
{
	int digits = 0;

	while (score > 0)
	{
		digits++;
		score /= 10;
	}

	return digits;
}

/********************************************************************************************************************************
Function Name:			printSummaryHead
Return value:			None
Description:			Prints the head of the game summary function.
Dinamically allocated:	None
********************************************************************************************************************************/
static void printSummaryHead(int& startPrintingCol, int &startPrintingRow, int maxLen)
{
	std::string gameSummaryStr = "Game Summary";
	int spacesToPrint = 0;
	int midSpace = 0;

	clearScreen();
	gotoxy(startPrintingCol, startPrintingRow++);
	setTextColor(BLACK, WHITE);
	std::cout << "|";
	printEqualSigns(maxLen);
	gotoxy(startPrintingCol, startPrintingRow++);
	spacesToPrint = (maxLen - gameSummaryStr.length());
	midSpace = spacesToPrint / 2;
	std::cout << "|";
	printSpaces(midSpace);
	std::cout << gameSummaryStr;
	printSpaces(spacesToPrint - midSpace);
	std::cout << "|";
}

/********************************************************************************************************************************
Function Name:			printPlayerSummary
Return value:			None
Description:			Prints the player's score in the game summary function.
Dinamically allocated:	None
********************************************************************************************************************************/
static void printPlayerSummary(int& startPrintingCol, int &startPrintingRow, int maxLen, const char *name, int score)
{
	int digits = 0;
	int spacesToPrint = 0;
	int midSpace = 0;

	gotoxy(startPrintingCol, startPrintingRow++);
	setTextColor(BLACK, WHITE);
	std::cout << "|";
	digits = calcDigits(score);
	spacesToPrint = maxLen - strlen(name) - 3 - digits;
	midSpace = spacesToPrint / 2;
	printSpaces(midSpace);
	std::cout << name << " ";
	printSpaces(3 - digits);
	std::cout << score;
	printSpaces(spacesToPrint - midSpace - 2);
	std::cout << "|";
}

/********************************************************************************************************************************
Function Name:			printFinish
Return value:			None
Description:			Prints the summary of the total games.
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::printFinish(std::string& playerAName, std::string& playerBName, int maxLen)
{
	int startPrintingRow = 12;
	int startPrintingCol = 48;
	int spacesToPrint = 0;
	int midSpace = 0;
	int digits = 0;

	printSummaryHead(startPrintingCol, startPrintingRow, maxLen);
	printPlayerSummary(startPrintingCol, startPrintingRow, maxLen, playerAName.c_str(), playerA.getScore());
	printPlayerSummary(startPrintingCol, startPrintingRow, maxLen, playerBName.c_str(), playerB.getScore());

	gotoxy(startPrintingCol, startPrintingRow);
	std::cout << "|";
	printEqualSigns(maxLen);
	std::cout << "|";
	Sleep(delay * 10);
}

/********************************************************************************************************************************
Function Name:			quiteModeSummery
Return value:			None
Description:			Prints the summery of a quite mode game
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::quiteModeSummery(int numberOfMoves, const std::string& winnerName)
{
	int startPrintingRow = 12, startPrintingCol = 48;
	clearScreen();
	gotoxy(startPrintingCol, startPrintingRow++);
	setTextColor(WHITE, BLACK);
	std::cout << "Game cycle: " << round << std::endl;
	gotoxy(startPrintingCol, startPrintingRow++);
	std::cout << "Num moves: " << numberOfMoves << std::endl;
	gotoxy(startPrintingCol, startPrintingRow);
	std::cout << "Winner: " << winnerName << std::endl;
}

/********************************************************************************************************************************
Function Name:			playWithFiles
Return value:			None
Description:			Play the game with files.
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::playWithFiles(std::ifstream& playerAfile, std::ifstream& playerBfile)
{
	int numberOfMoves = 0;
	std::string winnerName;
	time_t i = time(NULL);
	bool checkIfFileAOpen = playerAfile.good();
	bool checkIfFileBOpen = playerBfile.good();

	while (playerAfile.good() || playerBfile.good())
	{
		if (!quite) { Sleep(delay); }
		
		if ((((i++) % 2) == 0) && (checkIfFileAOpen = playerAfile.good()))
		{
			if (playerA.readMovesFormTextFile(gameBoard.getBoard(), playerAfile, quite)) { playerA.addToScore(quite); winnerName = playerA.getName(); break; }
		}
		else if (checkIfFileBOpen = playerBfile.good())
		{
			if (playerB.readMovesFormTextFile(gameBoard.getBoard(), playerBfile, quite)) { playerB.addToScore(quite); winnerName = playerB.getName(); break; }
		}
		numberOfMoves++;
		if (playerA.areAllDead()) { playerB.addToScore(quite); winnerName = playerB.getName(); break; }
		else if (playerB.areAllDead()) { playerA.addToScore(quite); winnerName = playerA.getName(); break; }
	}
	if (quite) { quiteModeSummery(numberOfMoves, winnerName); }
}

/********************************************************************************************************************************
Function Name:			cutEnding
Return value:			std::string
Description:			strips the extention from the file name.
Dinamically allocated:	None
********************************************************************************************************************************/
std::string gameManager::cutEnding(const std::string& fileName)
{
	std::string nameWithoutEnding;
	size_t extentionStarts = fileName.find_last_of('.');

	nameWithoutEnding = fileName;
	nameWithoutEnding.erase(extentionStarts, (fileName.length() - extentionStarts));

	return nameWithoutEnding;
}

/********************************************************************************************************************************
Function Name:			compareFilesName
Return value:			bool
Description:			Compare file names. return true if the name is equal
Dinamically allocated:	None
********************************************************************************************************************************/
bool gameManager::compareFilesName(int &pFileIndex, int &boardFileIndex, std::vector<std::string>& playerFiles)
{
	std::string playerAFile = cutEnding(playerFiles[pFileIndex]);
	std::string boardFile = cutEnding(boardFiles[boardFileIndex]);

	while (strcmp(playerAFile.c_str(), boardFile.c_str()) < 0)
	{
		pFileIndex++;
		playerAFile = cutEnding(playerFiles[pFileIndex]);
	}

	if (strcmp(playerAFile.c_str(), boardFile.c_str()) == 0) { return true; }
	
	return false;
}

/********************************************************************************************************************************
Function Name:			runGameWithFiles
Return value:			None
Description:			Manager function to run the game with files.
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::runGameWithFiles(void)
{
	int pAFileIndex = 0;
	int pBFileIndex = 0;
	int boardFileIndex = 0;
	int fileAsize = playerAFiles.size();
	int fileBsize = playerBFiles.size();
	int boardsFileSize = boardFiles.size();
	std::ifstream playerAfile;
	std::ifstream playerBfile;

	while (((pAFileIndex < fileAsize) || (pBFileIndex < fileBsize)) && boardFileIndex < boardsFileSize)
	{
		if (gameBoard.loadBoardFromTextFile(boardFiles[boardFileIndex].c_str(), playerA, playerB))
		{
			if (pAFileIndex < fileAsize) { if (compareFilesName(pAFileIndex, boardFileIndex, playerAFiles)) { playerAfile.open(playerAFiles[pAFileIndex++].c_str()); } }
			if (pBFileIndex < fileBsize) { if (compareFilesName(pBFileIndex, boardFileIndex, playerBFiles)) { playerBfile.open(playerBFiles[pBFileIndex++].c_str()); } }

			setGameLogic();
			if (!quite) { clearScreen();  gameBoard.printBoard(); }
			playWithFiles(playerAfile, playerBfile);

			if (playerAfile.is_open()) { playerAfile.close(); }
			if (playerBfile.is_open()) { playerBfile.close(); }
		}

		if (!quite) { Sleep(50 * delay); }
		boardFileIndex++;
	}
}

/********************************************************************************************************************************
Function Name:			appendNew
Return value:			None
Description:			adds the string "_New" to a file name
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::appendNew(std::string & boardFileName)
{
	for (auto file : boardFiles)
	{
		bareFileName(file);
		if (file == boardFileName) { boardFileName.append("_New"); }
	}
}

/********************************************************************************************************************************
Function Name:			stripPathFromFileName
Return value:			None
Description:			strips the path from the file name
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::stripPathFromFileName(std::string & file)
{
	int pathSize = dirPath.size() + 1;
	int subStringSize = file.size() - pathSize;
	file = file.substr(pathSize, subStringSize);
}

/********************************************************************************************************************************
Function Name:			finishTheGame
Return value:			None
Description:			finishes the game - manager function
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::finishTheGame(void)
{
	std::string playerAName = playerA.getName();
	std::string playerBName = playerB.getName();
	int maxLen = (max(14, max(playerAName.length(), playerBName.length())) + 3);

	printFinish(playerAName, playerBName, maxLen);
}

/********************************************************************************************************************************
Function Name:			keyboardGame
Return value:			None
Description:			play games with keyboard
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::keyboardGame()
{
	int numberOfFileBoards = boardFiles.size();

	while (!(endGame))
	{
		if (!(boardRandom))
		{
			if (numberOfFileBoards > round)
			{
				if (!firstGame)
				{
					gameBoard.clearBoard();
					playerA.resetPlayer();
					playerB.resetPlayer();
				}

				gameBoard.loadBoardFromTextFile(boardFiles[round].c_str(), playerA, playerB);
				_primarymenu.printMenuAndTakeUserChoice(this, endGame);
			}
			else
				endGame = true;
		}
		else
			_primarymenu.printMenuAndTakeUserChoice(this, endGame);

	}
}

/********************************************************************************************************************************
Function Name:			manageGame
Return value:			None
Description:			Prints the primary menu.
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::manageGame(int argc, char** argv)
{
	hideCursor();
	if (argc > 1) { takeUserParams(argc, argv); };

	openDirectory();

	if (movesKeyboard) { keyboardGame(); }
	else				{ runGameWithFiles(); }

	finishTheGame();
}

/********************************************************************************************************************************
Function Name:			setPlayersName
Return value:			None
Description:			Wrapper function for player::setName
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::setPlayersName(void)
{
	std::cout << "First Player: ";
	playerA.setName();
	std::cout << "Second Player: ";
	playerB.setName();
}

/********************************************************************************************************************************
Function Name:			resetScore
Return value:			None
Description:			Wrapper function for player::resetScore
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::resetScore(void)
{
	playerA.resetScore();
	playerB.resetScore();
}

/********************************************************************************************************************************
Function Name:			restartBoard
Return value:			None
Description:			The function is called when a user wishes to restart the game.
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::restartBoard(void)
{
	int i = 0;
	checker**& myGameBoard = gameBoard.getBoard();
	gameBoard.resetBoard();
	checker *positionsA = playerA.getStartingPosition();
	checker *positionsB = playerB.getStartingPosition();

	if (recordGame) { reopenMovesFiles(); }

	for (; i < CHECKER_NUMBER + 1; i++)
	{
		myGameBoard[positionsA[i].row][positionsA[i].col] = positionsA[i];
		myGameBoard[positionsB[i].row][positionsB[i].col] = positionsB[i];
	}

	playerA.setCheckers(CHECK1, &(myGameBoard[positionsA[0].row][positionsA[0].col]));
	playerA.setCheckers(CHECK2, &(myGameBoard[positionsA[1].row][positionsA[1].col]));
	playerA.setCheckers(CHECK3, &(myGameBoard[positionsA[2].row][positionsA[2].col]));
	playerB.setCheckers(CHECK7, &(myGameBoard[positionsB[0].row][positionsB[0].col]));
	playerB.setCheckers(CHECK8, &(myGameBoard[positionsB[1].row][positionsB[1].col]));
	playerB.setCheckers(CHECK9, &(myGameBoard[positionsB[2].row][positionsB[2].col]));
}

/********************************************************************************************************************************
Function Name:			takeUserParams
Return value:			None
Description:			take user parameters and turn flags on
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::takeUserParams(int argc, char ** argv)
{
	int i = 1;
	std::string key;
	std::string value;

	for (; i < argc; i++)
	{
		key = argv[i];

		if (!key.compare("-quiet")) { value = "t"; }
		else					    { value = argv[++i]; }

		turnOnFlags(key, value);
	}
}

/********************************************************************************************************************************
Function Name:			stringToNumber
Return value:			unsigned int
Description:			takes a string, converts it to number and returns the number
Dinamically allocated:	None
********************************************************************************************************************************/
static unsigned int stringToNumber(std::string str)
{
	unsigned int i = 0;
	unsigned int result = 0;

	for (; i < (unsigned int)str.size(); i++)
	{
		result *= 10;
		result += (str[i] - '0');
	}

	return result;
}

/********************************************************************************************************************************
Function Name:			turnOnFlags
Return value:			None
Description:			turns on the game flags based on user choice.
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::turnOnFlags(std::string key, std::string value)
{
	std::string _strBoard = "-board";
	std::string _strMoves = "-moves";
	std::string _strPath  = "-path";
	std::string _strquiet = "-quiet";
	std::string _strDelay = "-delay";

	if (MATCH == (key.compare(_strBoard)))
		determineBoardType(value);
	else if (MATCH == (key.compare(_strMoves)))
		determineMovesType(value);
	else if (MATCH == (key.compare(_strPath)))
		{ dirPath = value; runFromFiles = true; }
	else if (MATCH == (key.compare(_strquiet)))
	{
		if (checkIfQuiteIsPossibe()) { quite = true; }
		else; //Problem - illegal?
	}
	else if (MATCH == (key.compare(_strDelay)))
		delay = stringToNumber(value);
	else; //Problem - illegal parameter;
}

/********************************************************************************************************************************
Function Name:			openDirectory
Return value:			None
Description:			Opens a directory
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::openDirectory(void)
{
	directoryHandeling dir = { this };
	dir.openDir(dirPath);
}

/********************************************************************************************************************************
Function Name:			restartGame
Return value:			None
Description:			The function handles all the operation of restarting the game.
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::restartGame(void)
{
	gameRestarted = true;
	checker **&myGameBoard = gameBoard.getBoard();
	//Do we really need it?...
	/*
	checker** currCheckerA = playerA.findCurrentChecker();
	checker** currCheckerB = playerB.findCurrentChecker();
	*/

	if (recordGame) { clearFiles(); }
	//Do we really need it?...
	/*
	if (nullptr != currCheckerA) { myGameBoard[(*currCheckerA)->row][(*currCheckerA)->col].setCheckerType((*currCheckerA)->walkedOn); }
	if (nullptr != currCheckerB) { myGameBoard[(*currCheckerB)->row][(*currCheckerB)->col].setCheckerType((*currCheckerB)->walkedOn); }
	*/

	playerA.resetPlayer();
	playerB.resetPlayer();
	restartBoard();
	clearScreen();
	gameBoard.printBoard();
	printPlayersScore();
}

/********************************************************************************************************************************
Function Name:			ransomiseCheckers
Return value:			None
Description:			The function randomises the checkers for each player.
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::ransomiseCheckers(void)
{
	playerA.setCheckers(CHECK1, gameBoard.randomisePlayerChecker(CHECK1));
	playerA.setCheckers(CHECK2, gameBoard.randomisePlayerChecker(CHECK2));
	playerA.setCheckers(CHECK3, gameBoard.randomisePlayerChecker(CHECK3));
	playerB.setCheckers(CHECK7, gameBoard.randomisePlayerChecker(CHECK7));
	playerB.setCheckers(CHECK8, gameBoard.randomisePlayerChecker(CHECK8));
	playerB.setCheckers(CHECK9, gameBoard.randomisePlayerChecker(CHECK9));
	playerA.setCheckers(FlgA,	gameBoard.randomisePlayerChecker(FlgA));
	playerB.setCheckers(FlgB,	gameBoard.randomisePlayerChecker(FlgB));
}

/********************************************************************************************************************************
Function Name:			clearFiles
Return value:			None
Description:			clear players movement files
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::clearFiles(void)
{
	playerA.setFile().clear();
	playerB.setFile().clear();
}

/********************************************************************************************************************************
Function Name:			setDefaultPath
Return value:			None
Description:			sets default path
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::setDefaultPath(void)
{
	char *buffer = NULL;

	dirPath = _getcwd(buffer, _MAX_PATH);
}

/********************************************************************************************************************************
Function Name:			setPlayersAbilityToMove
Return value:			None
Description:			Sets the player's checkers the ability to move on top of forest and sea.
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::setPlayersAbilityToMove(void)
{
	playerA.getChecker(2)->setMoveForest(true);
	playerA.getChecker(2)->setMoveSea(true);
	playerA.getChecker(3)->setMoveSea(true);
	playerB.getChecker(1)->setMoveForest(true);
	playerB.getChecker(1)->setMoveSea(true);
	playerB.getChecker(2)->setMoveForest(true);
}

/********************************************************************************************************************************
Function Name:			setGameLogic
Return value:			None
Description:			sets the game logic for the players
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::setGameLogic(void)
{
	playerA.setPointersToCheckers();
	playerB.setPointersToCheckers();
	playerA.setCheckersLogic();
	playerB.setCheckersLogic();
	setPlayersAbilityToMove();
	playerA.saveStartingPosition();
	playerB.saveStartingPosition();
}
/********************************************************************************************************************************
Function Name:			prepareForGame
Return value:			None
Description:			Wrapper function to prepare the board, players and printing the board.
Dinamically allocated:	None
********************************************************************************************************************************/
void gameManager::prepareForGame(void)
{	
	if (!(firstGame) && (boardRandom)) 
	{ 
		gameBoard.resetBoard(); 
		playerA.resetPlayer();
		playerB.resetPlayer();
	}
	if (boardRandom) { ransomiseCheckers(); }
	setGameLogic();
	setTextColor(BLACK, BLACK);
	clearScreen();
	gameBoard.printBoard(); 
	printPlayersScore();
	firstGame = false;
}