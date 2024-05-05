#pragma once
#include "ExecutableType.hpp"

using boost::property_tree::ptree;

class Config
{
public:
	Config();

	static Config& GetInstance()
	{
		static Config* instance;

		if (instance == nullptr)
			instance = new Config();

		return *instance;
	}

	ExecutableType executableType = UNKNOWN;

	struct HookConfig
	{
		/* Client */
		std::string forceClientType;
		USHORT plasmaClientPort;
		USHORT plasmaServerPort;
		USHORT theaterClientPort;
		USHORT theaterServerPort;
		bool blockUnknownDomains;
		std::list<std::string> allowedDomains;

		/* Debug */
		bool showConsole;
		bool createLog;
		std::string logPath;

		INT consoleLogLevel;
		INT fileLogLevel;

		/* Patches */
		bool verifyGameVersion;
		bool patchDNS;
		bool patchSSL;

		/* Proxy */
		bool proxyEnable;
		std::string proxyAddress;
		USHORT proxyPort;
		std::string proxyPath;
		bool proxySSL;

		/* Overrides */
		std::string clientVersion;
		std::string serverVersion;
		USHORT sslPatchRetryCount;
	};

	HookConfig* hook = new HookConfig();
};
