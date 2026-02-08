# Robko01 Retrofit Firmware

Firmware and PlatformIO build setup for the Robko01 retrofit project. This repo includes multiple build profiles (serial, OTA, Modbus TCP/RTU, PS4), a `.env`-driven configuration flow, and helper scripts for repeatable builds.

## Contents
- Getting started
- Build profiles
- Configuration via `.env`
- Modbus documentation
- Changelog
- Contributing
- Agent guidelines

## Getting started — VS Code, PlatformIO & Git

### Windows
1. Install Visual Studio Code and the PlatformIO IDE extension.
2. Install Git for Windows.
3. Clone the repo:
```powershell
cd C:\Users\<User>\Desktop\PlatformIO
git clone https://github.com/robko01/robko01_retrofit_fw.git
cd robko01_retrofit_fw
```

### Linux
```bash
cd ~/Desktop/PlatformIO
git clone https://github.com/robko01/robko01_retrofit_fw.git
cd robko01_retrofit_fw
```

### macOS
```bash
cd ~/Desktop/PlatformIO
git clone https://github.com/robko01/robko01_retrofit_fw.git
cd robko01_retrofit_fw
```

Notes:
- If you installed PlatformIO Core globally (pipx/Homebrew/system), you can run `pio` directly without activating a virtualenv.
- After cloning, open the folder in VS Code and allow PlatformIO to finish setup.

## Build profiles

Profiles are defined in `platformio.ini` and the included `platformio_*.ini` files.

Common environments:
- `serial_local`, `serial_home`, `serial_remote`, `serial_ps4`
- `ota_local`, `ota_remote`
- `modbus_tcp`, `modbus_rtu`
- `env_dump` (prints resolved `.env` values on device boot)

Build example:
```bash
pio run -e serial_ps4
```

Upload example:
```bash
pio run -e serial_ps4 -t upload
```

## Configuration via `.env`

All build-time settings are loaded from `.env` by `pre_build.py`, then injected as C/C++ defines. You can keep device-specific values there without exporting them in your shell.

Sample `.env`:
```
# Project-level environment values (do not commit secrets to git)
WIFI_SSID=YourWifiSSID
WIFI_PASS=YourWifiPassword
OTA_PORT=3232
OTA_HOST_NAME=ROBKO01
OTA_PASS_HASH=21232f297a57a5a743894a0e4a801fc3
WG_ENDPOINT=1.2.3.4
WG_LOCAL_IP=10.0.0.2
WG_PRIVATE_KEY=your_wireguard_private_key_here
WG_PUBLIC_KEY=server_wireguard_public_key_here
PS4_MAC=E8:61:7E:40:63:18
```

Overrides (optional): environment variables in your shell take precedence over `.env`.

Security note: `.env` is ignored by git. Do not commit secrets.

## Modbus documentation

- Modbus TCP: `doc/MODBUS_TCP.md`
- Modbus RTU: `doc/MODBUS_RTU.md`

Build profiles:
```bash
pio run -e modbus_tcp
pio run -e modbus_rtu
```

## Changelog

See `CHANGELOG.md` for release notes and feature history.

## Contributing

See `CONTRIBUTING.md` for branch strategy, commit format, testing expectations, and merge/version workflow.

## Agent guidelines

See `AGENTS` for coding conventions and assistant-specific project guidance.

## Build (full workflow)

### Windows
```powershell
cd C:\Users\<User>\.platformio\penv\Scripts\
./activate

cd C:\Users\<User>\Desktop\PlatformIO\robko01_retrofit_fw
pio run -t clean
pio run -e serial_ps4
pio run -t upload
```

### Linux/macOS
```bash
source ~/.platformio/penv/bin/activate
cd ~/Desktop/PlatformIO/robko01_retrofit_fw
pio run -t clean
pio run -e serial_ps4
pio run -t upload
```



