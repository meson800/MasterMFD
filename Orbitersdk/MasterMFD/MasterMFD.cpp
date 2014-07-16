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

void MasterMFD::drawTextNextToButton(int buttonNum, std::string text, oapi::Sketchpad* skp)
{
	double percentY = .15 * (buttonNum % 5);
	//if buttonNum is less than 5 (on the left side), just draw normally
	//otherwise, draw out from the right
	if (buttonNum < 5)
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