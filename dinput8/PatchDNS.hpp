#pragma once

hostent* __stdcall ResolveDNS(const char* hostname);

class PatchDNS
{
public:
	static BOOL Patch();
};
