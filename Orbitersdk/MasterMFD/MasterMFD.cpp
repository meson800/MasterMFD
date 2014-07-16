#define STRICT
#define ORBITER_MODULE
#include "Orbitersdk.h"
#include "PersistData.h"
#include "MasterMFD.h"

int mfdIdentifierMode;

DLLCLBK void InitModule(HINSTANCE hDLL)
{
	static char *name = "Master";
	MFDMODESPECEX spec;
	spec.name = name;
	spec.key = OAPI_KEY_M;
	spec.context = NULL;
	spec.msgproc = MasterMFD::MsgProc;

	mfdIdentifierMode = oapiRegisterMFDMode(spec);

	//load lines
	PersistantData::fillLineData("testTree.txt");

}

DLLCLBK void opcOpenRenderViewport(HWND hRenderWnd, DWORD width, DWORD height, BOOL fullscreen)
{
	//ready container
	PersistantData::readyContainer();
}

DLLCLBK void ExitModule(HINSTANCE hDLL)
{
	oapiUnregisterMFDMode(mfdIdentifierMode);
}

MasterMFD::MasterMFD(int _mfdNumber, DWORD w, DWORD h, VESSEL *vessel)
: MFD2(w, h, vessel), width(w), height(h), mfdNumber(_mfdNumber)
{
	//start with default container
	currentContainer = &(PersistantData::topContainer);
}

bool MasterMFD::Update(oapi::Sketchpad* skp)
{
	skp->SetPen(GetDefaultPen(0));
	skp->SetFont(GetDefaultFont(0));
	drawTextNextToButton(0, "test test 0", skp);
	drawTextNextToButton(1, "test test 1", skp);
	drawTextNextToButton(2, "test test 2", skp);
	drawTextNextToButton(3, "test test 3", skp);
	drawTextNextToButton(4, "test test 4", skp);
	drawTextNextToButton(5, "test test 5", skp);
	drawTextNextToButton(6, "test test 6", skp);
	drawTextNextToButton(7, "test test 7", skp);
	drawTextNextToButton(8, "test test 8", skp);
	drawTextNextToButton(9, "test test 9", skp);
	drawTextNextToButton(10, "test test 10", skp);
	drawTextNextToButton(11, "test test 11", skp);

	return true;
}

void MasterMFD::drawTextNextToButton(int buttonNum, std::string text, oapi::Sketchpad* skp)
{
	double percentY = .143 * (buttonNum % 6) + .14;
	//if buttonNum is less than 6 (on the left side), just draw normally
	//otherwise, draw out from the right
	if (buttonNum < 6)
		drawAtLinePercentage(3, percentY, text, skp); //3 pixels out for spacing
	else
		drawAtLinePercentage(width - skp->GetTextWidth(text.c_str()) - 3, percentY, text, skp);
}

void MasterMFD::drawAtLinePercentage(int xLoc, double percentY, std::string text, oapi::Sketchpad* skp)
{
	skp->Text(xLoc, percentY * height, text.c_str(), text.size()); 
}

// message parser
int MasterMFD::MsgProc(UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
		case OAPI_MSG_MFD_OPENEDEX: {
			MFDMODEOPENSPEC *ospec = (MFDMODEOPENSPEC*)wparam;
			return (int)(new MasterMFD(mfd,ospec->w, ospec->h, (VESSEL*)lparam));
		}
	}
	return 0;
}