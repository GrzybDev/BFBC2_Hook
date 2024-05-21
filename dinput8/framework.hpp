// framework.hpp : Includes the necessary headers and libraries for the application.
#pragma once

// Windows Header Files
#include <windows.h>
#include <tchar.h>

// Standard C++ Library Headers
#include <string>
#include <list>

// Boost Library Headers
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/array.hpp>

// WinSock2 Library Headers
#include "WinSock2.h"
#pragma comment(lib, "ws2_32.lib")

// Detours Library Headers
#include "detours.h"
#pragma comment(lib, "detours.lib")

// OpenSSL Library
#pragma comment(lib, "ssleay32.lib")
#pragma comment(lib, "libeay32.lib")
