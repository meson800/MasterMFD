#pragma once

class MasterMFD : public MFD2
{
public:
	MasterMFD(int _mfdNumber, DWORD w, DWORD h, VESSEL *vessel);
	//char *ButtonLabel(int bt);
	//int  ButtonMenu(const MFDBUTTONMENU **menu) const;
	//void Update(HDC hDC);
	static int MsgProc(UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);
private:
	int mfdNumber;
};