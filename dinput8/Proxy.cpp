// Proxy.cpp : Defines the Proxy class, which handles the proxy operations.
#include "pch.hpp"
#include "Proxy.hpp"

#include "ProxyTCP.hpp"
#include "WebSocketClient.hpp"

using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ssl;


Proxy::Proxy()
{
	BOOST_LOG_FUNCTION()

	const auto config = &Config::GetInstance();

	// Infinite loop to handle exceptions and restart the proxy
	while (true)
	{
		try
		{
			// Create an io_service object for asynchronous I/O
			io_service ioService;

			USHORT plasmaPort;

			if (config->executableType == CLIENT)
			{
				plasmaPort = config->hook->plasmaClientPort;
			}
			else if (config->executableType == SERVER)
			{
				plasmaPort = config->hook->plasmaServerPort;
			}
			else
			{
				BOOST_LOG_TRIVIAL(fatal) << "Cannot start proxy! Invalid executable type!";
				MessageBoxA(nullptr, "Invalid executable type!", "Failed to initialize proxy!", MB_ICONERROR);
				ExitProcess(PROXY_STARTUP_FAILURE);
			}

			BOOST_LOG_TRIVIAL(info) << "Initializing...";

			// Create a context for the TLSv1.2 client and a shared pointer to a WebSocketClient
			context ctx(context::tlsv12_client);
			const auto ws = std::make_shared<WebSocketClient>(ioService, ctx, config->hook->proxySSL);

			// Create a new ProxyTCP for the plasma port
			auto plasmaProxy = new ProxyTCP(ioService, plasmaPort, true, ws);

			BOOST_LOG_TRIVIAL(info) << "Finished initialization, ready for receiving incoming connections!";
			ioService.run(); // Start the io_service
		}
		catch (std::exception& e)
		{
			// If an exception occurs, log a fatal error and an info message about restarting the proxy
			BOOST_LOG_TRIVIAL(fatal) << "An unhandled exception has occurred: " << e.what();
			BOOST_LOG_TRIVIAL(info) << "Restarting proxy...";
		}
	}
}
