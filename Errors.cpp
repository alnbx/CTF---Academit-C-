#include "Errors.h"

typedef struct _bucket
{
	char ch;
	int counter = 0;
} bucket;

static void addToBucket(std::vector<bucket>& tmpList, char ch)
{
	size_t i = 0;
	bool isNew = true;

	for (; i < tmpList.size(); i++) 
		if (ch == tmpList[i].ch) { tmpList[i].counter++; isNew = false; } 

	if (isNew) { tmpList.push_back( { ch, 1 } ); }
}

void Errors::dropDuplications(void)
{
	size_t i = 0;
	char currentChar = 0;
	std::vector<bucket>	tmpList;

	for (; i < charsError.size(); i++)	 { addToBucket(tmpList, charsError[i]); }
	charsError.clear();
	for (i = 0; i < tmpList.size(); i++) { charsError.push_back(tmpList[i].ch); }
}

void Errors::printErrors(const char* fileName)
{
	dropDuplications();

	for (auto i : charsError)	{ std::cout << "Wrong character on board " << i << " in file " << fileName << std::endl; }
}