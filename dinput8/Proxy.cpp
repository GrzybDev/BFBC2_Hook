#include "pch.hpp"
#include "Proxy.hpp"

#include "WebSocketClient.hpp"

using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ssl;


Proxy::Proxy()
{
	BOOST_LOG_FUNCTION()

	const auto config = &Config::GetInstance();

	while (true)
	{
		try
		{
			io_service ioService;

			BOOST_LOG_TRIVIAL(info) << "Initializing...";

			context ctx(context::tlsv12_client);

			const auto ws = std::make_shared<WebSocketClient>(ioService, ctx, config->hook->proxySSL);

			BOOST_LOG_TRIVIAL(info) << "Finished initialization, ready for receiving incoming connections!";
			ioService.run();
		}
		catch (std::exception& e)
		{
			BOOST_LOG_TRIVIAL(fatal) << "An unhandled exception has occurred: " << e.what();
			BOOST_LOG_TRIVIAL(info) << "Restarting proxy...";
		}
	}
}
