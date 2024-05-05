#include "pch.hpp"
#include "Hook.hpp"

Hook::Hook()
{
	InitLogging();

	BOOST_LOG_FUNCTION()
	BOOST_LOG_TRIVIAL(info) << "Initializing...";

	VerifyGameVersion();
	PatchGame();

	BOOST_LOG_TRIVIAL(info) << "Initialized successfully!";
}

VOID Hook::InitLogging() const
{
	using namespace std;
	namespace logging = boost::log;
	namespace expr = logging::expressions;

	const bool enableConsole = config_->hook->showConsole;
	const bool enableLogFile = config_->hook->createLog;
	int consoleLogLevel = config_->hook->consoleLogLevel;
	int fileLogLevel = config_->hook->fileLogLevel;

	logging::add_common_attributes();
	logging::core::get()->add_global_attribute("Scope", boost::log::attributes::named_scope());

	const auto logFormat = expr::format("[%1% %2%] %3%: %4%")
		% expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
		% expr::format_named_scope("Scope", logging::keywords::format = "%C")
		% logging::trivial::severity
		% expr::smessage;

	if (enableConsole)
	{
		// Create a console for Debug output
		AllocConsole();

		// Redirect standard error, output to console
		// std::cout, std::clog, std::cerr, std::cin
		FILE* fDummy;

		freopen_s(&fDummy, "CONOUT$", "w", stdout);
		freopen_s(&fDummy, "CONOUT$", "w", stderr);
		freopen_s(&fDummy, "CONIN$", "r", stdin);

		cout.clear();
		clog.clear();
		cerr.clear();
		cin.clear();

		// std::wcout, std::wclog, std::wcerr, std::wcin
		const HANDLE hConOut = CreateFile(_T("CONOUT$"), GENERIC_READ | GENERIC_WRITE,
		                                  FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
		                                  FILE_ATTRIBUTE_NORMAL, nullptr);
		const HANDLE hConIn = CreateFile(_T("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		                                 nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
		SetStdHandle(STD_ERROR_HANDLE, hConOut);
		SetStdHandle(STD_INPUT_HANDLE, hConIn);

		wcout.clear();
		wclog.clear();
		wcerr.clear();
		wcin.clear();

		const auto consoleSink = logging::add_console_log(std::cout);
		consoleSink->set_filter(logging::trivial::severity >= consoleLogLevel);
		consoleSink->set_formatter(logFormat);

		// Intro
		Utils::CenterPrint("Battlefield: Bad Company 2 Hook by Marek Grzyb (@GrzybDev)", '=', true);
		Utils::CenterPrint("Homepage: https://grzyb.dev/project/bfbc2emu", ' ', false);
		Utils::CenterPrint("Source code: https://github.com/GrzybDev/BFBC2_Hook", ' ', false);
		Utils::CenterPrint("Noticed a bug? Fill a bug report here: https://github.com/GrzybDev/BFBC2_Hook/issues", ' ',
		                   false);
		Utils::CenterPrint("Licensed under GNU Lesser General Public License v3, Contributions of any kind welcome!",
		                   ' ', true);
		Utils::CenterPrint("", '=', true);
	}

	if (enableLogFile)
	{
		const auto fileSink = logging::add_file_log(config_->hook->logPath);
		fileSink->set_filter(logging::trivial::severity >= fileLogLevel);
		fileSink->set_formatter(logFormat);
	}
}

VOID Hook::VerifyGameVersion() const
{
	BOOST_LOG_NAMED_SCOPE("GameVersion")

	std::string exeType;

	if (config_->hook->forceClientType.empty())
	{
		// "ROMEPC795745" - Client R11
		DWORD mClientVersionAddr = Utils::FindPattern(0x1400000, 0x600000, (BYTE*)config_->hook->clientVersion.c_str(),
		                                              "xxxxxxxxxxxxxx");

		// "ROMEPC851434" - Server R34
		DWORD mServerVersionAddr = Utils::FindPattern(0x1600000, 0x600000, (BYTE*)config_->hook->serverVersion.c_str(),
		                                              "xxxxxxxxxxxxxx");

		if (config_->hook->verifyGameVersion && (mClientVersionAddr == NULL && mServerVersionAddr == NULL))
		{
			MessageBoxA(nullptr, "Unknown client/server detected!", "Failed to initialize hook!", MB_OK | MB_ICONERROR);
			ExitProcess(UNKNOWN_GAME_VERSION);
		}

		if (mClientVersionAddr != NULL)
		{
			exeType = "Client";
			config_->executableType = CLIENT;
		}
		else if (mServerVersionAddr != NULL)
		{
			exeType = "Server";
			config_->executableType = SERVER;
		}
	}
	else
	{
		const std::string clientType = boost::algorithm::to_upper_copy(config_->hook->forceClientType);

		if (clientType == "CLIENT")
		{
			exeType = "Client";
			config_->executableType = CLIENT;
		}
		else if (clientType == "SERVER")
		{
			exeType = "Server";
			config_->executableType = SERVER;
		}
		else
		{
			MessageBoxA(nullptr, "Invalid client type specified!", "Failed to initialize hook!", MB_OK | MB_ICONERROR);
			ExitProcess(UNKNOWN_GAME_VERSION);
		}

		BOOST_LOG_TRIVIAL(warning) << "Forcing client type: " << exeType;
	}

	BOOST_LOG_TRIVIAL(info) << "Detected executable type: " << exeType;
}

VOID Hook::PatchGame() const
{
	if (config_->hook->patchDNS)
	{
		if (!PatchDNS::Patch())
		{
			MessageBoxA(nullptr, "Failed to patch DNS resolution!", "Failed to initialize hook!", MB_OK | MB_ICONERROR);
			ExitProcess(PATCH_FAILED);
		}
	}

	if (config_->hook->patchSSL)
	{
		const PatchSSL* sslPatch = &PatchSSL::GetInstance();

		if (!sslPatch->Patch())
		{
			MessageBoxA(nullptr, "Failed to patch SSL certificate verification!", "Failed to initialize hook!",
			            MB_OK | MB_ICONERROR);
			ExitProcess(PATCH_FAILED);
		}
	}
}
