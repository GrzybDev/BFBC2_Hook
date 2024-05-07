// PatchSSL.cpp : Defines the PatchSSL class, which handles the patching of SSL certificate verification.
#include "pch.hpp"

PatchSSL::PatchSSL()
{
	// Get the handle of the current module
	HANDLE hModule = GetModuleHandle(nullptr);

	// Get the size and entry point offset of the module
	size_ = Utils::GetModuleSize(hModule);
	offset_ = Utils::GetEntryPointOffset(hModule);
	// Calculate the entry point of the module
	entryPoint_ = reinterpret_cast<DWORD>(hModule) + offset_;
}

BOOL PatchSSL::Patch() const
{
	BOOST_LOG_NAMED_SCOPE("SSLPatch")

	const Config* config = &Config::GetInstance();
	const USHORT retryCount = config->hook->sslPatchRetryCount;

	// Try to patch SSL certificate verification for the number of times specified in the retry count
	for (int i = 0; i < retryCount; i++)
	{
		BOOST_LOG_TRIVIAL(info) << boost::format("Patching SSL Certificate Verification... (Attempt %i/%i)") % (i + 1) %
			retryCount;

		// Find the address of the SSL certificate verification code
		const DWORD sSSLPatchAddr = Utils::FindPattern(entryPoint_, size_, (BYTE*)"\x5E\xB8\x03\x10\x00\x00\x5D\xC3",
		                                               "xxxxxxxx");

		// If the address is found
		if (sSSLPatchAddr != NULL)
		{
			BOOST_LOG_TRIVIAL(debug) << boost::format("Found SSL Certificate Verification code at %1$#x") %
				sSSLPatchAddr;

			// Patch the SSL certificate verification code
			*reinterpret_cast<BYTE*>(sSSLPatchAddr + 2) = 0x15;
			*reinterpret_cast<BYTE*>(sSSLPatchAddr + 3) = 0x00;

			BOOST_LOG_TRIVIAL(info) << "Successfully patched SSL Certificate Verification code!";
			return TRUE;
		}

		// If the address is not found, log a warning message and wait for 1 second
		BOOST_LOG_TRIVIAL(warning) << "Failed to find SSL Certificate Verification code, backing off for 1 second...";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	// If the address is not found after all attempts, log error messages and return false
	BOOST_LOG_TRIVIAL(error) << "Failed to find SSL Certificate Verification code!";
	BOOST_LOG_TRIVIAL(error) <<
		"Game executable is not compatible with this version of hook. (It either is outdated, compressed or protected in some other way)";
	BOOST_LOG_TRIVIAL(error) <<
		"Cannot continue now, if you want to launch the game but without SSL Certificate Verification please disable SSL Patch in config file.";

	return FALSE;
}
