#include <exception>
#include "EventManager.hpp"

EventManager::EventManager(Network *& net) : _net(net)
{
	_fs.open("C:/Users/antoine/logs.log", std::ofstream::out);
	if (_fs)
		_is_running = true;
	else
	{
		OutputDebugStringW(L"FILE OPEN FAILED\n");
		_is_running = false;
	}
}

EventManager::~EventManager() { _fs.close(); }

void EventManager::kill(void) { _is_running = false; }

void EventManager::addEvent(const BinaryStruct& b)
{
	msgs.push(b);
}

void EventManager::run()
{
	BinaryStruct	s;

	OutputDebugStringW(L"run eventmanager\n");
	while (_is_running)
	{
		while (!msgs.empty())
		{
			msgs.try_pop(s);
			if (!trySend(s))
			{
				_fs.write((char *)&s, 11);
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	OutputDebugStringW(L"done eventmanager\n");
}

