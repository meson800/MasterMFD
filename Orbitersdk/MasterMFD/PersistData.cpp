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

//returns true if this container is empty
bool MFDContainer::checkContainer()
{
	//first, recurse into children, removing them if they are empty
	std::vector<MFDContainer*>::iterator it = children.begin();
	while (it != children.end())
	{
		
		if ((*it)->checkContainer())
		{
			//it's empty, delete it
			delete (*it);
			//remove element and cleanup
			it = children.erase(it);
		}
		else
			it++;
	}

	//now check our MFD's, and fill them if they exist
	std::vector<MFDData>::iterator MFDit = MFDS.begin();
	while (MFDit != MFDS.end())
	{
		int mfdID = oapiGetMFDModeSpecEx((char*)(MFDit->name.c_str()));
		//check if we have a legit MFD
		if (mfdID != MFD_NONE)
		{
			MFDit->mfdID = mfdID;
			MFDit++;
		}
		//otherwise, delete it!
		else
		{
			//log it
			oapiWriteLog((char*)(std::string("Couldn't find MFD named:" + MFDit->name).c_str()));

			//delete it
			MFDit = MFDS.erase(MFDit);
		}
	}

	//now, return true if we have either an MFD or a subcategory
	if (children.size() > 0 || MFDS.size() > 0)
		return true;
	else
		return false;
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

//ONLY CALL THIS WHEN WE ARE ALLOWED TO-AFTER INITIAL LOAD, DURING SIMULATION
void PersistantData::readyContainer()
{
	//
}