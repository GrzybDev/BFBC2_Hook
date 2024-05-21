#pragma once
#include "WebSocketClient.hpp"

class ConnectionTheater : public boost::enable_shared_from_this<ConnectionTheater>, boost::noncopyable
{
public:
	ConnectionTheater(boost::asio::io_service& ioService, const std::shared_ptr<WebSocketClient>& ws);
	~ConnectionTheater();

	using pointer = boost::shared_ptr<ConnectionTheater>;
	boost::asio::ip::tcp::socket& GameSocket();

	void Start();

private:
	boost::asio::ip::tcp::socket gameSocket_;
	std::shared_ptr<WebSocketClient> ws_;

	boost::array<char, PACKET_MAX_LENGTH> buffer_;

	bool connected = false;

	void HandleRead(const boost::system::error_code& error, size_t bytesTransferred);
	// normal read (also deals with multiple packets)
	void HandleStop() const; // cleanup after a disconnect

	void SendToGame(boost::array<char, PACKET_MAX_LENGTH> data, size_t length);
};
