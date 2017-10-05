#pragma once

#include <thread>
#include <chrono>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "Protocol.hpp"

enum
{
	max_length = 1024
};

class client
{
public:
	client(boost::asio::io_service& io_service,
		boost::asio::ssl::context& context,
		boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
	bool verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx);
	void handle_connect(const boost::system::error_code& error);
	void handle_handshake(const boost::system::error_code& error);
	void readData(const boost::system::error_code& error, size_t bytes_transferred);

private:
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
	char request_[max_length];
	char reply_[max_length];
	bool _sendData;
};

class Network
{
public:
	Network();
	~Network();
	void run(void);
private:
	client *_client;
};