// PatchDNS.cpp : Defines the PatchDNS class, which handles the patching of DNS resolution.
#include "pch.hpp"

extern "C" {
hostent* (WINAPI __stdcall*original_gethostbyname)(const char* hostname) = gethostbyname;
}

hostent* __stdcall ResolveDNS(const char* hostname)
{
	BOOST_LOG_FUNCTION()

	const Config* config = &Config::GetInstance();

	// If blocking unknown domains is enabled in the configuration
	if (config->hook->blockUnknownDomains)
	{
		// If the hostname is in the list of allowed domains
		if (std::find(config->hook->allowedDomains.begin(), config->hook->allowedDomains.end(), hostname) != config->
			hook->allowedDomains.end())
		{
			// Log a debug message and return the host entry for localhost
			BOOST_LOG_TRIVIAL(debug) << hostname << " -> localhost";
			return original_gethostbyname("localhost");
		}

		// If the hostname is not in the list of allowed domains, log a debug message and return null
		BOOST_LOG_TRIVIAL(debug) << "DNS Resolution blocked for: " << hostname;
		return nullptr;
	}

	// If blocking unknown domains is not enabled in the configuration, resolve the hostname and log a debug message
	const auto resolved = original_gethostbyname(hostname);
	BOOST_LOG_TRIVIAL(debug) << hostname << " -> " << resolved->h_name;

	return resolved;
}

BOOL PatchDNS::Patch()
{
	BOOST_LOG_NAMED_SCOPE("DNSPatch")
	BOOST_LOG_TRIVIAL(info) << "Patching DNS Resolution...";

	// Get the handle for ws2_32.dll and the address of gethostbyname
	const auto winsock2 = GetModuleHandleA("ws2_32.dll");
	original_gethostbyname = reinterpret_cast<hostent * (__stdcall*)(const char*)>(GetProcAddress(
		winsock2, "gethostbyname"));

	// Begin a detour transaction, update the thread, and attach a detour for the gethostbyname function
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&reinterpret_cast<PVOID&>(original_gethostbyname), ResolveDNS);

	// Commit the detour transaction
	const LONG commitStatus = DetourTransactionCommit();

	// If the commit fails
	if (commitStatus != NO_ERROR)
	{
		// Cleanup COM and Winsock
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
