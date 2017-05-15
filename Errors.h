#pragma once

#include <iostream>
#include <vector>

#include "Utils.h"

class Errors
{
	std::vector<char>	charsError = {};

public:
	Errors()
	{

	}

	void dropDuplications(void);
	void printErrors(const char* fileName);
	void pushErrorChar(char c)		{ charsError.push_back(c); }
	void emptyErrors(void)			{ charsError.clear(); }
	bool isErrorExists(void)
	{
		if (charsError.size() > 0) { return true; }
		
		return false;
	}
};