#include "pch.hpp"
#include "ProxyUDP.hpp"

using namespace boost;
using namespace asio;
using ip::udp;

ProxyUDP::ProxyUDP(io_service& ioService, const USHORT port, const std::shared_ptr<WebSocketClient>& ws) : socket(
	ioService, udp::endpoint(udp::v4(), port))
{
	BOOST_LOG_FUNCTION()

	this->port_ = port;
	this->ws_ = ws;

	StartReceive();

	BOOST_LOG_TRIVIAL(info) << "Created UDP Proxy (listening on port " << port << ")...";
}

void ProxyUDP::StartReceive()
{
	socket.async_receive_from(buffer(buffer_), remote_endpoint,
	                          bind(&ProxyUDP::HandleReceive, this, asio::placeholders::error,
	                               asio::placeholders::bytes_transferred));
}

void ProxyUDP::HandleReceive(const system::error_code& error, size_t bytesTransferred)
{
	if (!error && bytesTransferred > 0)
	{
		const Packet packet(buffer_, bytesTransferred);
		ws_->Write(buffer_, bytesTransferred);

		BOOST_LOG_TRIVIAL(debug) << format("[UDP] [GAME (Theater)] -> [SERVER)] %s 0x%08x (%i bytes) {%s}") % packet.
service % packet.kind % packet.length % packet.data;

		// Clear the buffer
		buffer_.fill(0);

		StartReceive();
	}
}
