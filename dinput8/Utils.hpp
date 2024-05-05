#pragma once

class Utils
{
public:
	static VOID CenterPrint(std::string text, CHAR filler, bool endNewLine = true);
	static VOID PrintChar(CHAR c, UINT count);

	static DWORD FindPattern(DWORD start, DWORD size, const BYTE* pattern, const CHAR* mask);
};
