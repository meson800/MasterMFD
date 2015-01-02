#pragma once

#include <string>
#include <vector>
#include "PersistData.h"


enum ButtonType
{
	NAV,CAT,MFD
};

enum NavType
{
	UP = 0
};

struct ButtonData
{
	ButtonData(ButtonType _type, int _id) : buttonType(_type), id(_id) {}
	ButtonType buttonType;
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
	void drawCategories(oapi::Sketchpad* skp);
	void drawMFDS(oapi::Sketchpad* skp);

	void generateTreeLocation();
	int calculatePages();

	void drawTextAtNextButton(const std::string& text, ButtonData info, oapi::Sketchpad* skp);
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