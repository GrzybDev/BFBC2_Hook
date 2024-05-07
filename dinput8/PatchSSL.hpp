// PatchSSL.hpp : Defines the PatchSSL class, which handles the patching of SSL certificate verification.
#pragma once

class PatchSSL
{
public:
	PatchSSL();

	static PatchSSL& GetInstance()
	{
		static PatchSSL* instance;

		if (instance == nullptr)
			instance = new PatchSSL();

		return *instance;
	}

	BOOL Patch() const;

private:
	DWORD size_;
	DWORD offset_;
	DWORD entryPoint_;
};
