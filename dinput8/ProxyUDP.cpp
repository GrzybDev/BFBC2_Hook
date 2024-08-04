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

	ws->SetTheaterUDPCallback([this](const boost::array<char, PACKET_MAX_LENGTH>& data, const size_t length)
	{
		SendToGame(data, length);
	});

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
	BOOST_LOG_NAMED_SCOPE("Theater (UDP (Read))")

	if (!error && bytesTransferred > 0)
	{
		std::string remoteMeta = "META|" + remote_endpoint.address().to_string() + "|" + std::to_string(remote_endpoint.port());
		ws_->WriteText(remoteMeta);

		const Packet packet(buffer_, bytesTransferred);
		ws_->Write(buffer_, bytesTransferred);

		BOOST_LOG_TRIVIAL(debug) << format("[GAME] -> [SERVER) %s 0x%08x (%i bytes) {%s}") % packet.
			service % packet.kind % packet.length % packet.data;

		// Clear the buffer
		buffer_.fill(0);

		StartReceive();
	}
}

void ProxyUDP::HandleSend(const system::error_code& error, size_t bytesTransferred)
{
}

void ProxyUDP::SendToGame(boost::array<char, PACKET_MAX_LENGTH> data, size_t length)
{
	BOOST_LOG_NAMED_SCOPE("Theater (UDP (Write))")

	const Packet packet(data, length);
	socket.async_send_to(buffer(data, length), remote_endpoint,
	                     boost::bind(&ProxyUDP::HandleSend, this, asio::placeholders::error,
	                                 asio::placeholders::bytes_transferred));

	BOOST_LOG_TRIVIAL(debug) << format("[GAME] <- [SERVER) %s 0x%08x (%i bytes) {%s}") % packet.
		service % packet.kind % packet.length % packet.data;
}
