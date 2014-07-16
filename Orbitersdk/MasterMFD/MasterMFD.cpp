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

bool MasterMFD::areAtTop()
{
	return !(currentContainer->parent);
}

bool MasterMFD::ConsumeButton(int bt, int event)
{
	if (!(event & PANEL_MOUSE_LBDOWN)) return false;

	//check if this button is an sub-category or a MFD
	if (bt < currentContainer->children.size())
	{
		//go into a subcategory
		currentContainer = currentContainer->children[bt];
		return true;
	}
	if (bt >= currentContainer->children.size() && bt < currentContainer->children.size() + currentContainer->MFDS.size())
	{
		//switch to that MFD
		oapiOpenMFD(currentContainer->MFDS[bt - currentContainer->children.size()].mfdID, mfdNumber);
		return true;
	}
	return false;
}

bool MasterMFD::Update(oapi::Sketchpad* skp)
{
	resetNextButton();
	generateTreeLocation();
	drawNavigation(skp);
	drawCategories(skp);
	drawMFDS(skp);

	return true;
}

//regenerate tree location
void MasterMFD::generateTreeLocation()
{
	//start with current subcategory name
	if (currentContainer->parent != NULL)
		treeLocation = currentContainer->name;
	//go up through parents, appending name
	MFDContainer* navigationContainer = currentContainer;
	while (navigationContainer->parent != NULL )
	{
		navigationContainer = navigationContainer->parent;
		//write out this one's name, with an arrow
		if ((navigationContainer->name.compare("")))
			treeLocation.insert(0, navigationContainer->name + "->");
		
	}
}

void MasterMFD::drawNavigation(oapi::Sketchpad* skp)
{
	Title(skp, "MasterMFD");

	//draw current location in the tree
	skp->SetFont(GetDefaultFont(0));
	skp->SetTextColor(GetDefaultColour(2));
	//justify text in the center
	int xCoord = (width - skp->GetTextWidth(treeLocation.c_str())) / 2;
	drawAtLinePercentage(xCoord, .05, treeLocation.c_str(), skp);
}

void MasterMFD::drawCategories(oapi::Sketchpad* skp)
{
	skp->SetFont(GetDefaultFont(0));
	skp->SetTextColor(GetDefaultColour(0));

	//draw the current categories in green
	for (int i = 0; i < currentContainer->children.size(); i++)
	{
		drawTextAtNextButton(currentContainer->children[i]->name, skp);
	}
}

void MasterMFD::drawMFDS(oapi::Sketchpad* skp)
{

	skp->SetFont(GetDefaultFont(0));
	skp->SetTextColor(GetDefaultColour(1));

	//draw the current categories in green
	for (int i = 0; i < currentContainer->MFDS.size(); i++)
	{
		drawTextAtNextButton(currentContainer->MFDS[i].name, skp);
	}
}

void MasterMFD::drawTextAtNextButton(std::string text, oapi::Sketchpad* skp)
{
	if (nextButton < 12)
		drawTextNextToButton(nextButton, text, skp);
	nextButton++;
}

void MasterMFD::resetNextButton()
{
	nextButton = 0;
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