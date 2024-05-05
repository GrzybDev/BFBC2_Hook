// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.hpp"

#include "Hook.hpp"

VOID LoadOriginalLibrary(const std::string& libraryName)
{
	// Get the system directory
	char libraryPath[MAX_PATH];
	GetSystemDirectoryA(libraryPath, MAX_PATH);

	// Append the library name to the path
	strcat_s(libraryPath, ("\\" + libraryName).c_str());

	// Load the original library
	original_library = LoadLibraryA(libraryPath);

	if (original_library >= reinterpret_cast<HMODULE>(HINSTANCE_ERROR))
	{
		original_function = reinterpret_cast<DirectInput8Create_t>(GetProcAddress(
			original_library, "DirectInput8Create"));
	}
}

VOID UnprotectProcessMemory()
{
	auto hModule = GetModuleHandle(nullptr);

	const auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
	const auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<DWORD>(hModule) + dosHeader->e_lfanew);

	// Unprotect entire PE image.
	const auto size = ntHeaders->OptionalHeader.SizeOfImage;
	DWORD oldProtect;
	VirtualProtect(hModule, size, PAGE_EXECUTE_READWRITE, &oldProtect);
}

DWORD WINAPI InitHook(LPVOID /*lpParam*/)
{
	Hook::GetInstance();

	return NULL;
}

BOOL APIENTRY DllMain(HMODULE /*hModule*/,
                      const DWORD ulReasonForCall,
                      LPVOID /*lpReserved*/
)
{
	if (ulReasonForCall == DLL_PROCESS_ATTACH)
	{
		LoadOriginalLibrary("dinput8.dll");
		UnprotectProcessMemory();

		CreateThread(nullptr, NULL, InitHook, nullptr, NULL, nullptr);
	}

	return TRUE;
}
