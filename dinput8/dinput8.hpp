#pragma once
#include <Unknwn.h>

#define DINPUT8_API __declspec(dllexport)

using DirectInput8Create_t = HRESULT(*)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut,
                                        LPUNKNOWN punkOuter);

extern DirectInput8Create_t original_function;
extern HMODULE original_library;

extern "C" {
DINPUT8_API HRESULT DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut,
                                       LPUNKNOWN punkOuter);
}
