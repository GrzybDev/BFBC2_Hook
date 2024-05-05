#include "pch.hpp"

PatchSSL::PatchSSL()
{
	HANDLE hModule = GetModuleHandle(nullptr);

	size_ = Utils::GetModuleSize(hModule);
	offset_ = Utils::GetEntryPointOffset(hModule);
	entryPoint_ = reinterpret_cast<DWORD>(hModule) + offset_;
}

BOOL PatchSSL::Patch() const
{
	BOOST_LOG_NAMED_SCOPE("SSLPatch")

	const Config* config = &Config::GetInstance();
	const USHORT retryCount = config->hook->sslPatchRetryCount;

	for (int i = 0; i < retryCount; i++)
	{
		BOOST_LOG_TRIVIAL(info) << boost::format("Patching SSL Certificate Verification... (Attempt %i/%i)") % (i + 1) %
			retryCount;

		const DWORD sSSLPatchAddr = Utils::FindPattern(entryPoint_, size_, (BYTE*)"\x5E\xB8\x03\x10\x00\x00\x5D\xC3",
		                                               "xxxxxxxx");

		if (sSSLPatchAddr != NULL)
		{
			BOOST_LOG_TRIVIAL(debug) << boost::format("Found SSL Certificate Verification code at %1$#x") %
				sSSLPatchAddr;

			*reinterpret_cast<BYTE*>(sSSLPatchAddr + 2) = 0x15;
			*reinterpret_cast<BYTE*>(sSSLPatchAddr + 3) = 0x00;

			BOOST_LOG_TRIVIAL(info) << "Successfully patched SSL Certificate Verification code!";
			return TRUE;
		}

		BOOST_LOG_TRIVIAL(warning) << "Failed to find SSL Certificate Verification code, backing off for 1 second...";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	BOOST_LOG_TRIVIAL(error) << "Failed to find SSL Certificate Verification code!";
	BOOST_LOG_TRIVIAL(error) <<
		"Game executable is not compatible with this version of hook. (It either is outdated, compressed or protected in some other way)";
	BOOST_LOG_TRIVIAL(error) <<
		"Cannot continue now, if you want to launch the game but without SSL Certificate Verification please disable SSL Patch in config file.";

	return FALSE;
}
