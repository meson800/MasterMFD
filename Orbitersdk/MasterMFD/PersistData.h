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

struct MFDContainer
{
	lineIterator fill(lineIterator currentLine, int whitespaceLevel);

	std::string name;
	MFDContainer * parent;
	std::vector<MFDContainer*> children;
	std::vector<MFDData> MFDS;
};

class PersistantData
{
public:
	static bool fillLineData(std::string treeFilename);

	static std::vector<LineData> lines;
	static MFDContainer topContainer;
	static bool containerReady;

};