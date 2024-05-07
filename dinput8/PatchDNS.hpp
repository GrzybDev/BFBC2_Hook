// PatchDNS.hpp : Defines the PatchDNS class and the ResolveDNS function, which are used for patching DNS resolution.
#pragma once

hostent* __stdcall ResolveDNS(const char* hostname);

class PatchDNS
{
public:
	static BOOL Patch();
};
