#include "pch.hpp"
#include "Config.hpp"

Config::Config()
{
	readConfig();
}

void Config::readConfig() const
{
	// Create an empty property tree object
	using boost::property_tree::ptree;
	ptree pt;

	std::string configPath = "config.json";

	try
	{
		read_json(configPath, pt);
	}
	catch (boost::property_tree::json_parser_error& e)
	{
		// Will use defaults in case of parse error

		if (boost::filesystem::exists(configPath))
		{
			// Print parse error only if it's actual parse error
			auto temp = std::string(e.what());
			auto errMessage = std::wstring(temp.begin(), temp.end());
			std::wstring message = L"Hook will use default configs because error occured while parsing config file: ";
			message += errMessage;

			MessageBox(nullptr, message.c_str(), L"Config Parse Error", MB_ICONEXCLAMATION | MB_OK);
		}
	}

	parseConfig(pt);
}

void Config::parseConfig(boost::property_tree::ptree pt) const
{
	// Section - Client
	hook->forceClientType = pt.get("client.forceClientType", "");
	hook->clientPlasmaPort = pt.get("client.clientPlasmaPort", CLIENT_PLASMA_PORT);
	hook->clientTheaterPort = pt.get("client.clientTheaterPort", CLIENT_THEATER_PORT);
	hook->serverPlasmaPort = pt.get("client.serverPlasmaPort", SERVER_PLASMA_PORT);
	hook->serverTheaterPort = pt.get("client.serverTheaterPort", SERVER_THEATER_PORT);

	// Section - Debug
	hook->showConsole = pt.get("debug.showConsole", true);
	hook->createLog = pt.get("debug.createLog", false);

	boost::posix_time::ptime timeLocal = boost::posix_time::second_clock::local_time();
	auto facet = new boost::posix_time::time_facet("%Y-%m-%d_%H-%M-%S");

	std::ostringstream is;
	is.imbue(std::locale(is.getloc(), facet));
	is << timeLocal;

	hook->logPath = pt.get("debug.logPath", (boost::format("bfbc2_%1%.log") % is.str()).str());
	hook->consoleLogLevel = pt.get("debug.logLevelConsole", static_cast<int>(boost::log::trivial::info));
	hook->fileLogLevel = pt.get("debug.logLevelFile", static_cast<int>(boost::log::trivial::debug));

	// Section - Patches
	hook->verifyGameVersion = pt.get("patches.verifyGameVersion", true);
	hook->patchDNS = pt.get("patches.patchDNS", true);
	hook->patchSSL = pt.get("patches.patchSSL", true);

	// Section - Proxy
	hook->proxyEnable = pt.get("proxy.enable", true);
	hook->connectRetail = pt.get("proxy.connectToRetail", false);
	hook->serverAddress = pt.get("proxy.serverAddress", "bfbc2.grzyb.app");
	hook->serverPort = pt.get("proxy.serverPort", 443);
	hook->serverSecure = pt.get("proxy.serverSecure", true);

	// Section - Overrides
	hook->clientVersion = pt.get("overrides.clientVersion", "\"ROMEPC795745\"");
	hook->serverVersion = pt.get("overrides.serverVersion", "\"ROMEPC851434\"");
	hook->sslPatchRetryCount = pt.get("overrides.sslPatchRetryCount", 3);
}
