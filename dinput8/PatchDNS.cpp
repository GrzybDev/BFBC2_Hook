#include "pch.hpp"

extern "C" {
hostent* (WINAPI __stdcall*original_gethostbyname)(const char* hostname) = gethostbyname;
}

hostent* __stdcall ResolveDNS(const char* hostname)
{
	BOOST_LOG_FUNCTION()

	const Config* config = &Config::GetInstance();

	if (config->hook->blockUnknownDomains)
	{
		if (std::find(config->hook->allowedDomains.begin(), config->hook->allowedDomains.end(), hostname) != config->
			hook->allowedDomains.end())
		{
			BOOST_LOG_TRIVIAL(debug) << hostname << " -> localhost";
			return original_gethostbyname("localhost");
		}

		BOOST_LOG_TRIVIAL(debug) << "DNS Resolution blocked for: " << hostname;
		return nullptr;
	}

	const auto resolved = original_gethostbyname(hostname);
	BOOST_LOG_TRIVIAL(debug) << hostname << " -> " << resolved->h_name;

	return resolved;
}

BOOL PatchDNS::Patch()
{
	BOOST_LOG_NAMED_SCOPE("DNSPatch")
	BOOST_LOG_TRIVIAL(info) << "Patching DNS Resolution...";

	const auto winsock2 = GetModuleHandleA("ws2_32.dll");
	original_gethostbyname = reinterpret_cast<hostent * (__stdcall*)(const char*)>(GetProcAddress(
		winsock2, "gethostbyname"));

	// Attach detour for gethostbyname
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&reinterpret_cast<PVOID&>(original_gethostbyname), ResolveDNS);

	const LONG commitStatus = DetourTransactionCommit();

	if (commitStatus != NO_ERROR)
	{
		/* Cleanup COM and Winsock */
		WSACleanup();

		BOOST_LOG_TRIVIAL(error) << "Failed to patch DNS Resolution!";
		BOOST_LOG_TRIVIAL(error) << "Detour library returned " << commitStatus <<
			" error code while transacting commit.";
		BOOST_LOG_TRIVIAL(error) << "Could not change gethostbyname function from ws2_32.dll to custom hook function.";

		return FALSE;
	}

	BOOST_LOG_TRIVIAL(info) << "Successfully patched DNS Resolution!";
	return TRUE;
}
