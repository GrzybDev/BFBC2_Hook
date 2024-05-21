#pragma once
#include "WebSocketClient.hpp"

class ProxyUDP
{
public:
	ProxyUDP(boost::asio::io_service& ioService, USHORT port, const std::shared_ptr<WebSocketClient>& ws);

	boost::asio::ip::udp::socket socket;
	boost::asio::ip::udp::endpoint remote_endpoint;

private:
	USHORT port_;

	boost::array<char, PACKET_MAX_LENGTH> buffer_;
	std::shared_ptr<WebSocketClient> ws_;

	void StartReceive();

	void HandleReceive(const boost::system::error_code& error, size_t bytesTransferred);
	void HandleSend(const boost::system::error_code& error, size_t bytesTransferred);
};
