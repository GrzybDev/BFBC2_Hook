// Hook.hpp : Defines the Hook class, which handles the initialization, game version verification, and game patching.
#pragma once

#include "pch.hpp"

class Hook
{
public:
	Hook();

	static Hook& GetInstance()
	{
		static Hook* instance;

		if (instance == nullptr)
			instance = new Hook();

		return *instance;
	}

private:
	Config* config_ = &Config::GetInstance();

	VOID InitLogging() const;
	VOID VerifyGameVersion() const;
	VOID PatchGame() const;
};
