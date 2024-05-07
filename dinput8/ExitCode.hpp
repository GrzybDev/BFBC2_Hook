// ExitCode.hpp : Defines the ExitCode enumeration, which represents the possible exit codes of the application.
#pragma once

enum ExitCode
{
	OK = 0,
	UNKNOWN_GAME_VERSION = 1,
	PATCH_FAILED = 2,
	PROXY_STARTUP_FAILURE = 3,
};
