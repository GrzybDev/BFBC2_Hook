#include "pch.hpp"
#include "Proxy.hpp"

#include "Hook.hpp"
#include "ProxyClient.hpp"
#include "ProxyTCP.hpp"

DWORD WINAPI ProxyInit(LPVOID /*lpParameter*/)
{
	Proxy::getInstance();
	return TRUE;
}

Proxy::Proxy()
{
	BOOST_LOG_FUNCTION();
	using namespace boost::asio;

	Config* config = &Config::getInstance();
	Hook* hook = &Hook::getInstance();

	while (true)
	{
		try
		{
			io_service io_service;

			USHORT feslPort;
			std::wstring feslPath;
			USHORT theaterPort;
			std::wstring theaterPath;

			if (hook->exeType == CLIENT)
			{
				feslPort = CLIENT_FESL_PORT;
				feslPath = L"/fesl/client";

				theaterPort = CLIENT_THEATER_PORT;
				theaterPath = L"/theater/client";
			}
			else if (hook->exeType == SERVER)
			{
				feslPort = SERVER_FESL_PORT;
				feslPath = L"/fesl/server";

				theaterPort = SERVER_THEATER_PORT;
				theaterPath = L"/theater/server";
			}
			else
				return;

			BOOST_LOG_TRIVIAL(info) << "Initializing...";
			ProxyTCP* feslProxy = new ProxyTCP(io_service, feslPort, true, feslPath);

			if (config->hook->connectRetail)
			{
				ProxyClient* proxyClient = &ProxyClient::getInstance();
				proxyClient->feslResolver = new ip::tcp::resolver(io_service);
			}

			BOOST_LOG_TRIVIAL(info) << "Finished initialization, ready for receiving incoming connections!";

			io_service.run();
		}
		catch (std::exception& e)
		{
			BOOST_LOG_TRIVIAL(error) << "An unhandled exception has occurred: " << e.what();
			BOOST_LOG_TRIVIAL(info) << "Restarting proxy...";

			continue;
		}

		break;
	}
}
