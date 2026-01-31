# TCM Command Interface

This document describes the TCM (terminal command mode) serial command interface for the Robko 01 controller.

## Overview

TCM provides a simple ASCII command interface over the main UART. It is intended for direct serial control (e.g., PLCs, HMIs, or a PC terminal).

## Configuration

### Build Profile

Use the `serial_tcm` environment to build firmware with TCM support:

```bash
pio run -e serial_tcm
pio run -e serial_tcm -t upload
```

### Default Settings (serial_tcm)

| Parameter | Default Value | Description |
|-----------|---------------|-------------|
| Baudrate | 9600 | Serial communication speed |
| Data bits | 8 | Number of data bits |
| Parity | None | Parity checking |
| Stop bits | 1 | Number of stop bits |
| WDT timeout | 20000 ms | Watchdog timeout (if ENABLE_WDT) |

The `serial_tcm` profile enables:
`ENABLE_TCM_COMMANDS`, `ENABLE_TCM_GPIO`, `ENABLE_MOTORS`, `ENABLE_MOTORS_IO`,
`ENABLE_LIMITS`, `ENABLE_ESTOP`, `ENABLE_INTERPOLATOR`, and `ENABLE_WDT`.

### Custom Configuration

Override defaults in `platformio_serial_tcm.ini`:

```ini
build_flags =
  -D ENABLE_TCM_COMMANDS=1
  -D ENABLE_INTERPOLATOR=1
  -D WDT_TIMEOUT=20000
  -D LINE_LENGTH=128
```

## Serial Connection

TCM uses the ESP32 main UART (Serial). On Windows, this is typically a COM port (for example `COM7`).

## Command Reference

Commands are ASCII lines terminated with CR/LF. Commas are accepted as separators and are converted to spaces internally.

| Command | Args | Description | Response |
|---------|------|-------------|----------|
| `FREE` | none | Disable motor drivers | `OK` |
| `@CLOSE` | none | Run close routine (uses limit switches if enabled) | `OK` |
| `@READ` | none | Read J1-J6 positions and limit switches state | `J1, J2, J3, J4, J5, J6, Limits` |
| `@RESET` | none | Zero current positions | `OK` |
| `@SET` | `speed` | Set default motor speed used by `@STEP` | `OK` |
| `@STEP` | `speed, J1, J2, J3, J4, J5, J6, DOs` | Set speed and move to target positions (last arg reserved) | `OK` |
| `@MOVEJ` | `J1, J2, J3, J4, J5, J6` | Interpolated joint move (requires `ENABLE_INTERPOLATOR`) | `OK DUR:<ms>` or `ERR:<reason>` |
| `@MOVEIK` | `X, Y, Z, Pitch, Roll, Gripper` | IK + interpolation (requires `ENABLE_INTERPOLATOR`) | `OK DUR:<ms>` or `ERR:<reason>` |

Common `@MOVEJ` / `@MOVEIK` errors:
`INTERP_FAIL`, `NO_MOTORS`, `UNREACHABLE`, `JOINT_LIMIT`, `INVALID_INPUT`.

## Examples (PowerShell)

```powershell
$port = New-Object System.IO.Ports.SerialPort
$port.PortName = "COM7"
$port.BaudRate = 9600
$port.Parity = [System.IO.Ports.Parity]::None
$port.DataBits = 8
$port.StopBits = [System.IO.Ports.StopBits]::One
$port.Open()

# Read current positions
$port.WriteLine("@READ")
Start-Sleep -Milliseconds 200
Write-Host $port.ReadExisting()

# Move all joints (interpolated)
$port.WriteLine("@MOVEJ 100,100,100,0,0,0")
Start-Sleep -Milliseconds 1000
Write-Host $port.ReadExisting()

$port.Close()
```

## Troubleshooting

- No response: verify baudrate, line endings (CR/LF), and that `serial_tcm` is the active build profile.
- Limit switches always 0: ensure `ENABLE_LIMITS` is enabled and wiring is correct.

## See Also

- `doc/MODBUS_RTU.md` for serial Modbus control
