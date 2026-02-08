# Changelog

All notable changes to the Robko 01 Retrofit Firmware project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.62] - 2026-02-08

### Fixed
- Add enable_drivers(true) to Modbus RTU StartMotion handler (register 20)
- Suppress DEBUGLOG output when Modbus RTU uses Serial as communication bus
- Add --boot-wait to test_modbus_rtu.py for ESP32 DTR reset handling
- Remove deprecated method="rtu" param from pymodbus client

## [1.0.61] - 2026-02-01

### Added
- Modbus holding register macro definitions in ApplicationConfiguration.h

### Changed
- Modbus TCP/RTU docs now reference register macro names

## [1.0.60] - 2026-01-31

### Added
- Modbus communication interface using eModbus library
- Modbus TCP server mode (MODBUS_TCP) for network access
- Modbus RTU slave mode (MODBUS_RTU) using main Serial port (UART0)
- New build profiles: modbus_tcp, modbus_rtu
- Modbus register map for motor positions, speeds, and control

## [1.0.59] - 2026-01-31

### Added
- Joint interpolator commands for SUPER protocol
- OpCodesExtension.h with extended operation codes
- MoveInterpolated command (OpCode 17) for synchronized joint motion
- GetInterpolatorState command (OpCode 19) for status queries
- Changelog update rules in CLAUDE.md

### Changed
- Enable ENABLE_INTERPOLATOR in all SUPER-enabled profiles

## [1.0.58] - 2026-01-31

### Added
- Centralized Version.h for firmware version tracking
- CHANGELOG.md with git history synthesis
- Version management rules in CLAUDE.md
- Ignore EnvDumpConfig.h in version control (auto-generated file)
- Build verification for environment dump

## [1.0.56] - 2026-01-31

### Added
- Inject build settings from .env file via pre_build.py

## [1.0.54] - 2026-01-31

### Added
- Interpolator library for synchronized multi-joint motion
- `@MOVEJ` command for interpolated joint movements
- `@MOVEIK` command for inverse kinematics movements
- ENABLE_INTERPOLATOR feature flag
- claude_tests folder for hardware test scripts
- Test script for interpolator commands

## [1.0.47] - 2026-01-27

### Removed
- ENABLE_SHMR code and configuration (cleanup)
- Ethernet (ETH) configuration and includes
- ENABLE_FEATURES_FLAGS functionality
- soft_features profile configuration

### Fixed
- platformio.ini syntax error
- PS4_MAC configuration handling
- gitignore to properly track PlatformIO lib folder

## [1.0.39] - 2026-01-18

### Added
- .env documentation and improved pre_build.py
- Testing and approval rules to Git workflow documentation
- Complete Git workflow documentation in CLAUDE.md

### Changed
- Integrated WDT (Watchdog Timer) with TCM commands
- Refactored motor speed handling for safety
- Enabled limits, estop, wdt and gpio for serial_tcm profile

## [1.0.32] - 2026-01-17

### Added
- CLAUDE.md project guidelines for Claude Code assistance

## [1.0.31] - 2025-10-13

### Added
- Empty Ethernet profile for future ENC28J60 support
- Headers for ENC28J60 Ethernet support

### Changed
- Updated readme file

## [1.0.28] - 2025-10-03

### Added
- Build name and version display in device properties
- MAC address display for PS4 controller example
- Validation for PS4 MAC address (blocks invalid addresses)

### Removed
- Firmware version from previous location (consolidated)

## [1.0.25] - 2025-09-08

### Added
- WireGuard VPN section and configuration

## [1.0.24] - 2025-09-03

### Changed
- Commented unused variables in build process
- Added notes about environment variables usage

## [1.0.22] - 2025-06-12

### Added
- Main application code
- Initial codebase structure

## [1.0.19] - 2025-06-11

### Added
- Main README file
- .gitignore configuration

## [1.0.17] - 2025-06-10

### Added
- Initial commit - project foundation

---

## Version History Summary

### Major Features by Release

| Version | Date | Key Features |
|---------|------|--------------|
| 1.0.58 | 2026-01-31 | EnvDump config management |
| 1.0.54 | 2026-01-31 | Multi-joint interpolation, inverse kinematics |
| 1.0.47 | 2026-01-27 | Code cleanup, removed SHMR |
| 1.0.39 | 2026-01-18 | WDT integration, improved safety |
| 1.0.32 | 2026-01-17 | Claude Code guidelines |
| 1.0.31 | 2025-10-13 | Ethernet preparation |
| 1.0.28 | 2025-10-03 | Build versioning, PS4 validation |
| 1.0.25 | 2025-09-08 | WireGuard VPN support |
| 1.0.22 | 2025-06-12 | Core application |
| 1.0.17 | 2025-06-10 | Project initialization |

### Feature Modules

The firmware supports the following configurable features (enabled via build flags):

- **ENABLE_MOTORS** - Stepper motor control for 6 axes
- **ENABLE_MOTORS_IO** - Motor GPIO pin definitions
- **ENABLE_LIMITS** - Limit switch detection and homing
- **ENABLE_ESTOP** - Emergency stop functionality
- **ENABLE_WIFI** - WiFi connectivity
- **ENABLE_MDNS** - mDNS service discovery
- **ENABLE_NTP** - Network time synchronization
- **ENABLE_WG** - WireGuard VPN support
- **ENABLE_OTA** - Over-the-air firmware updates
- **ENABLE_SUPER** - Super protocol (TCP/UDP/Serial)
- **ENABLE_TCM_COMMANDS** - Terminal command interface
- **ENABLE_WDT** - Watchdog timer for safety
- **ENABLE_STATUS_LCD** - I2C LCD status display
- **ENABLE_PS4** - PS4 controller support
- **ENABLE_INTERPOLATOR** - Synchronized multi-joint motion
