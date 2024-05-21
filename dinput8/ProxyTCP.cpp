// ProxyTCP.cpp : Defines the ProxyTCP class, which handles the TCP proxy operations.
#include "pch.hpp"
#include "ProxyTCP.hpp"

#include "ProxyCert.hpp"

using namespace boost;
using namespace asio;
using ip::tcp;

ProxyTCP::ProxyTCP(io_service& ioService, const USHORT port, const bool secure,
                   const std::shared_ptr<WebSocketClient>& ws) :
	acceptor_(ioService, tcp::endpoint(tcp::v4(), port)), context_(asio::ssl::context::sslv23)
{
	BOOST_LOG_FUNCTION()

	port_ = port;
	secure_ = secure;
	ws_ = ws;

	// If secure, set up SSL context
	if (secure)
	{
		SSL_CTX_set_cipher_list(context_.native_handle(), "ALL:!DH");
		SSL_CTX_set_options(context_.native_handle(), SSL_OP_ALL);

		SSL_CTX_use_certificate_ASN1(context_.native_handle(), sizeof(proxy_certificate), proxy_certificate);
		SSL_CTX_use_PrivateKey_ASN1(EVP_PKEY_RSA, context_.native_handle(), proxy_private_key,
		                            sizeof(proxy_private_key));
		SSL_CTX_set_verify_depth(context_.native_handle(), 1);
	}

	StartAccept();
	BOOST_LOG_TRIVIAL(info) << "Created TCP Proxy (listening on port " << port << ")...";
}

void ProxyTCP::StartAccept()
{
	if (secure_)
	{
		newPlasmaConnection_.reset(new ConnectionPlasma(static_cast<io_context&>(acceptor_.get_executor().context()),
		                                                context_, ws_));
		acceptor_.async_accept(newPlasmaConnection_->GameSocket(),
		                       bind(&ProxyTCP::HandleAcceptPlasma, this, asio::placeholders::error));
	}
	else
	{
		newTheaterConnection_.reset(
			new ConnectionTheater(static_cast<io_context&>(acceptor_.get_executor().context()), ws_));
		acceptor_.async_accept(newTheaterConnection_->GameSocket(),
		                       boost::bind(&ProxyTCP::HandleAcceptTheater, this, asio::placeholders::error));
	}
}

void ProxyTCP::HandleAcceptPlasma(const system::error_code& error)
{
	BOOST_LOG_NAMED_SCOPE("Plasma")

	// If the acceptor is not open, log an error and return
	if (!acceptor_.is_open())
	{
		BOOST_LOG_TRIVIAL(error) << "TCP Socket, port " << port_ << " - acceptor is not open";
		return;
	}

	if (!error)
		newPlasmaConnection_->Start();
	else
		BOOST_LOG_TRIVIAL(error) << "TCP Socket, port " << port_ << " - error: " << error.message() << ", error code: "
			<< error.value();

	StartAccept();
}

void ProxyTCP::HandleAcceptTheater(const system::error_code& error)
{
	BOOST_LOG_NAMED_SCOPE("Theater")

	if (!acceptor_.is_open())
	{
		BOOST_LOG_TRIVIAL(error) << "TCP Socket, port " << port_ << " - acceptor is not open";
		return;
	}

	if (!error)
		newTheaterConnection_->Start();
	else
		BOOST_LOG_TRIVIAL(error) << "TCP Socket, port " << port_ << " - error: " << error.message() << ", error code: "
			<< error.value();

	StartAccept();
}
