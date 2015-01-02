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
	PersistantData::fillLineData("mainTree.txt");

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

DLLCLBK void opcPreStep(double simt, double simdt, double mjd)
{
	if (PersistantData::switchMFD)
	{
		//switch it!
		oapiOpenMFD(PersistantData::mfdMode, PersistantData::mfdNum);
		//rest switchMFD
		PersistantData::switchMFD = false;
	}

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

	switch (buttons[bt].buttonType)
	{
		case ButtonType::NAV:
		{
			//switch the ID
			switch (buttons[bt].id)
			{
			case NavType::UP:
				//it's the back button, advance up the tree
				currentContainer = currentContainer->parent;
				generateTreeLocation();
				return true;
				break;
			}
			break;
		}
		case ButtonType::CAT:
		{
			//go into a subcategory
			currentContainer = currentContainer->children[buttons[bt].id];
			//update location
			generateTreeLocation();
			return true;
			break;
		}
	case ButtonType::MFD:
		{
			//schedule a MFD switch at next frame
			PersistantData::mfdMode = currentContainer->MFDS[buttons[bt].id].mfdID;
			PersistantData::mfdNum = mfdNumber;
			PersistantData::switchMFD = true;
			return true;
			break;
		}
	default:
		return false;
	}
	return false;
}

bool MasterMFD::Update(oapi::Sketchpad* skp)
{
	resetNextButton();
	drawNavigation(skp);
	drawCategories(skp);
	drawMFDS(skp);

	return true;
}

//regenerate tree location
void MasterMFD::generateTreeLocation()
{
	treeLocation = "";
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

	//set pages correctly
	currentPage = 1;
	pages = calculatePages();
}

int MasterMFD::calculatePages()
{
	int numItems = currentContainer->children.size() + currentContainer->MFDS.size();
	//if we have up to 11 items, then we have one page
	if (numItems <= 11)
		return 1;
	//if we have more, then we can only fit 9 items per page (we need two buttons for previous/next page)
	return ceil((float)numItems / 9.0);
}

void MasterMFD::drawNavigation(oapi::Sketchpad* skp)
{
	Title(skp, "MasterMFD");

	//draw current location in the tree
	skp->SetFont(GetDefaultFont(0));
	skp->SetTextColor(GetDefaultColour(2));

	//draw tree location
	//justify text in the center

	drawAtLinePercentage(xCoord, .05, treeLocation.c_str(), skp);

	//draw page identifier
	std::string pageString = "Page " + std::to_string(currentPage) + " of " + std::to_string(pages);
	xCoord = (width - skp->GetTextWidth(pageString.c_str())) / 2;
	drawAtLinePercentage(xCoord, .95, pageString.c_str(), skp);


	//if we aren't at the top, draw a "back" button to advance up a category
	if (!areAtTop())
		drawTextAtNextButton("Up a level", ButtonData(ButtonType::NAV, NavType::UP), skp);
}

void MasterMFD::drawCategories(oapi::Sketchpad* skp)
{
	skp->SetFont(GetDefaultFont(0));
	skp->SetTextColor(GetDefaultColour(0));

	//draw the current categories in green
	for (unsigned int i = 0; i < currentContainer->children.size(); i++)
	{
		drawTextAtNextButton(currentContainer->children[i]->name, ButtonData(ButtonType::CAT, i), skp);
	}
}

void MasterMFD::drawMFDS(oapi::Sketchpad* skp)
{

	skp->SetFont(GetDefaultFont(0));
	skp->SetTextColor(GetDefaultColour(1));

	//draw the current categories in green
	for (unsigned int i = 0; i < currentContainer->MFDS.size(); i++)
	{
		drawTextAtNextButton(currentContainer->MFDS[i].name, ButtonData(ButtonType::MFD,i), skp);
	}
}

void MasterMFD::drawTextAtNextButton(std::string text, ButtonData info, oapi::Sketchpad* skp)
{
	buttons.push_back(info);
	if (nextButton < 12)
		drawTextNextToButton(nextButton, text, skp);
	nextButton++;
}

void MasterMFD::resetNextButton()
{
	buttons.clear();
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
	skp->Text(xLoc, (int)(percentY * height), text.c_str(), text.size()); 
}

void MasterMFD::drawCenteredAtLinePercentage(double percentY, std::string text, oapi::Sketchpad* skp)
{
	int xCoord = (width - skp->GetTextWidth(text.c_str())) / 2;
	drawAtLinePercentage(xCoord, percentY, text, skp);

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