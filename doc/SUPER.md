# SUPER Protocol Interface

This document describes the SUPER protocol interface for the Robko 01 controller.

## Overview

SUPER is a binary control protocol used by the firmware for high-speed robot control.
It can run over UDP, TCP, or serial depending on build flags.

## Configuration

### Build Profiles

SUPER is enabled in these environments:

- `serial_local`
- `serial_remote`
- `serial_home`
- `ota_local`
- `ota_remote`

Build and upload with one of them:

```bash
pio run -e serial_local
pio run -e serial_local -t upload
```

### Default Settings

| Parameter | Default Value | Description |
|-----------|---------------|-------------|
| Transport | UDP | Default transport when `ENABLE_SUPER` is set |
| Service Port | 10182 | SUPER UDP/TCP port |

### Custom Configuration

Override defaults in your environment file or `ApplicationConfiguration.h`:

```ini
build_flags =
  -D ENABLE_SUPER=1
  -D SUPER_SERVICE_PORT=10182
  -D SUPER_UDP
  ; -D SUPER_TCP
  ; -D SUPER_SERIAL
```

Notes:
- UDP/TCP require `ENABLE_WIFI` in the build profile.
- The firmware defaults to `SUPER_UDP` if no transport is selected.

## Transport Behavior

- **UDP**: listens on `SUPER_SERVICE_PORT` and replies to the sender. With `ENABLE_WDT`,
  incoming packets feed the watchdog; a timeout disables motors until new traffic arrives.
- **TCP**: accepts one client at a time. On connect, motors are enabled; on disconnect,
  motors are disabled (when `ENABLE_MOTORS_IO` is enabled).

## Supported OpCodes

Base opcodes are defined in the SUPER framework header `OperationsCodes.h`
(external dependency). The firmware handles these base operations:

- `Ping`
- `Stop`
- `Disable`
- `Enable`
- `Clear`
- `MoveRelative`
- `MoveAbsolute`
- `MoveSpeed`
- `DO` (digital outputs)
- `DI` (digital inputs / limit switches)
- `IsMoving`
- `CurrentPosition`
- `SetRobotID`
- `GetRobotID`

Payload format for motion-related opcodes uses the framework `JointPosition_t`
structure (positions and speeds for 6 joints). Refer to the framework headers
for exact layout and field ordering.

## Extension OpCodes (Firmware-Specific)

Defined in `include/OpCodesExtension.h`:

| OpCode | Name | Payload | Response |
|--------|------|---------|----------|
| 17 | `MoveInterpolated` | 6 x int16 (J1..J6 target steps) | `OK` + duration (uint32 ms) |
| 18 | `MoveIK` | 6 x int16 (X, Y, Z, Pitch, Roll, Gripper) | Defined in header; not handled in `src/main.cpp` |
| 19 | `GetInterpolatorState` | none | `OK` + state + progress (uint16, 0-10000) + 2 bytes reserved |

## Troubleshooting

- No response: confirm `ENABLE_SUPER=1` and a UDP/TCP transport flag in your environment.
- No network traffic: verify `ENABLE_WIFI` and that the device has a valid IP.
- Motors disable unexpectedly: check watchdog settings (`ENABLE_WDT`, `WDT_TIMEOUT`).

## See Also

- `doc/MODBUS_TCP.md`
- `doc/MODBUS_RTU.md`
