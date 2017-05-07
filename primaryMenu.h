#pragma once

#include <iostream>
#include <string>

#include "defs.h"
#include "Utils.h"

class gameManager;

class primaryMenu
{
	userChoice choice = EXIT;

public:
	primaryMenu()
	{

	}


	void printMenuAndTakeUserChoice(gameManager* manager, bool endGame);

private:
	void handleUserChoice(gameManager* manager, bool endGame);
	void printMenu(gameManager *manager);
};