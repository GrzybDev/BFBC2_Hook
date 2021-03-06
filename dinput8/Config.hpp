#pragma once

class Config
{
public:
	Config();
	Config(const Config&) {}

	static Config& getInstance() {
		static Config* _instance = nullptr;

		if (_instance == nullptr)
			_instance = new Config();

		return *_instance;
	}

	struct HookConfig
	{
		/* Debug */
		bool showConsole; // Show debug console
		bool createLog; // Create a log file
		std::string logPath; // Log file path

		int consoleLogLevel; // Console message level
		int fileLogLevel; // File message level

		/* Patches */
		bool verifyGameVersion; // Verify if game version is supported
		bool patchDNS; // Patch DNS Resolution
		bool patchSSL; // Patch SSL Verification

		/* Proxy */
		bool proxyEnable; // Enable proxy
		bool connectRetail; // Connect to retail
	};

	HookConfig* hook = new HookConfig();

private:
	void readConfig() const;
	void parseConfig(boost::property_tree::ptree pt) const;
};
