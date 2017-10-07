#include <iostream>
#include <boost/lexical_cast.hpp>
#include <mutex>
#include "Network.hpp"

std::mutex mtx;
std::vector<BinaryStruct> toSend;

Network::Network()
{
}

Network::~Network()
{
}

bool trySend(const BinaryStruct s)
{
	mtx.lock();
	toSend.push_back(s);
	std::wstringstream tmp;
	tmp << "Add data to list, size : " << toSend.size() << "\n";
	OutputDebugStringW(tmp.str().c_str());
	mtx.unlock();
	return true;
}

void Network::run()
{
	OutputDebugStringW(L"run network\n");
	begin:
	try
	{
		OutputDebugStringW(L"Client created\n");
		boost::asio::io_service _io_service;
		boost::asio::ip::tcp::resolver resolver(_io_service);
		boost::asio::ip::tcp::resolver::query query("10.19.254.190", "4242");
		boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
		boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
		ctx.load_verify_file("C:/Users/antoine/Documents/cpp_spider/cert/server.crt");
		_client = new client(_io_service, ctx, iterator);
		_io_service.run();
	}
	catch (std::exception &e)
	{
		std::wstringstream tmp;

		tmp << e.what() << "\n";
		OutputDebugStringW(tmp.str().c_str());
		delete _client;
		goto begin;
	}
	OutputDebugStringW(L"End network\n");
}

client::client(boost::asio::io_service& io_service,
	boost::asio::ssl::context& context,
	boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
	: socket_(io_service, context)
{
	_sendData = true;
	socket_.set_verify_mode(boost::asio::ssl::verify_peer);
	socket_.set_verify_callback(boost::bind(&client::verify_certificate, this, _1, _2));
	boost::asio::async_connect(socket_.lowest_layer(), endpoint_iterator,
		boost::bind(&client::handle_connect, this,
			boost::asio::placeholders::error));
}


bool client::verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx)
{
	char subject_name[256];
	X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());

	X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
	std::cout << "Verifying " << subject_name << "\n";
	return preverified;
}

void client::handle_connect(const boost::system::error_code& error)
{
	if (!error)
	{
		OutputDebugStringW(L"CONNECTED\n");
		socket_.async_handshake(boost::asio::ssl::stream_base::client,
			boost::bind(&client::handle_handshake, this,
				boost::asio::placeholders::error));
	}
	else
	{
		std::cout << "Connect failed: " << error.message() << "\n";
		throw std::exception("Conection failed");
	}
}

void client::handle_handshake(const boost::system::error_code& error)
{
	if (!error)
	{
		if ((std::string)reply_ == "START")
			_sendData = true;
		else if ((std::string)reply_ == "STOP")
			_sendData = false;

		mtx.lock();
		if (toSend.size() == 0 || !_sendData)
		{
			BinaryStruct tmp;
			tmp.key = 0;
			//read data
			mtx.unlock();
			boost::asio::async_write(socket_,
				boost::asio::buffer(&tmp, sizeof(BinaryStruct)),
				boost::bind(&client::readData, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
		else if (toSend.size() > 0 && _sendData)
		{
			BinaryStruct data;
			//send data to server
			data = toSend.front();
			boost::array<BinaryStruct, 1> binData = {data};
			toSend.erase(toSend.begin());
			mtx.unlock();
			boost::asio::async_write(socket_,
				boost::asio::buffer(binData),
				boost::bind(&client::readData, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
	}
	else
	{
		std::cout << "Handshake failed: " << error.message() << "\n";
		throw std::exception("Handshake failed");
	}
}

void client::readData(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (!error)
	{
		//read data to reply
		OutputDebugStringW(L"READING\n");
		std::memset(reply_, 0, max_length);
		boost::asio::async_read(socket_,
			boost::asio::buffer(reply_, bytes_transferred),
			boost::bind(&client::handle_handshake, this,
				boost::asio::placeholders::error));
	}
	else
	{
		OutputDebugStringW(L"READ FAILED\n");
		std::cout << "Write failed: " << error.message() << "\n";
	}
}
