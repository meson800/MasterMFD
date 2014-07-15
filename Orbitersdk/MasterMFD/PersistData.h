#include <vector>
#include <string>
#include <Orbitersdk.h>

struct LineData
{
	int numWhitespace;
	std::string name;
};

struct MFDData
{
	int mfdID;
	std::string name;
};

struct MFDContainer
{
	std::string name;
	MFDContainer * parent;
	std::vector<MFDContainer*> children;
	std::vector<MFDData> MFDS;
};

class PersistantData
{
public:
	//static bool fillLineData(FILEHANDLE treeFile);

	static std::vector<LineData> lines;
	static MFDContainer topContainer;
	static bool containerReady;

};