# Disconnect-Protection

An AzerothCore module that protects players from dying during DDoS attacks or network outages by automatically casting Divine Intervention on all online players when connectivity is lost.

## Features

- Monitors network connectivity by pinging two URLs: `000.red` and `example.com`
- When BOTH URLs become unreachable for 3 consecutive seconds, the module triggers protection
- Automatically casts Divine Intervention (Spell ID 19753) on all online players
- Configurable check interval and unreachable threshold
- Prevents deaths during server DDoS attacks or network issues

## Installation

1. Clone this repository into your AzerothCore `modules` directory:
```bash
cd path/to/azerothcore/modules
git clone https://github.com/11011010/Disconnect-Protection.git
```

2. Re-run CMake and rebuild your server:
```bash
cd path/to/azerothcore/build
cmake ..
make -j $(nproc)
```

3. Copy the configuration file:
```bash
cp path/to/azerothcore/modules/Disconnect-Protection/conf/disconnect_protection.conf.dist path/to/azerothcore/env/dist/etc/modules/disconnect_protection.conf
```

4. Edit the configuration file if needed and restart your worldserver.

## Configuration

Edit `disconnect_protection.conf` to customize the module:

- **DisconnectProtection.Enable**: Enable or disable the module (default: 1)
- **DisconnectProtection.CheckInterval**: Time in milliseconds between connectivity checks (default: 1000)
- **DisconnectProtection.UnreachableThreshold**: Duration in seconds both URLs must be unreachable before triggering (default: 3)

## How It Works

1. The module runs a connectivity check every second (configurable)
2. It attempts to ping both `000.red` and `example.com`
3. If BOTH URLs are unreachable, it starts a countdown
4. After 3 seconds (configurable) of continuous unreachability, it casts Divine Intervention on all online players
5. Divine Intervention prevents players from dying and gives raid time to handle the situation
6. When connectivity is restored, the protection resets

## Requirements

- AzerothCore (WotLK 3.3.5a)
- libcurl (for HTTP connectivity checks)

## License

This module is released under the GNU AGPL v3 license. See LICENSE for details.
