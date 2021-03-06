#include "PersistData.h"

bool PersistantData::switchMFD = false;
int PersistantData::mfdNum = 0;
int PersistantData::mfdMode = 0;
bool PersistantData::containerReady = false;
std::vector<LineData> PersistantData::lines;
MFDContainer PersistantData::topContainer;

lineIterator MFDContainer::fill(lineIterator currentLine, int whitespaceLevel)
{
	//return if we got a bad current line
	if (currentLine == PersistantData::lines.end())
		return currentLine;

	//return when we go above our whitespace level (less whitespace)
	while (currentLine->numWhitespace >= whitespaceLevel)
	{
		//Check the next two lines's whitespace level.
		int currentLevel, nextLevel;
		currentLevel = currentLine->numWhitespace;

		//prevent this from exploding
		if (currentLine + 1 != PersistantData::lines.end())
			nextLevel = (currentLine + 1)->numWhitespace;
		else
			nextLevel = 0;

		//if both are at the same whitespace level, or next level is less current line is an MFD.
		if (currentLevel == whitespaceLevel && nextLevel <= whitespaceLevel)
		{
			MFDS.push_back(MFDData(currentLine->name));

			//add to the item list
			items.push_back(ItemData(ItemType::MFD, MFDS.size() - 1)); //size-1 to get the current index
			++currentLine;
		}

		//if  the next level is greater than this level, then it's another container
		if (currentLevel == whitespaceLevel && nextLevel > currentLevel)
		{
			children.push_back(new MFDContainer);
			children.back()->parent = this;
			children.back()->name = currentLine->name;

			//add to the item list
			items.push_back(ItemData(ItemType::CAT, children.size() - 1)); //size-1 to get current index

			currentLine = children.back()->fill(currentLine + 1, nextLevel);
		}
		//return if we got a bad current line
		if (currentLine == PersistantData::lines.end())
			return currentLine;
	}

	//we're done!  return the next line
	return currentLine;
}

//returns true if this container is empty
bool MFDContainer::checkContainer()
{

	std::vector<ItemData>::iterator it = items.begin();
	while (it != items.end())
	{
		switch (it->itemType)
		{
			case ItemType::CAT:
			{
				//check the category, remove if necessary
				if (!(children[it->index]->checkContainer()))
				{
					//empty container, remove this reference to it after deleting it
					//but NOT removing it from children, as that would mess up the indexes
					delete children[it->index];
					it = items.erase(it);
				}
				else
				{
					it++;
				}
				break;
			}
			case ItemType::MFD:
			{
				int mfdID = oapiGetMFDModeSpecEx((char*)(MFDS[it->index].name.c_str()));
				//check if we have a legit MFD
				if (mfdID != MFD_NONE)
				{
					MFDS[it->index].mfdID = mfdID;
					it++;
				}
				//otherwise, delete it from the items list
				else
				{
					//log it
					oapiWriteLog((char*)(std::string("[MasterMFD] Couldn't find MFD named-" + MFDS[it->index].name).c_str()));

					//delete it
					it = items.erase(it);
				}
				break;
			}
			default:
				break;

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
	treeFile.open(filename.c_str());
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
	return true;
}

//ONLY CALL THIS WHEN WE ARE ALLOWED TO-AFTER INITIAL LOAD, DURING SIMULATION
void PersistantData::readyContainer()
{
	if (!containerReady)
	{
		//set top container's parent so we can distiguish it later
		topContainer.parent = NULL;

		//return if we don't have any lines
		if (lines.size() == 0)
		{
			oapiWriteLog("[MasterMFD] No lines read.");
			return;
		}
		//fill everything
		lineIterator lineIt = lines.begin();
		topContainer.fill(lineIt, lines[0].numWhitespace);

		//ready the upper-level container!
		if (topContainer.checkContainer())
		{
			containerReady = true;
		}
	}
}