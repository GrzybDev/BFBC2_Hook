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

|         Type | Value                                                          |
|-------------:|:---------------------------------------------------------------|
| Developer(s) | EA DICE                                                        |
| Publisher(s) | Electronic Arts                                                |
|  Producer(s) | Patrick Bach, Djörn Johnsson, John Hargelid, Anders Gyllenberg |
|       Series | Battlefield                                                    |
|       Engine | Frostbite 1.5                                                  |
|  Platform(s) | Microsoft Windows, PlayStation 3, Xbox 360, iOS, Kindle Fire   |
|     Genre(s) | First-person shooter                                           |
|      Mode(s) | Single-player, Multi-Player                                    |

Legal notes
-----------

- The project doesn't contain ***any*** original code from the game!
- To use this project you need to have an original copy of the game (bought from [Origin](https://www.ea.com/games/battlefield/battlefield-bad-company-2)), the project doesn't make piracy easier and doesn't break any of the DRM included in-game.

Build Requirements
------------------

- Visual Studio 2022

### Libraries:
- Boost
- detours
- OpenSSL v1.0.2 (with SSLv2/v3 support)

Compiling
---------

See [build workflow](https://github.com/GrzybDev/BFBC2_Hook/blob/main/.github/workflows/build.yaml) for more detailed steps

1. Create `dependencies` folder in project root
2. Build requirements (specified in [Build Requirements](#build-requirements)) to `dependencies` folder in project root, each library has to be installed in its own folder (look at [libs](https://github.com/GrzybDev/BFBC2_Hook/tree/main/libs) folder structure for naming)
3. Build project in Visual Studio (It has to be x86 Release build, it's the only one configuration setup, but please check before doing that)

Installing
----------

Copy all `dll` files from either the latest build in [GitHub Releases](https://github.com/GrzybDev/BFBC2_Hook/releases) or from `Release` folder if you compiled it yourself to the game root folder (where `exe` file is located)

Configuration
-------------

You can change the default behavior of the hook by creating JSON config named `config.json` in the game root folder

| Key                          | Description                                                                      | Allowed Values                 | Default Value                                                                                                                                      |
|------------------------------|----------------------------------------------------------------------------------|--------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------|
| client.forceClientType       | Force specific client type                                                       | String ("Client" or "Server")  |                                                                                                                                                    |
| client.plasmaClientPort      | Plasma port for clients (proxy will listen on that port)                         | Unsigned short (Range 0-65535) | 18390                                                                                                                                              |
| client.theaterClientPort     | Theater port for clients (proxy will listen on that port)                        | Unsigned short (Range 0-65535) | 18395                                                                                                                                              |
| client.plasmaServerPort      | Plasma port for servers (proxy will listen on that port)                         | Unsigned short (Range 0-65535) | 19021                                                                                                                                              |
| client.theaterServerPort     | Theater port for servers (proxy will listen on that port)                        | Unsigned short (Range 0-65535) | 19026                                                                                                                                              |
| client.blockUnknownDomains   | Block DNS resolution for unknown (not *allowed*) domains                         | Boolean (true/false)           | true                                                                                                                                               |
| client.allowedDomains        | List of domains that is allowed to be redirected to localhost (a.k.a proxy)      | List of strings                | ["bfbc2-pc.fesl.ea.com","bfbc2-pc.theater.ea.com","bfbc2-pc-server.fesl.ea.com","bfbc2-pc-server.theater.ea.com","easo.ea.com","bfbc2.gos.ea.com"] |
| debug.showConsole            | Shows hook debug console                                                         | Boolean (true/false)           | false                                                                                                                                              |
| debug.createLog              | Create hook log file                                                             | Boolean (true/false)           | false                                                                                                                                              |
| debug.logPath                | Path to where save log file                                                      | Any string                     | bfbc2_(current date and time).log                                                                                                                  |
| debug.logLevelConsole        | Changes how detailed console logging is                                          | Integer (Range 0-5)            | 2                                                                                                                                                  |
| debug.logLevelFile           | Changes how detailed file logging is                                             | Integer (Range 0-5)            | 1                                                                                                                                                  |
| patches.verifyGameVersion    | Verify whether game version is compatible with hook                              | Boolean (true/false)           | true                                                                                                                                               |
| patches.DNS                  | Patch game DNS resolution (redirects all game requests to localhost)             | Boolean (true/false)           | true                                                                                                                                               |
| patches.SSL                  | Patch game SSL certificate verification (required for hook proxy to work)        | Boolean (true/false)           | true                                                                                                                                               |
| proxy.enable                 | Enable/Disable hook proxy                                                        | Boolean (true/false)           | true                                                                                                                                               |
| proxy.host                   | IP Address or Domain Name of custom server implementation                        | Any string                     | bfbc2.grzyb.app                                                                                                                                    |
| proxy.port                   | Port of custom server implementation                                             | Unsigned short (Range 0-65535) | 443                                                                                                                                                |
| proxy.path                   | Endpoint path                                                                    | Any string                     | /ws                                                                                                                                                |
| proxy.secure                 | Use secure connection (wss) or not encrypted connection (ws)                     | Boolean (true/false)           | true                                                                                                                                               |
| overrides.clientVersion      | Client version identifier                                                        | Any string                     | "ROMEPC795745"                                                                                                                                     |
| overrides.serverVersion      | Server version identifier                                                        | Any string                     | "ROMEPC851434"                                                                                                                                     |
| overrides.sslPatchRetryCount | Number of times hook will attempt to patch SSL Certificate Verification          | Unsigned short (Range 0-65535) | 3                                                                                                                                                  |

### Example configuration

This config will make game connect to locally hosted server, with final endpoint being `ws://localhost:8000/ws`

```
{
   "debug":{
      "logLevelConsole":1
   },
   "proxy":{
      "address":"localhost",
      "port":8000,
      "secure":false
   }
}
```

Credits
-------

- [GrzybDev](https://grzyb.dev)

Special thanks to:
- DICE (for making the game)
