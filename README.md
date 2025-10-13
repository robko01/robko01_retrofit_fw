
# Robko01 Retrofit Firmware

This repository contains the firmware and build configuration for the Robko01 retrofit project. It provides PlatformIO environments, example configuration files, and helper scripts to build, flash, and update firmware for retrofit hardware. The firmware integrates WiFi and WireGuard networking support and includes optional OTA update flows and multiple serial environment configurations for different use cases. Use the provided PlatformIO INI files and the pre-build script to customize builds for local, OTA, and remote deployments. The instructions below explain how to set environment variables needed at build time and how to supply device-specific values such as PS4 controller MAC addresses.


## Setup the environment variables 
**This commands are for Windows terminal only.**

### Wi-Fi
Setup the WiFi SSID name.
```sh
    set WIFI_SSID="\"YOUR_SSID\""
```

Setup the WiFi password.
```sh
    set YOUR_PASS="\"YOUR_PASS\""
```

### OTA
port
```sh
    set OTA_PORT=3232
```

host
```sh
    set OTA_HOST_NAME="\"Robko01\""
```

password hash
```sh
    set OTA_PASS_HASH="\"21232f297a57a5a743894a0e4a801fc3\""
```

### Wireguard
Endpoint IP address.
```sh
    set WG_ENDPOINT="\"WG_ENDPOINT\""
```

Local IP address.
```sh
    set WG_LOCAL_IP="\"WG_LOCAL_IP\""
```

Your private key.
```sh
    set WG_PRIVATE_KEY="\"WG_PRIVATE_KEY\""
```

Server public key.
```sh
    set WG_PUBLIC_KEY="\"WG_PUBLIC_KEY\""
```

### Playstation 4 controller
PS4 controller MAC address.
```sh
    set PS4_MAC="\"XX:XX:XX:XX:XX:XX\""
```

## Notes

[Doc](https://docs.platformio.org/en/latest/envvars.html) specification how to use environment variables in build time.

[Article](https://community.platformio.org/t/providing-credentials-without-the-keys-being-displayed-in-the-code/32437/3) that describe how to use commands described previous:

## Wireguard


