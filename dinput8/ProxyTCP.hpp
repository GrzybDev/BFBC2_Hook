// ProxyTCP.hpp : Defines the ProxyTCP class, which handles the TCP proxy operations.

#pragma once
#include "ConnectionPlasma.hpp"
#include "WebSocketClient.hpp"

using namespace boost;
using namespace asio;
using ip::tcp;

class ProxyTCP : asio::noncopyable
{
public:
	ProxyTCP(io_service& ioService, USHORT port, bool secure, const std::shared_ptr<WebSocketClient>& ws);

private:
	ip::tcp::acceptor acceptor_;
	asio::ssl::context context_;

	int port_;
	bool secure_;
	std::shared_ptr<WebSocketClient> ws_;

	void StartAccept();

	ConnectionPlasma::pointer newPlasmaConnection_;

	void HandleAcceptPlasma(const system::error_code& error);
};
