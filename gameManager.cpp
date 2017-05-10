#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <time.h>
#include <cstdlib>
#include <direct.h>

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


std::ofstream openFileToWrite(const char *fileName, char * extantion)
{
	std::string tmpFileName = fileName;
	tmpFileName.append(extantion);

	std::ofstream myFile(tmpFileName.c_str());

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
	std::ofstream playerAMovesFile;
	std::ofstream playerBMovesFile;
	
	round++;
	prepareForGame();
	if (recordGame)
	{
		if (boardRandom)
		{
			std::string boardFileName = "random_" + std::to_string(round);
			gameBoard.saveBoardToFile(boardFileName.c_str());

			playerAMovesFile = openFileToWrite(boardFileName.c_str(), ".moves-a_full");
			playerBMovesFile = openFileToWrite(boardFileName.c_str(), ".moves-b_full");
		}

		else
		{
			playerAMovesFile = openFileToWrite(gameBoard.getBoardName(), ".moves-a_full");
			playerBMovesFile = openFileToWrite(gameBoard.getBoardName(), ".moves-b_full");
		}
	}

	playKeboard(playerAMovesFile, playerBMovesFile);

	if (recordGame)
	{
		playerAMovesFile.close();
		playerBMovesFile.close();
	}
}

void gameManager::playKeboard(std::ofstream& playerAMovesFile, std::ofstream& playerBMovesFile)
{
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

static void printEqualSigns(int len)
{
	int i = 0;

	for (; i < len; i++) { std::cout << "="; }
	std::cout << "|" << std::endl;
}

static void printClosingFrame(int maxLen, int printedSoFar)
{
	int spacesToPrint = maxLen - printedSoFar - 1;
	int i = 0;

	for (; i < spacesToPrint; i++) { std::cout << ' '; };
	std::cout << '|' << std::endl;
}

void gameManager::printFinish(std::string& playerAName, std::string& playerBName, int maxLen)
{
	int startPrintingRow = 12;
	int startPrintingCol = 48;
	std::string gameSummaryStr = "Game Summary";

	//TODO: finish to handle the frame
	clearScreen();
	gotoxy(startPrintingCol, startPrintingRow++);
	setTextColor(WHITE, BLACK);
	std::cout << "|";
	printEqualSigns(maxLen);
	gotoxy(startPrintingCol, startPrintingRow++);
	std::cout << "| " << gameSummaryStr; 
	printClosingFrame(maxLen, gameSummaryStr.length());
	gotoxy(startPrintingCol, startPrintingRow++);
	std::cout << "| " << playerAName << " " << playerA.getScore();
	printClosingFrame(maxLen, playerAName.length() + 3);
	gotoxy(startPrintingCol, startPrintingRow++);
	std::cout << "| " << playerBName << " " << playerB.getScore() << " |" << std::endl;
	gotoxy(startPrintingCol, startPrintingRow);
	std::cout << "|";
	printEqualSigns(maxLen);
	Sleep(delay * 10);
}

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

void gameManager::playWithFiles(std::ifstream& playerAfile, std::ifstream& playerBfile)
{
	int numberOfMoves = 0;
	std::string winnerName;
	time_t i = time(NULL);
	bool checkIfFileAOpen = playerAfile.good();
	bool checkIfFileBOpen = playerBfile.good();

	while (playerAfile.good() || playerBfile.good())
	{
		Sleep(delay);
		
		if ((((i++) % 2) == 0) && checkIfFileAOpen)
		{
			if (playerA.readMovesFormTextFile(gameBoard.getBoard(), playerAfile, quite)) { playerA.addToScore(quite); winnerName = playerA.getName(); break; }
		}
		else if (checkIfFileBOpen)
		{
			if (playerB.readMovesFormTextFile(gameBoard.getBoard(), playerBfile, quite)) { playerB.addToScore(quite); winnerName = playerB.getName(); break; }
		}
		numberOfMoves++;
		if (playerA.areAllDead()) { playerB.addToScore(quite); winnerName = playerB.getName(); break; }
		else if (playerB.areAllDead()) { playerA.addToScore(quite); winnerName = playerA.getName(); break; }
	}
	if (quite) { quiteModeSummery(numberOfMoves, winnerName); }
}

void gameManager::runGameWithFiles(void)
{
	int fileAsize = playerAFiles.size();
	int fileBsize = playerBFiles.size();
	int boardsCounter = boardFiles.size();
	int roundsNum = min(boardsCounter, max(fileAsize, fileBsize));
	std::ifstream playerAfile;
	std::ifstream playerBfile;

	for (; (round < roundsNum); ++round)
	{
		if ((fileAsize - round) > 0) { playerAfile.open(playerAFiles[round].c_str()); }
		if ((fileBsize - round) > 0) { playerBfile.open(playerBFiles[round].c_str()); }

		gameBoard.loadBoardFromTextFile(boardFiles[round].c_str(), playerA, playerB);
		if (!quite) { gameBoard.printBoard(); }
		playWithFiles(playerAfile, playerBfile);

		if (playerAfile.is_open()) { playerAfile.close(); }
		if (playerBfile.is_open()) { playerBfile.close(); }
		Sleep(50 * delay);
	}
}

void gameManager::finishTheGame(void)
{
	std::string playerAName = playerA.getName();
	std::string playerBName = playerB.getName();
	int maxLen = (max(14, max(playerAName.length(), playerBName.length())) + 3);

	//if (runFromFiles)	{ printFinish(playerAName, playerBName, maxLen); }
	printFinish(playerAName, playerBName, maxLen);
}

void gameManager::keyboardGame()
{
	bool endGame = false;
	int numberOfFileBoards = boardFiles.size();

	while (!(endGame))
	{
		if (numberOfFileBoards > round)
		{
			if (!(boardRandom))
			{
				if (!firstGame)
				{
					gameBoard.clearBoard();
					playerA.resetPlayer();
					playerB.resetPlayer();
				}

				gameBoard.loadBoardFromTextFile(boardFiles[round].c_str(), playerA, playerB);
			}
			_primarymenu.printMenuAndTakeUserChoice(this, endGame);
		}
		else
			endGame = true;
	}
	finishTheGame();
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

	if (runFromFiles) { openDirectory(); };

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

	for (; i < CHECKER_NUMBER; i++)
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
	//playerA.setCheckers(&(myGameBoard[positionsA[0].row][positionsA[0].col]), &(myGameBoard[positionsA[1].row][positionsA[1].col]), &(myGameBoard[positionsA[2].row][positionsA[2].col]));
	//playerB.setCheckers(&(myGameBoard[positionsB[0].row][positionsB[0].col]), &(myGameBoard[positionsB[1].row][positionsB[1].col]), &(myGameBoard[positionsB[2].row][positionsB[2].col]));
}

void gameManager::takeUserParams(int argc, char ** argv)
{
	int i = 1;
	std::string key;
	std::string value;

	for (; i < argc; i++)
	{
		key = argv[i];

		if (!key.compare("-quite")) { value = "t"; }
		else					    { value = argv[++i]; }

		turnOnFlags(key, value);
	}
}

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

void gameManager::turnOnFlags(std::string key, std::string value)
{
	std::string _strBoard = "-board";
	std::string _strMoves = "-moves";
	std::string _strPath  = "-path";
	std::string _strquiet = "-quite";
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
	checker** currCheckerA = playerA.findCurrentChecker();
	checker** currCheckerB = playerB.findCurrentChecker();
	if (nullptr != currCheckerA) { myGameBoard[(*currCheckerA)->row][(*currCheckerA)->col].setCheckerType((*currCheckerA)->walkedOn); }
	if (nullptr != currCheckerB) { myGameBoard[(*currCheckerB)->row][(*currCheckerB)->col].setCheckerType((*currCheckerB)->walkedOn); }

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
	gameBoard.randomisePlayerChecker(FlgA);
	gameBoard.randomisePlayerChecker(FlgB);
}

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
	playerA.setPointersToCheckers();
	playerB.setPointersToCheckers();
	playerA.setCheckersLogic();
	playerB.setCheckersLogic();
	setPlayersAbilityToMove();
	playerA.saveStartingPosition();
	playerB.saveStartingPosition();
	setTextColor(BLACK, BLACK);
	clearScreen();
	gameBoard.printBoard(); 
	printPlayersScore();
	firstGame = false;
}
