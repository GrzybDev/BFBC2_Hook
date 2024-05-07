// WebSocketClient.cpp : Defines the WebSocketClient class, which handles the WebSocket client operations.
#include "pch.hpp"
#include "WebSocketClient.hpp"

WebSocketClient::WebSocketClient(net::io_context& ioc, ssl::context& ctx, const bool isSecure) : resolver_(ioc),
	ws_(ioc), wss_(ioc, ctx)
{
	isSecure_ = isSecure;
}

void WebSocketClient::SetPlasmaCallback(
	const std::function<void(boost::array<char, PACKET_MAX_LENGTH>, size_t)>& callback)
{
	plasmaWrite_ = callback;
}

void WebSocketClient::Connect(const std::string& host, const USHORT& port, const std::string& target)
{
	std::string fullAddress = (isSecure_ ? "wss://" : "ws://") + host + ":" + std::to_string(port) + target;
	BOOST_LOG_TRIVIAL(info) << "Connecting to master server (at " << fullAddress << ")...";

	// Update the host_ string. This will provide the value of the
	// Host HTTP header during the WebSocket handshake.
	// See https://tools.ietf.org/html/rfc7230#section-5.4
	auto host_ = host;
	auto userAgent = "Battlefield: Bad Company 2 Hook (" + std::string(BOOST_BEAST_VERSION_STRING) + ")";

	// Look up the domain name
	const auto results = resolver_.resolve(host, std::to_string(port));

	if (isSecure_)
	{
		// Make the connection on the IP address we get from a lookup
		auto ep = net::connect(get_lowest_layer(wss_), results);

		// Set SNI Hostname (many hosts need this to handshake successfully)
		if (!SSL_set_tlsext_host_name(wss_.next_layer().native_handle(), host.c_str()))
			throw beast::system_error(
				beast::error_code(static_cast<int>(ERR_get_error()), net::error::get_ssl_category()),
				"Failed to set SNI Hostname");

		// Update the host_ string. This will provide the value of the
		// Host HTTP header during the WebSocket handshake.
		// See https://tools.ietf.org/html/rfc7230#section-5.4
		host_ += ':' + std::to_string(ep.port());

		// Perform the SSL handshake
		wss_.next_layer().handshake(ssl::stream_base::client);

		// Set a decorator to change the User-Agent of the handshake
		wss_.set_option(websocket::stream_base::decorator(
			[userAgent](websocket::request_type& req)
			{
				req.set(http::field::user_agent,
				        userAgent);
			}));

		// Perform the websocket handshake
		wss_.handshake(host, target);
	}
	else
	{
		// Make the connection on the IP address we get from a lookup
		auto ep = net::connect(ws_.next_layer(), results);

		// Update the host_ string. This will provide the value of the
		// Host HTTP header during the WebSocket handshake.
		// See https://tools.ietf.org/html/rfc7230#section-5.4
		host_ += ':' + std::to_string(ep.port());

		// Set a decorator to change the User-Agent of the handshake
		ws_.set_option(websocket::stream_base::decorator(
			[userAgent](websocket::request_type& req)
			{
				req.set(http::field::user_agent,
				        userAgent);
			}));

		// Perform the websocket handshake
		ws_.handshake(host, target);
	}

	BOOST_LOG_TRIVIAL(info) << "Connected to Master Server!";

	if (isSecure_)
	{
		wss_.binary(true);

		// Read a message into our buffer
		wss_.async_read(
			buffer_,
			beast::bind_front_handler(
				&WebSocketClient::OnRead,
				shared_from_this()));
	}
	else
	{
		ws_.binary(true);

		// Read a message into our buffer
		ws_.async_read(
			buffer_,
			beast::bind_front_handler(
				&WebSocketClient::OnRead,
				shared_from_this()));
	}
}

void WebSocketClient::OnRead(const beast::error_code& ec, std::size_t bytesTransferred)
{
	if (ec)
	{
		BOOST_LOG_TRIVIAL(error) << "Failed to read: " << ec.message();
		throw std::runtime_error("Failed to read: " + ec.message());
	}

	boost::array<char, PACKET_MAX_LENGTH> readBuf;
	std::memcpy(readBuf.data(), buffer_.data().data(), bytesTransferred);
	plasmaWrite_(readBuf, bytesTransferred);

	if (isSecure_)
	{
		// Read a message into our buffer
		wss_.async_read(
			buffer_,
			beast::bind_front_handler(
				&WebSocketClient::OnRead,
				shared_from_this()));
	}
	else
	{
		// Read a message into our buffer
		ws_.async_read(
			buffer_,
			beast::bind_front_handler(
				&WebSocketClient::OnRead,
				shared_from_this()));
	}
}

void WebSocketClient::Write(boost::array<char, PACKET_MAX_LENGTH> data, const size_t size)
{
	if (isSecure_)
		wss_.write(boost::asio::buffer(data, size));
	else
		ws_.write(boost::asio::buffer(data, size));
}

void WebSocketClient::Disconnect()
{
	if (isSecure_)
		wss_.close(websocket::close_code::normal);
	else
		ws_.close(websocket::close_code::normal);
}
