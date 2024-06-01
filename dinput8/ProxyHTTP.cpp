#include "pch.hpp"
#include "ProxyHTTP.hpp"

using namespace boost;
using namespace asio;
using ip::tcp;

ProxyHTTP::ProxyHTTP(io_service& ioService, const USHORT port, const std::shared_ptr<HttpClient>& http):
	acceptor_(ioService, tcp::endpoint(tcp::v4(), port))
{
	BOOST_LOG_FUNCTION();

	port_ = port;
	http_ = http;

	StartAccept();
	BOOST_LOG_TRIVIAL(info) << "Created HTTP Proxy (listening on port " << port << ")...";
}

void ProxyHTTP::StartAccept()
{
	new_http_connection_.reset(new HttpHandler(static_cast<io_context&>(acceptor_.get_executor().context()), http_));
	acceptor_.async_accept(new_http_connection_->GameSocket(),
	                       bind(&ProxyHTTP::HandleAccept, this, asio::placeholders::error));
}

void ProxyHTTP::HandleAccept(const system::error_code& error)
{
	BOOST_LOG_NAMED_SCOPE("HTTP")

	// If the acceptor is not open, log an error and return
	if (!acceptor_.is_open())
	{
		BOOST_LOG_TRIVIAL(error) << "Acceptor is not open!";
		return;
	}

	if (!error)
		new_http_connection_->Start();
	else
		BOOST_LOG_TRIVIAL(error) << "Error accepting connection: " << error.message();

	StartAccept();
}
