#pragma once

#include <string>
#include <vector>
#include "PersistData.h"

enum NavType
{
	UP = 0,
	PREVIOUS_PAGE,
	NEXT_PAGE
};

struct ButtonData
{
	ButtonData(ItemType _type, int _id) : itemType(_type), id(_id) {}
	ButtonData() : itemType(ItemType::NAV), id(0) {}
	ItemType itemType;
	int id;
};

class MasterMFD : public MFD2
{
public:
	MasterMFD(int _mfdNumber, DWORD w, DWORD h, VESSEL *vessel);
	//char *ButtonLabel(int bt);
	//int  ButtonMenu(const MFDBUTTONMENU **menu) const;
	bool ConsumeButton(int bt, int event);
	bool Update(oapi::Sketchpad* skp);
	static int MsgProc(UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);
private:
	void drawNavigation(oapi::Sketchpad* skp);
	void drawItems(oapi::Sketchpad* skp, int numItems);

	void generateTreeLocation();
	int calculatePages();

	void drawTextAtNextButton(const std::string& text, ButtonData info, oapi::Sketchpad* skp);
	void drawItemNextToButton(const std::string& text, ButtonData info, int buttonNum, oapi::Sketchpad* skp);

	void resetNextButton();
	void drawTextNextToButton(int buttonNum, const std::string& text, oapi::Sketchpad* skp);
	void drawAtLinePercentage(int xLoc, double percentY, const std::string& text, oapi::Sketchpad* skp);
	void drawCenteredAtLinePercentage(double percentY, const std::string& text, oapi::Sketchpad* skp);

	bool areAtTop();

	std::vector<ButtonData> buttons;

	std::string treeLocation;
	DWORD width, height;
	MFDContainer* currentContainer;
	int mfdNumber;
	int nextButton;
	int currentPage, pages;
};