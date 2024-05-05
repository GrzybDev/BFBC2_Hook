// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#define CLIENT_PLASMA_PORT 18390
#define CLIENT_THEATER_PORT 18395
#define SERVER_PLASMA_PORT 19021
#define SERVER_THEATER_PORT 19026
#define HTTP_PORT 80
#define HTTPS_PORT 443

// add headers that you want to pre-compile here
#include "framework.hpp"
#include "dinput8.hpp"
#include "Config.hpp"
#include "ExecutableType.hpp"
#include "Utils.hpp"
#include "ExitCode.hpp"
#include "PatchDNS.hpp"
#include "PatchSSL.hpp"

#endif //PCH_H
