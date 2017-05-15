#include <iostream>

#include "primaryMenu.h"
#include "gameManager.h"
#include "player.h"

class gameManager;
class player;

/********************************************************************************************************************************
Function Name:			printMenu
Return value:			None
Description:			The name indicates it all...
Dinamically allocated:	None
********************************************************************************************************************************/
void primaryMenu::printMenu(gameManager *manager)
{
	clearScreen();
	std::cout << "[1] Choose names" << std::endl;
	std::cout << "[2] Start the game" << std::endl;
	std::cout << "[3] Start the game in reversed mode" << std::endl;
	std::cout << "[4] Reset score" << std::endl;
	manager->isGameRecording() ? std::cout << "[5] Stop Recording Game" << std::endl : std::cout << "[5] Record Game" << std::endl;
	std::cout << "[9] Exit Game" << std::endl;
	manager->printPlayersScore();
}

/********************************************************************************************************************************
Function Name:			printMenuAndTakeUserChoice
Return value:			None
Description:			The function takes and handles the user's choice.
Dinamically allocated:	None
********************************************************************************************************************************/
void primaryMenu::printMenuAndTakeUserChoice(gameManager *manager, bool &endGame)
{
	unsigned char userChose = 0;

	if (endGame)
		return;

	printMenu(manager);

	std::cin >> userChose;
	choice = (userChoice)(userChose - '0');
	handleUserChoice(manager, endGame);
}

/********************************************************************************************************************************
Function Name:			printMenuAndTakeUserChoice
Return value:			None
Description:			The function handles the user's choice and sets the desired action.
Dinamically allocated:	None
********************************************************************************************************************************/
void primaryMenu::handleUserChoice(gameManager *manager, bool &endGame)
{
	unsigned char userChose = 0;
	bool loopCheck = true;

	while (loopCheck)
	{
		switch (choice)
		{
		case (CHOOSE_NAMES):
			manager->setPlayersName();
			printMenu(manager);
			break;

		case (START_GAME):
			manager->setReverseScore(false);
			manager->run();
			loopCheck = false;
			break;

		case (REVERSED_MODE):
			manager->setReverseScore(true);
			manager->run();
			loopCheck = false;
			break;

		case (RESET_SCORE):
			manager->resetScore();
			printMenu(manager);
			break;

		case(RECORD_GAME):
			manager->startAndStopRecordingGame();
			printMenu(manager);
			break;

		case (EXIT):
			std::cout << "Bye Bye..." << std::endl;
			loopCheck = false;
			endGame = true;
			break;

		default:
			printMenu(manager);
			std::cout << "\nBad Choice! Please try again: ";
			break;
		}

		if (loopCheck)
		{
			std::cin >> userChose;
			choice = (userChoice)(userChose - '0');
		}
	}
}
