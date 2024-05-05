#include "pch.hpp"

VOID Utils::CenterPrint(std::string text, const CHAR filler, const bool endNewLine)
{
	using namespace std;

	if (!text.empty())
		text = " " + text + " ";

	CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleScreenBufferInfo);

	const UINT columns = consoleScreenBufferInfo.srWindow.Right - consoleScreenBufferInfo.srWindow.Left + 1;
	const UINT paddingSize = columns / 2 - text.size() / 2;

	PrintChar(filler, paddingSize);
	cout << text;
	PrintChar(filler, paddingSize);

	if (endNewLine)
		cout << '\n';
}

VOID Utils::PrintChar(const CHAR c, const UINT count)
{
	using namespace std;

	for (UINT i = 0; i < count; i++)
		cout << c;
}

DWORD Utils::FindPattern(const DWORD start, const DWORD size, const BYTE* pattern, const CHAR* mask)
{
	UINT i = NULL;
	const INT iLen = strlen(mask) - 1;

	for (DWORD dwRet = start; dwRet < start + size; dwRet++)
	{
		if (*reinterpret_cast<BYTE*>(dwRet) == pattern[i] || mask[i] == '?')
		{
			if (mask[i + 1] == '\0')
				return (dwRet - iLen);

			i++;
		}
		else
			i = NULL;
	}

	return NULL;
}

DWORD Utils::GetModuleSize(const HANDLE handle)
{
	auto hModule = static_cast<HMODULE>(handle);

	if (!hModule)
		return NULL;

	const auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);

	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;

	const auto ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<DWORD>(hModule) + dosHeader->e_lfanew);

	if (ntHeader->Signature != IMAGE_NT_SIGNATURE)
		return NULL;

	const PIMAGE_OPTIONAL_HEADER pImageOptionalHeader = &ntHeader->OptionalHeader;
	return pImageOptionalHeader->SizeOfImage;
}

DWORD Utils::GetEntryPointOffset(const HANDLE hHandle)
{
	auto hModule = static_cast<HMODULE>(hHandle);

	if (!hModule)
		return NULL;

	const auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);

	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;

	const auto ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<DWORD>(hModule) + dosHeader->e_lfanew);

	if (ntHeader->Signature != IMAGE_NT_SIGNATURE)
		return NULL;

	const PIMAGE_OPTIONAL_HEADER pImageOptionalHeader = &ntHeader->OptionalHeader;
	return pImageOptionalHeader->BaseOfCode;
}
