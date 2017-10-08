#pragma once

#include <fstream>
#include <concurrent_queue.h>
#include "Protocol.hpp"
#include "Network.hpp"

bool	trySend(const BinaryStruct s);
void	getTime(BinaryStruct& s);

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
	void	createMouseEvent(void) const;
	bool			_is_running;
	bool			_updated;
	std::ofstream	_fs;
	std::ifstream	_in;
	Network		   *_net;
};