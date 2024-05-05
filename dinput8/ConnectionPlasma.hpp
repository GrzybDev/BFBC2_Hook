#pragma once
#include "pch.hpp"

#include "WebSocketClient.hpp"

using socket_ssl = ssl::stream<boost::asio::ip::tcp::socket>;

class ConnectionPlasma : public boost::enable_shared_from_this<ConnectionPlasma>, boost::asio::noncopyable
{
public:
	ConnectionPlasma(boost::asio::io_service& ioService, ssl::context& context, std::shared_ptr<WebSocketClient> ws);
	~ConnectionPlasma();

	using pointer = boost::shared_ptr<ConnectionPlasma>;

	socket_ssl::lowest_layer_type& GameSocket();

	void Start();

private:
	socket_ssl gameSocket_;
	std::shared_ptr<WebSocketClient> ws_;

	void HandleHandshake(const boost::system::error_code& error); // first sequence of packets for ssl connection
	void HandleRead(const boost::system::error_code& error, size_t bytesTransferred); // normal read
	void HandleStop() const; // cleanup after a disconnect

	void SendToGame(boost::array<char, PACKET_MAX_LENGTH> data, size_t length);

	boost::array<char, PACKET_MAX_LENGTH> buffer_;
};
