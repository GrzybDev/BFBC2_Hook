#include "pch.hpp"

DirectInput8Create_t original_function;
HMODULE original_library;

DINPUT8_API HRESULT DirectInput8Create(const HINSTANCE hinst, const DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut,
                                       const LPUNKNOWN punkOuter)
{
	return original_function(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}
