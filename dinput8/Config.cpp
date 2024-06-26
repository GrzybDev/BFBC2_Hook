// Config.cpp : Defines the configuration settings for the application.
#include "pch.hpp"

Config::Config()
{
	// Define the path to the configuration file
	const auto configPath = "config.json";
	ptree pt;

	try
	{
		// Try to read the configuration file
		read_json(configPath, pt);
	}
	catch (boost::property_tree::json_parser_error& e)
	{
		// If there is a parse error, use the default configuration

		if (boost::filesystem::exists(configPath))
		{
			// If the configuration file exists, print the parse error
			auto temp = std::string(e.what());
			const auto errMessage = std::wstring(temp.begin(), temp.end());
			const auto message = L"Hook will use default configs because error occured while parsing config file: " +
				errMessage;

			MessageBox(nullptr, message.c_str(), L"Config Parse Error",MB_ICONEXCLAMATION | MB_OK);
		}
	}

	// Section - Client
	hook->forceClientType = pt.get("client.forceClientType", "");
	hook->plasmaClientPort = pt.get("client.plasmaClientPort", CLIENT_PLASMA_PORT);
	hook->plasmaServerPort = pt.get("client.plasmaServerPort", SERVER_PLASMA_PORT);
	hook->theaterClientPort = pt.get("client.theaterClientPort", CLIENT_THEATER_PORT);
	hook->theaterServerPort = pt.get("client.theaterServerPort", SERVER_THEATER_PORT);
	hook->blockUnknownDomains = pt.get("client.blockUnknownDomains", true);

	std::list<std::string> allowedDomains;
	const auto domains = pt.get_child_optional("client.allowedDomains");

	if (domains)
	{
		// If there are allowed domains in the configuration file, read them
		BOOST_FOREACH(boost::property_tree::ptree::value_type & field, pt.get_child("client.allowedDomains"))
			allowedDomains.push_back(field.second.data());
	}
	else
	{
		// If there are no allowed domains in the configuration file, use the default domains
		allowedDomains.emplace_back("bfbc2-pc.fesl.ea.com"); // FESL Client
		allowedDomains.emplace_back("bfbc2-pc.theater.ea.com"); // Theater Client
		allowedDomains.emplace_back("bfbc2-pc-server.fesl.ea.com"); // FESL Server
		allowedDomains.emplace_back("bfbc2-pc-server.theater.ea.com"); // Theater Server
		allowedDomains.emplace_back("easo.ea.com"); // EASO
		allowedDomains.emplace_back("bfbc2.gos.ea.com"); // GOS
	}

	hook->allowedDomains = allowedDomains;

	// Section - Debug
	hook->showConsole = pt.get("debug.showConsole", false);
	hook->createLog = pt.get("debug.createLog", false);

	const boost::posix_time::ptime timeLocal = boost::posix_time::second_clock::local_time();
	const auto facet = new boost::posix_time::time_facet("%Y-%m-%d_%H-%M-%S");

	std::ostringstream is;
	is.imbue(std::locale(is.getloc(), facet));
	is << timeLocal;

	hook->logPath = pt.get("debug.logPath", (boost::format("bfbc2_%1%.log") % is.str()).str());
	hook->consoleLogLevel = pt.get("debug.logLevelConsole", static_cast<int>(boost::log::trivial::info));
	hook->fileLogLevel = pt.get("debug.logLevelFile", static_cast<int>(boost::log::trivial::debug));

	// Section - Patches
	hook->verifyGameVersion = pt.get("patches.verifyGameVersion", true);
	hook->patchDNS = pt.get("patches.DNS", true);
	hook->patchSSL = pt.get("patches.SSL", true);

	// Section - Proxy
	hook->proxyEnable = pt.get("proxy.enable", true);
	hook->proxyHost = pt.get("proxy.host", "bfbc2.grzyb.app");
	hook->proxyPort = pt.get("proxy.port", HTTPS_PORT);
	hook->proxyPath = pt.get("proxy.path", "/ws");
	hook->proxySSL = pt.get("proxy.secure", true);

	// Section - Overrides
	hook->clientVersion = pt.get("overrides.clientVersion", "\"ROMEPC795745\"");
	hook->serverVersion = pt.get("overrides.serverVersion", "\"ROMEPC851434\"");
	hook->sslPatchRetryCount = pt.get("overrides.sslPatchRetryCount", 3);
}
