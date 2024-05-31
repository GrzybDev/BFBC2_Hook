#include "pch.hpp"
#include "ConnectionTheater.hpp"

using namespace boost::asio;
using namespace ip;

ConnectionTheater::ConnectionTheater(io_service& ioService,
                                     const std::shared_ptr<WebSocketClient>& ws) : gameSocket_(ioService), buffer_()
{
	BOOST_LOG_FUNCTION()

	ws_ = ws;
}

ConnectionTheater::~ConnectionTheater()
= default;

ip::tcp::socket& ConnectionTheater::GameSocket()
{
	return gameSocket_;
}

void ConnectionTheater::Start()
{
	ws_->SetTheaterCallback([this](const boost::array<char, PACKET_MAX_LENGTH>& data, const size_t length)
	{
		SendToGame(data, length);
	});

	gameSocket_.async_read_some(buffer(buffer_, USHRT_MAX),
	                            bind(&ConnectionTheater::HandleRead, shared_from_this(),
	                                 placeholders::error,
	                                 placeholders::bytes_transferred));
}

void ConnectionTheater::HandleRead(const boost::system::error_code& error, const size_t bytesTransferred)
{
	BOOST_LOG_NAMED_SCOPE("Theater (Read)")

	if (!error)
	{
		const Packet packet(buffer_, bytesTransferred);
		ws_->Write(buffer_, bytesTransferred);

		BOOST_LOG_TRIVIAL(debug) << boost::format("[GAME] -> [SERVER] %s 0x%08x (%i bytes) {%s}")
				% packet.service % packet.kind % packet.length % packet.data;

		// Clear the buffer
		buffer_.fill(0);

		gameSocket_.async_read_some(buffer(buffer_, PACKET_MAX_LENGTH),
		                            boost::bind(&ConnectionTheater::HandleRead, shared_from_this(),
		                                        placeholders::error,
		                                        placeholders::bytes_transferred));
	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "Error message: " << error.message() << ", error code: " << error.value();
		HandleStop();
	}
}

void ConnectionTheater::SendToGame(boost::array<char, PACKET_MAX_LENGTH> data, const size_t length)
{
	BOOST_LOG_NAMED_SCOPE("Plasma (Write)")

	const Packet packet(data, length);
	write(gameSocket_, buffer(data, length));

	BOOST_LOG_TRIVIAL(debug) << boost::format("[GAME] <- [SERVER] %s 0x%08x (%i bytes) {%s}")
		% packet.service % packet.kind % packet.length % packet.data;
}

void ConnectionTheater::HandleStop() const
{
	BOOST_LOG_NAMED_SCOPE("Plasma (Stop)")

	ws_->Disconnect();

	BOOST_LOG_TRIVIAL(info) << "Client Disconnected!";
}
