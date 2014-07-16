#pragma once

#include <string>
#include "PersistData.h"

class MasterMFD : public MFD2
{
public:
	MasterMFD(int _mfdNumber, DWORD w, DWORD h, VESSEL *vessel);
	//char *ButtonLabel(int bt);
	//int  ButtonMenu(const MFDBUTTONMENU **menu) const;
	//void Update(oapi::Sketchpad* skp);
	static int MsgProc(UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);
private:
	void drawTextNextToButton(int buttonNum, std::string text, oapi::Sketchpad* skp);
	void drawAtLinePercentage(int xLoc, double percentY, std::string text, oapi::Sketchpad* skp);
	DWORD width, height;
	MFDContainer* currentContainer;
	int mfdNumber;
};