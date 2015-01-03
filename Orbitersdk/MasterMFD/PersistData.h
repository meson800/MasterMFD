#pragma once

#include <vector>
#include <string>
#include <Orbitersdk.h>
#include <fstream>

#include "globals.h"

struct LineData
{
	LineData(int _numWhitespace,std::string _name) : numWhitespace(_numWhitespace), name(_name) {}
	int numWhitespace;
	std::string name;
};

struct MFDData
{
	MFDData(std::string _name) : name(_name) {}
	int mfdID;
	std::string name;
};

enum ItemType
{
	CAT, MFD, NAV
};

struct ItemData
{
	ItemData(ItemType _itemType, int _id) : itemType(_itemType), index(_id) {}
	ItemType itemType;
	int index;
};

struct MFDContainer
{
	lineIterator fill(lineIterator currentLine, int whitespaceLevel);
	bool checkContainer();

	std::string name;
	MFDContainer * parent;
	std::vector<MFDContainer*> children;
	std::vector<MFDData> MFDS;
	std::vector<ItemData> items;
};

class PersistantData
{
public:
	static bool fillLineData(std::string treeFilename);
	static void readyContainer();
	static std::vector<LineData> lines;
	static MFDContainer topContainer;
	static bool containerReady;
	static bool switchMFD;
	static int mfdNum, mfdMode;

};