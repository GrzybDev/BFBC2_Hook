// ConnectionPlasma.cpp : Defines the methods for the ConnectionPlasma class, which handles the connection to the game.
#include "pch.hpp"
#include "ConnectionPlasma.hpp"

#include "Packet.hpp"


using namespace boost::asio;
using namespace ip;

ConnectionPlasma::ConnectionPlasma(io_service& ioService, boost::asio::ssl::context& context,
                                   std::shared_ptr<WebSocketClient> ws) : gameSocket_(ioService, context), buffer_()
{
	BOOST_LOG_FUNCTION()

	ws_ = ws;
}

ConnectionPlasma::~ConnectionPlasma()
= default;


socket_ssl::lowest_layer_type& ConnectionPlasma::GameSocket()
{
	return gameSocket_.lowest_layer();
}

void ConnectionPlasma::Start()
{
	// Before reading stuff we have to do a handshake
	gameSocket_.async_handshake(boost::asio::ssl::stream_base::server,
	                            bind(&ConnectionPlasma::HandleHandshake, shared_from_this(), placeholders::error));
}

void ConnectionPlasma::HandleHandshake(const boost::system::error_code& error)
{
	BOOST_LOG_NAMED_SCOPE("Plasma (Handshake)")

	if (!error)
	{
		BOOST_LOG_TRIVIAL(debug) << "New connection, initializing...";

		const auto config = &Config::GetInstance();
		ws_->Connect(config->hook->proxyHost, config->hook->proxyPort, config->hook->proxyPath);

		gameSocket_.async_read_some(buffer(buffer_, PACKET_MAX_LENGTH),
		                            boost::bind(&ConnectionPlasma::HandleRead, shared_from_this(),
		                                        placeholders::error, placeholders::bytes_transferred));

		ws_->SetPlasmaCallback([this](const boost::array<char, PACKET_MAX_LENGTH>& data, const size_t length)
		{
			SendToGame(data, length);
		});

		BOOST_LOG_TRIVIAL(info) << "Client connected";
	}
	else
	{
		BOOST_LOG_TRIVIAL(info) << "Disconnected, error message: " << error.message() << ", error code: "
			<< error.value();
		HandleStop();
	}
}

void ConnectionPlasma::HandleRead(const boost::system::error_code& error, const size_t bytesTransferred)
{
	BOOST_LOG_NAMED_SCOPE("Plasma (Read)")

	if (!error)
	{
		const Packet packet(buffer_, bytesTransferred);
		ws_->Write(buffer_, bytesTransferred);

		BOOST_LOG_TRIVIAL(debug) << boost::format("[GAME] -> [SERVER] %s 0x%08x (%i bytes) {%s}")
			% packet.service % packet.kind % packet.length % packet.data;

		// Clear the buffer
		buffer_.fill(0);

		gameSocket_.async_read_some(buffer(buffer_, PACKET_MAX_LENGTH),
		                            boost::bind(&ConnectionPlasma::HandleRead, shared_from_this(),
		                                        placeholders::error, placeholders::bytes_transferred));
	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "Error message: " << error.message() << ", error code: " << error.value();
		HandleStop();
	}
}

void ConnectionPlasma::SendToGame(boost::array<char, PACKET_MAX_LENGTH> data, size_t length)
{
	BOOST_LOG_NAMED_SCOPE("Plasma (Write)")

	const Packet packet(data, length);
	write(gameSocket_, buffer(data, length));

	BOOST_LOG_TRIVIAL(debug) << boost::format("[GAME] <- [SERVER] %s 0x%08x (%i bytes) {%s}")
		% packet.service % packet.kind % packet.length % packet.data;
}


void ConnectionPlasma::HandleStop() const
{
	BOOST_LOG_NAMED_SCOPE("Plasma (Stop)")

	ws_->Disconnect();

	BOOST_LOG_TRIVIAL(info) << "Client Disconnected!";
}
