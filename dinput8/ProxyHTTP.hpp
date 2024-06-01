#pragma once
#include "HttpClient.hpp"
#include "HttpHandler.hpp"

class ProxyHTTP : boost::asio::noncopyable
{
public:
	ProxyHTTP(boost::asio::io_service& ioService, USHORT port, const std::shared_ptr<HttpClient>& http);

private:
	boost::asio::ip::tcp::acceptor acceptor_;
	int port_;
	std::shared_ptr<HttpClient> http_;

	HttpHandler::pointer new_http_connection_;

	void StartAccept();
	void HandleAccept(const boost::system::error_code& error);
};
