#pragma once

#include <fstream>
#include <concurrent_queue.h>
#include "Protocol.hpp"
#include "Network.hpp"

bool trySend(const BinaryStruct s);

class EventManager
{
public:
	EventManager(Network *& net);
	~EventManager();
	void addEvent(const BinaryStruct& b);
	void run();
	void kill();
private:
	concurrency::concurrent_queue<BinaryStruct>	msgs;
	bool			_is_running;
	std::ofstream		_fs;
	Network			*_net;
};