Battlefield: Bad Company 2 Hook
===============================

Hook for Battlefield: Bad Company 2 that enables proxying game network traffic (Plasma, Theater and HTTP) to either retail server or WebSocket server re-implementation (like [BFBC2_MasterServer](https://github.com/GrzybDev/BFBC2_MasterServer))

Table of Contents
-----------------
- [Game Info](#game-info)
- [Legal notes](#legal-notes)
- [Build Requirements](#build-requirements)
- [Compiling](#compiling)
- [Installing](#installing)
- [Configuration](#configuration)
- [Credits](#credits)

Game Info
---------
![Battlefield: Bad Company 2 Cover](https://upload.wikimedia.org/wikipedia/en/b/b3/Battlefield_Bad_Company_2_cover.jpg "Battlefield: Bad Company 2 Cover")

|         Type | Value                                                        |
|-------------:|:-------------------------------------------------------------|
| Developer(s) | EA DICE                                                      |
| Publisher(s) | Electronic Arts                                              |
|    Writer(s) | David Goldfarb                                               |
|  Composer(s) | Mikael Karlsson, Joel Eriksson                               |
|       Series | Battlefield                                                  |
|       Engine | Frostbite 1.5                                                |
|  Platform(s) | Microsoft Windows, PlayStation 3, Xbox 360, iOS, Kindle Fire |
|     Genre(s) | First-person shooter                                         |
|      Mode(s) | Single-player, Multi-Player                                  |

Legal notes
-----------

- The project doesn't contain ***any*** original code from the game!
- To use this project you need to have an original copy of the game (bought from [Origin](https://www.ea.com/games/battlefield/battlefield-bad-company-2)), the project doesn't make piracy easier and doesn't break any of the DRM included in-game.

Build Requirements
------------------

- Visual Studio 2022

### Libraries:
- Boost
- cpprestsdk
- detours
- OpenSSL v1.0.2 (with SSLv2/v3 support)
- OpenSSL v1.1.1
- zlib

Compiling
---------

See [build workflow](https://github.com/GrzybDev/BFBC2_Hook/blob/main/.github/workflows/build.yaml) for more detailed steps

1. Create `third_party` folder in project root
2. Build requirements (specified in [Build Requirements](#build-requirements)) to `third_party` folder in project root, each library has to be installed in its own folder (look at [external](https://github.com/GrzybDev/BFBC2_Hook/tree/main/external) folder structure for naming)
3. Build project in Visual Studio (It has to be x86 Release build, it's the only one configuration setup, but please check before doing that)

Installing
----------

Copy all `dll` files from either the latest build in [GitHub Releases](https://github.com/GrzybDev/BFBC2_Hook/releases) or from `Release` folder if you compiled it yourself to the game root folder (where `exe` file is located)

Configuration
-------------

You can change the default behavior of the hook by creating JSON config named `config.json` in the game root folder

| Key                          | Description                                                                      | Allowed Values                 | Default Value                     |
|------------------------------|----------------------------------------------------------------------------------|--------------------------------|-----------------------------------|
| client.forceClientType       | Force specific client type                                                       | String ("Client" or "Server")  |                                   |
| client.clientPlasmaPort      | Plasma port for clients (proxy will listen on that port)                         | Unsigned short (Range 0-65535) | 18390                             |
| client.clientTheaterPort     | Theater port for clients (proxy will listen on that port)                        | Unsigned short (Range 0-65535) | 18395                             |
| client.serverPlasmaPort      | Plasma port for servers (proxy will listen on that port)                         | Unsigned short (Range 0-65535) | 18321                             |
| client.serverTheaterPort     | Theater port for servers (proxy will listen on that port)                        | Unsigned short (Range 0-65535) | 18326                             |
| debug.showConsole            | Shows hook debug console                                                         | Boolean (true/false)           | true                              |
| debug.createLog              | Create hook log file                                                             | Boolean (true/false)           | false                             |
| debug.logPath                | Path to where save log file                                                      | Any string                     | bfbc2_(current date and time).log |
| debug.logLevelConsole        | Changes how detailed console logging is                                          | Integer (Range 0-5)            | 2                                 |
| debug.logLevelFile           | Changes how detailed file logging is                                             | Integer (Range 0-5)            | 1                                 |
| patches.verifyGameVersion    | Verify whether game version is compatible with hook                              | Boolean (true/false)           | true                              |
| patches.patchDNS             | Patch game DNS resolution (redirects all game requests to localhost)             | Boolean (true/false)           | true                              |
| patches.patchSSL             | Patch game SSL certificate verification (required for hook proxy to work)        | Boolean (true/false)           | true                              |
| proxy.enable                 | Enable/Disable hook proxy                                                        | Boolean (true/false)           | true                              |
| proxy.connectToRetail        | Connect to retail server instead of custom server (useful for capturing traffic) | Boolean (true/false)           | false                             |
| proxy.serverAddress          | IP Address or Domain Name of custom server implementation                        | Any string                     | bfbc2.grzyb.app                   |
| proxy.serverPort             | Port of custom server implementation                                             | Unsigned short (Range 0-65535) | 443                               |
| proxy.serverSecure           | Use secure connection (wss) or not encrypted connection (ws)                     | Boolean (true/false)           | true                              |
| overrides.clientVersion      | Client version identifier                                                        | Any string                     | "ROMEPC795745"                    |
| overrides.serverVersion      | Server version identifier                                                        | Any string                     | "ROMEPC851434"                    |
| overrides.sslPatchRetryCount | Number of times hook will attempt to patch SSL Certificate Verification          | Unsigned short (Range 0-65535) | 3                                 |

### Example configuration

```
{
   "debug":{
      "logLevelConsole":1
   },
   "proxy":{
      "connectToRetail":false,
      "serverAddress":"localhost",
      "serverPort":8000,
      "serverSecure":false
   }
}
```
(Makes hook connect to locally hosted server implementation [`ws://localhost:8000`])

Credits
-------

- [GrzybDev](https://grzyb.dev)

Special thanks to:
- DICE (for making the game)
