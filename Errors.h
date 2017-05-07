#pragma once

#include <iostream>
#include <vector>

class Errors
{
	std::vector<char>	charsError = {};

public:
	Errors()
	{

	}

	void pushErrorChar(char c)		{ charsError.push_back(c); }
	void dropDuplications(void);
};