#include "pch.hpp"
#include "ConnectionTheater.hpp"
#include "Config.hpp"

using namespace boost;
using namespace asio;
using ip::tcp;
using namespace web;
using namespace websockets::client;

ConnectionTheater::ConnectionTheater(io_service& io_service, ssl::context& context) : game_socket_(io_service)
{
	BOOST_LOG_FUNCTION();

	retailCtx = new ConnectionRetail(THEATER,
	                                 [=](unsigned char* data, int length) { sendToGame(data, length); },
	                                 [=]() { handle_stop(false); },
	                                 io_service, context);

	wsCtx = new ConnectionWebSocket(THEATER,
	                                [=](unsigned char* data, int length) { sendToGame(data, length); },
	                                [=]() { handle_stop(false); });
}

ConnectionTheater::~ConnectionTheater()
{
	delete retailCtx;
	delete wsCtx;
}

tcp::socket& ConnectionTheater::gameSocket()
{
	return game_socket_;
}

void ConnectionTheater::start()
{
	std::fill_n(receive_buffer, USHRT_MAX, 0);
	game_socket_.async_read_some(buffer(receive_buffer, USHRT_MAX),
	                             bind(&ConnectionTheater::handle_read, shared_from_this(), asio::placeholders::error,
	                                  asio::placeholders::bytes_transferred));
}

void ConnectionTheater::handle_read(const system::error_code& error, size_t bytes_transferred)
{
	if (!error)
	{
		receive_length += bytes_transferred;

		Config* config = &Config::getInstance();

		if (!connected)
		{
			if (config->hook->connectRetail)
				retailCtx->connectToRetail();
			else
				wsCtx->connect();

			connected = true;
		}

		unsigned int current_offset = 0;

		while (true)
		{
			unsigned int length = length = Utils::DecodeInt(receive_buffer + current_offset + LENGTH_OFFSET,
				HEADER_VALUE_LENGTH);

			if (length == 0)
				break;

			auto packet = new Packet(receive_buffer + current_offset, length);

			if (!packet->isValid)
			{
				// Invalid packet, check if packet is *just* incomplete (so we should wait for more data)

				if (packet->isIncomplete)
				{
					BOOST_LOG_TRIVIAL(debug) << "Incomplete packet, receiving next chunk...";
					game_socket_.read_some(buffer(receive_buffer + current_offset + packet->realLength, USHRT_MAX - current_offset - packet->realLength));
					std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // It just... makes this thing work?

					continue;
				}

				// Invalid packet
				break;
			}

			current_offset += length;
			BOOST_LOG_TRIVIAL(debug) << format("[PROXY] <- [GAME (Theater)] %s 0x%08x (%i bytes) {%s}") % packet->service % packet->kind % packet->length % packet->data;

			if (config->hook->connectRetail)
				retailCtx->sendToRetail(receive_buffer + current_offset - length, length);
			else
				wsCtx->send(receive_buffer + current_offset - length, length);
		}

		receive_length = 0;
		std::fill_n(receive_buffer, USHRT_MAX, 0);

		game_socket_.async_read_some(buffer(receive_buffer, USHRT_MAX),
		                             bind(&ConnectionTheater::handle_read, shared_from_this(),
		                                  asio::placeholders::error, asio::placeholders::bytes_transferred));
	}
}

void ConnectionTheater::sendToGame(unsigned char* data, int length)
{
	Packet packet(data, length);

	if (!packet.isValid)
	{
		BOOST_LOG_TRIVIAL(error) << "Tried to send invalid package! Skipping...";
		return;
	}

	try
	{
		write(game_socket_, buffer(data, length));
		BOOST_LOG_TRIVIAL(debug) << format("[PROXY] -> [GAME (Theater)] %s 0x%08x (%i bytes) {%s}") % packet.service % packet.kind % packet.length % packet.data;
	}
	catch (std::exception &e)
	{
		BOOST_LOG_TRIVIAL(debug) << "[Theater] write() failed: " << e.what();
	}
}

void ConnectionTheater::handle_stop(bool crash)
{
	if (wsCtx->connected)
		wsCtx->close();

	if (retailCtx->connected)
		retailCtx->close();

	if (connected)
	{
		game_socket_.lowest_layer().close();
		connected = false;
	}

	BOOST_LOG_TRIVIAL(info) << "Client Disconnected!";

	if (crash)
	{
		// Throw an exception to restart proxy
		throw ProxyStopException();
	}
}
