#include <algorithm>

#include "directoryHandle.h"
#include "gameManager.h"

#define MATCH	0

void directoryHandeling::parseBuffedData(std::string data, std::string& dirPath)
{
	int end = 0;
	std::string tmp;

	while ((end = data.find('\n')) != std::string::npos) 
	{
		tmp = dirPath;
		tmp += "\\";
		tmp += data.substr(0, end);
		dirContent.push_back(tmp);
		data.erase(0, end + 1);
		tmp.clear();
	}
}

void directoryHandeling::openDir(std::string& dirPath)
{
	char buffer[4096];
	std::string data;
	std::string command = "2>NUL dir /a - d /b ";
	command += dirPath;

	FILE* fp = _popen(command.c_str(), "r");

	while (fgets(buffer, 4095, fp))
	{
		data += std::string(buffer);
	}
	_pclose(fp);

	parseBuffedData(data, dirPath);
	sortContent();
	devideToPlayersAndBoard();
}

void directoryHandeling::devideToPlayersAndBoard(void)
{
	size_t index = 0; 
	size_t extentionStarts = 0;
	std::string extention = "";

	for (; index < dirContent.size(); index++)
	{
		extention.clear();
		extentionStarts = dirContent[index].find_last_of('.');
		extention.assign(dirContent[index].c_str() + extentionStarts);
		//every file being pushed is already sorted!
		if		(MATCH == (extention.compare(".gboard")))		 { _gameManager->boardFiles.push_back(dirContent[index]);}
		else if (MATCH == (extention.compare(".moves-a_full")))  { _gameManager->playerAFiles.push_back(dirContent[index]); }
		else if (MATCH == (extention.compare(".moves-b_full")))  { _gameManager->playerBFiles.push_back(dirContent[index]); }
		else; //Exit???
	}
}
