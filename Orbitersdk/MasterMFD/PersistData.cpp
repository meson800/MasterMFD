#include "PersistData.h"

bool PersistantData::containerReady = false;
std::vector<LineData> PersistantData::lines;

lineIterator MFDContainer::fill(lineIterator currentLine, int whitespaceLevel)
{
	//this works recursively

	//return when we go above our whitespace level (less whitespace)
	while (currentLine->numWhitespace >= whitespaceLevel)
	{
		//Check the next two lines's whitespace level.
		int currentLevel, nextLevel;
		currentLevel = currentLine->numWhitespace;
		nextLevel = (currentLine + 1)->numWhitespace;

		//if both are at the same whitespace level, current line is an MFD.
		if (currentLevel == whitespaceLevel && nextLevel == whitespaceLevel)
		{
			MFDS.push_back(MFDData(currentLine->name));
		}

		//if  the next level is greater than this level, then it's another container
		if (currentLevel == whitespaceLevel && nextLevel > currentLevel)
		{
			children.push_back(new MFDContainer);
			children.back()->parent = this;
			children.back()->name = currentLine->name;
			currentLine = children.back()->fill(currentLine + 1, nextLevel);
			//continue so we refresh our current line
			continue;
		}
	}

	//we're done!  return the next line
	return currentLine + 1;
}

//Reads each line, and counts whitespace characters in it to fill lines
//Filename is from ORBITER_ROOT/Config/MasterMFD/
bool PersistantData::fillLineData(std::string treeFilename)
{
	//open ifstream to file
	std::string filename = "Config/MasterMFD/" + treeFilename;
	std::ifstream treeFile;
	treeFile.open(treeFilename.c_str());
	if (treeFile.is_open() != true)
		return false;

	//now, read line by line
	std::string currentLine;
	while (!treeFile.eof())
	{

		//grab the line
		std::getline(treeFile, currentLine);

		//start extracting characters
		int whitespaceCount = 0;
		std::string name;

		while (currentLine.size() != 0)
		{
			char currentChar = currentLine.front();
			if (currentChar == ' ' || currentChar == '\t')
			{
				//increase whitespace count
				whitespaceCount++;
				currentLine.erase(0, 1);
			}
			else
			{
				//take the rest of the string as the name
				name = currentLine;
				currentLine.erase();
			}
		}

		//if this isn't an empty line, push back a line
		if (name.size() > 0)
			lines.push_back(LineData(whitespaceCount, name));
	}
}