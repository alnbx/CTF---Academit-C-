#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <algorithm>

class gameManager;

class directoryHandeling
{
	gameManager* _gameManager = nullptr;
	int a = 5;
	std::vector<std::string> dirContent;

public:
	directoryHandeling(gameManager* manager) : _gameManager(manager)
	{};

	void openDir(std::string& dirPath);

private:
	void parseBuffedData(std::string data, std::string& dirPath);
	void sortContent(void) { std::sort(dirContent.begin(), dirContent.end()); }
	void devideToPlayersAndBoard(void);
};