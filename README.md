
# Robko01 Retrofit Firmware

This repository contains the firmware and build configuration for the Robko01 retrofit project. It provides PlatformIO environments, example configuration files, and helper scripts to build, flash, and update firmware for retrofit hardware. The firmware integrates WiFi and WireGuard networking support and includes optional OTA update flows and multiple serial environment configurations for different use cases. Use the provided PlatformIO INI files and the pre-build script to customize builds for local, OTA, and remote deployments. The instructions below explain how to set environment variables needed at build time and how to supply device-specific values such as PS4 controller MAC addresses.


## Getting started — VS Code, PlatformIO & Git

Follow the steps below to install Visual Studio Code, the PlatformIO extension, and Git, then clone this repository. Pick the instructions for your operating system.

### Windows
- Install Visual Studio Code: download from https://code.visualstudio.com/ and run the installer.
- Open VS Code, go to Extensions (left sidebar) and install "PlatformIO IDE" (search for "PlatformIO").
- Install Git for Windows: download from https://git-scm.com/download/win and run the installer (enable "Use Git from the Windows Command Prompt" if you want git on PATH).
- Open PowerShell or Git Bash and run:
```powershell
cd C:\Users\<User>\Desktop\PlatformIO
git clone https://github.com/robko01/robko01_retrofit_fw.git
cd robko01_retrofit_fw
```

### Linux
- Install Visual Studio Code: follow platform instructions at https://code.visualstudio.com/ (deb/rpm or snap).
- Open VS Code and install the "PlatformIO IDE" extension from the Extensions pane.
- Install Git: on Debian/Ubuntu run `sudo apt update; sudo apt install git`, on Fedora use `sudo dnf install git`.
- Clone the repo:
```bash
cd ~/Desktop/PlatformIO
git clone https://github.com/robko01/robko01_retrofit_fw.git
cd robko01_retrofit_fw
```

### macOS
- Install Visual Studio Code: download the macOS build from https://code.visualstudio.com/ and move it to Applications.
- Open VS Code and install the "PlatformIO IDE" extension.
- Install Git: either install Xcode Command Line Tools `xcode-select --install` or install Git via Homebrew `brew install git`.
- Clone the repo:
```bash
cd ~/Desktop/PlatformIO
git clone https://github.com/robko01/robko01_retrofit_fw.git
cd robko01_retrofit_fw
```

Notes:
- If you installed PlatformIO via pip, pipx or Homebrew, you may not need the PlatformIO VS Code extension to run builds from the command line, but the extension provides an integrated workflow in VS Code.
- After cloning, open the folder in VS Code (`File -> Open Folder...`) and allow PlatformIO to finish any recommended extension or environment setup.


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


## Build

This section shows how to run the PlatformIO build and upload steps on Windows, Linux and macOS. The commands below mirror the Windows sequence you provided (activate the PlatformIO environment, change into the project folder, run a clean, build and upload). On Linux/macOS you usually don't need to activate the PlatformIO virtualenv if you installed PlatformIO Core globally or via pip — but the examples include a virtualenv activation step that matches the Windows flow.

### Windows
```powershell
# open PowerShell
cd C:\Users\<User>\.platformio\penv\Scripts\
./activate

cd C:\Users\<User>\Desktop\PlatformIO\robko01_retrofit_fw
pio run --target clean
pio run --environment serial_ps4
pio run --target upload
```

Notes:
- `activate` runs the PlatformIO virtualenv activation script that PlatformIO creates when installed via the recommended installer. On Windows you can also run PlatformIO with the full path to the `platformio.exe` inside the virtualenv.

### Linux
```bash
# open a shell
cd /home/<user>/.platformio/penv/bin
source activate

cd /home/<user>/Desktop/PlatformIO/robko01_retrofit_fw
pio run --target clean
pio run --environment serial_ps4
pio run --target upload
```

Notes:
- If PlatformIO Core is installed globally (e.g. `pipx`, system pip), you can skip the virtualenv activation and run `pio` directly from any shell.
- Replace `<user>` with your Linux username. The virtualenv `activate` script may be named `activate` and lives under `.../penv/bin/activate`.

### macOS
```bash
# open Terminal
cd /Users/<user>/.platformio/penv/bin
source activate

cd /Users/<user>/Desktop/PlatformIO/robko01_retrofit_fw
pio run --target clean
pio run --environment serial_ps4
pio run --target upload
```

Notes:
- macOS commands are identical to Linux in most cases. If you used Homebrew or `pipx` to install PlatformIO, you can run `pio` without activating a virtualenv.
- For upload, ensure your device is connected and the correct environment (`serial_ps4`) is selected in `platformio.ini`.




## Notes

[Doc](https://docs.platformio.org/en/latest/envvars.html) specification how to use environment variables in build time.

[Article](https://community.platformio.org/t/providing-credentials-without-the-keys-being-displayed-in-the-code/32437/3) that describe how to use commands described previous:

