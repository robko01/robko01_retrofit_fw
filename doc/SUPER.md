# SUPER Protocol Interface

This document describes the SUPER protocol interface for the Robko 01 controller.

## Overview

**SUPER** stands for:
- **S** - Synchronous
- **U** - Unified
- **P** - Protocol for
- **E** - Extendable
- **R** - Robots

SUPER is a lightweight binary control protocol designed for high-speed, real-time robot control. It provides a simple request-response communication model with CRC validation for data integrity.

### Key Characteristics

| Feature | Description |
|---------|-------------|
| Type | Binary protocol (not text-based) |
| Model | Request-response |
| Integrity | CRC-16 validation on all frames |
| Transports | UDP, TCP, or Serial |
| Max payload | 26 bytes |
| Byte order | Little-endian |

## Frame Structure

All SUPER communication uses a fixed frame format. Both requests and responses follow the same structure.

### Frame Format

```
┌──────────┬──────────┬────────┬────────┬─────────────┬───────────┐
│ Sentinel │ FrameType│ Length │ OpCode │   Payload   │   CRC-16  │
│  1 byte  │  1 byte  │ 1 byte │ 1 byte │  0-26 bytes │  2 bytes  │
└──────────┴──────────┴────────┴────────┴─────────────┴───────────┘
     0xAA     0x01/02    N+3      0x01+     [data]      [crc_lo][crc_hi]
```

### Frame Fields

| Offset | Field | Size | Description |
|--------|-------|------|-------------|
| 0 | Sentinel | 1 byte | Always `0xAA` - marks the start of a frame |
| 1 | FrameType | 1 byte | `0x01` = Request, `0x02` = Response |
| 2 | Length | 1 byte | Total bytes from OpCode to end of CRC (inclusive) |
| 3 | OpCode | 1 byte | Operation code (see OpCodes section) |
| 4 | StatusCode | 1 byte | **Response only** - status of the operation |
| 4/5 | Payload | 0-26 bytes | Command-specific data |
| N-2 | CRC-16 | 2 bytes | CRC of bytes from Sentinel through Payload |

### Frame Constraints

- **Minimum frame length**: 6 bytes (sentinel + type + length + opcode + crc)
- **Maximum frame length**: 32 bytes
- **Maximum payload length**: 26 bytes

### Request Frame

```
┌──────────┬──────────┬────────┬────────┬─────────────┬───────────┐
│   0xAA   │   0x01   │ Length │ OpCode │   Payload   │   CRC-16  │
└──────────┴──────────┴────────┴────────┴─────────────┴───────────┘
```

### Response Frame

Response frames include a StatusCode byte after the OpCode:

```
┌──────────┬──────────┬────────┬────────┬────────────┬─────────────┬───────────┐
│   0xAA   │   0x02   │ Length │ OpCode │ StatusCode │   Payload   │   CRC-16  │
└──────────┴──────────┴────────┴────────┴────────────┴─────────────┴───────────┘
```

### Status Codes

| Value | Name | Description |
|-------|------|-------------|
| 1 | `Ok` | Operation completed successfully |
| 2 | `Error` | An error occurred (e.g., invalid parameters) |
| 3 | `Busy` | Robot is busy with another operation |
| 4 | `TimeOut` | Operation timed out |

### CRC Calculation

The protocol uses CRC-16 for frame validation. The CRC is calculated over all bytes from the Sentinel through the end of the Payload (excluding the CRC bytes themselves). The CRC is transmitted in little-endian order (low byte first).

## Communication Flow

### Request-Response Sequence

```
Client                                Server (Robot)
   │                                       │
   │  ────── Request Frame ──────────────► │
   │                                       │  Process command
   │  ◄───── Response Frame ───────────── │
   │                                       │
```

### Frame Parsing State Machine

The receiver parses incoming bytes using this state machine:

```
      ┌─────────────────────────────────────────┐
      │                                         │
      ▼                                         │
 ┌─────────┐  0xAA   ┌─────────┐  type   ┌──────┴──┐
 │ Sentinel│────────►│FrameType│────────►│ Length  │
 └─────────┘         └─────────┘         └────┬────┘
      ▲                                       │
      │ invalid                               ▼
      │                                  ┌─────────┐
      └──────────────────────────────────│ OpCode  │
                                         └────┬────┘
                ┌───────────────────────────┐ │
                │                           ▼ ▼
           ┌────┴──┐  all bytes   ┌──────────────┐
           │  CRC  │◄─────────────│    Data      │
           └───────┘   received   └──────────────┘
```

## JointPosition Payload Structure

Many motion commands use the `JointPosition_t` structure for sending position and speed data for all 6 joints.

### Structure Layout (24 bytes)

```c
typedef struct __attribute__((packed)) {
    int16_t BasePos;        // Bytes 0-1:   Base joint position [steps]
    int16_t BaseSpeed;      // Bytes 2-3:   Base joint speed [steps/sec]
    int16_t ShoulderPos;    // Bytes 4-5:   Shoulder joint position [steps]
    int16_t ShoulderSpeed;  // Bytes 6-7:   Shoulder joint speed [steps/sec]
    int16_t ElbowPos;       // Bytes 8-9:   Elbow joint position [steps]
    int16_t ElbowSpeed;     // Bytes 10-11: Elbow joint speed [steps/sec]
    int16_t LeftDiffPos;    // Bytes 12-13: Left Differential position [steps]
    int16_t LeftDiffSpeed;  // Bytes 14-15: Left Differential speed [steps/sec]
    int16_t RightDiffPos;   // Bytes 16-17: Right Differential position [steps]
    int16_t RightDiffSpeed; // Bytes 18-19: Right Differential speed [steps/sec]
    int16_t GripperPos;     // Bytes 20-21: Gripper position [steps]
    int16_t GripperSpeed;   // Bytes 22-23: Gripper speed [steps/sec]
} JointPosition_t;
```

### Joint Mapping

| Index | Name | Description |
|-------|------|-------------|
| J1 | Base | Rotation of the entire arm |
| J2 | Shoulder | First arm segment angle |
| J3 | Elbow | Second arm segment angle |
| J4 | LeftDiff | Left differential (pitch/roll mechanism) |
| J5 | RightDiff | Right differential (pitch/roll mechanism) |
| J6 | Gripper | End effector grip position |

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

SUPER can operate over three different transports. Only one transport can be active at a time.

### UDP Transport (`SUPER_UDP`)

```
┌─────────────────┐                    ┌─────────────────┐
│     Client      │                    │  Robot (ESP32)  │
│                 │  UDP Datagram      │                 │
│   Port: any     │ ──────────────────►│   Port: 10182   │
│                 │  Request Frame     │                 │
│                 │                    │                 │
│                 │  UDP Datagram      │                 │
│                 │ ◄──────────────────│                 │
│                 │  Response Frame    │                 │
└─────────────────┘                    └─────────────────┘
```

- Listens on `SUPER_SERVICE_PORT` (default 10182)
- Connectionless - each request/response is independent
- Replies are sent to the sender's IP and port
- **Watchdog integration**: When `ENABLE_WDT` is defined, incoming packets feed the watchdog timer. If no packets arrive within `WDT_TIMEOUT`, motors are automatically disabled for safety.

### TCP Transport (`SUPER_TCP`)

```
┌─────────────────┐                    ┌─────────────────┐
│     Client      │                    │  Robot (ESP32)  │
│                 │  TCP Connect       │                 │
│                 │ ══════════════════►│   Port: 10182   │
│                 │                    │  Motors Enable  │
│                 │  Request Frame     │                 │
│                 │ ──────────────────►│                 │
│                 │  Response Frame    │                 │
│                 │ ◄──────────────────│                 │
│                 │        ...         │                 │
│                 │  TCP Disconnect    │                 │
│                 │ ══════════════════►│  Motors Disable │
└─────────────────┘                    └─────────────────┘
```

- Listens on `SUPER_SERVICE_PORT` (default 10182)
- Accepts only **one client at a time**
- Connection-oriented with automatic motor control:
  - **On connect**: Motors are enabled (when `ENABLE_MOTORS_IO` is defined)
  - **On disconnect**: Motors are disabled for safety
- Provides reliable, ordered delivery

### Serial Transport (`SUPER_SERIAL`)

```
┌─────────────────┐                    ┌─────────────────┐
│     Client      │                    │  Robot (ESP32)  │
│                 │  Serial TX         │                 │
│   USB/UART      │ ──────────────────►│   Serial RX     │
│                 │  Request Frame     │                 │
│                 │                    │                 │
│                 │  Serial RX         │                 │
│                 │ ◄──────────────────│   Serial TX     │
│                 │  Response Frame    │                 │
└─────────────────┘                    └─────────────────┘
```

- Uses hardware serial port
- Baud rate configured in PlatformIO environment
- No automatic motor enable/disable on connection

### Transport Selection Priority

If multiple transports are defined, the firmware uses this priority:
1. `SUPER_TCP` (if defined)
2. `SUPER_UDP` (if defined)
3. `SUPER_SERIAL` (if defined)
4. `SUPER_UDP` (default fallback)

## Supported OpCodes

### Base OpCodes (1-16)

These are defined in the framework library's `OperationsCodes.h`.

| OpCode | Name | Request Payload | Response Payload | Description |
|--------|------|-----------------|------------------|-------------|
| 1 | `Ping` | Echo data (0-25 bytes) | Echo data back | Connection test, returns same data |
| 2 | `Stop` | None | None | Emergency stop, halts all motion immediately |
| 3 | `Disable` | None | None | Disable motor drivers (de-energize) |
| 4 | `Enable` | None | None | Enable motor drivers (energize) |
| 5 | `Clear` | None | None | Reset position counters to zero (home position) |
| 6 | `MoveRelative` | `JointPosition_t` (24 bytes) | None or busy axis (1 byte) | Move by relative offset from current position |
| 7 | `MoveAbsolute` | `JointPosition_t` (24 bytes) | None or busy axis (1 byte) | Move to absolute position |
| 8 | `DO` | 1 byte (bitmask) | 1 byte (new state) | Set digital outputs |
| 9 | `DI` | None | 1 byte (input state) | Read digital inputs (limit switches) |
| 10 | `IsMoving` | None | 1 byte (0=idle, 1=moving) | Check if robot is in motion |
| 11 | `CurrentPosition` | None | `JointPosition_t` (24 bytes) | Get current joint positions |
| 12 | `MoveSpeed` | `JointPosition_t` (24 bytes) | None | Continuous velocity mode |
| 13 | `SetRobotID` | ID string | ID string | Store robot identifier |
| 14 | `GetRobotID` | None | ID string | Retrieve robot identifier |

### Detailed OpCode Descriptions

#### Ping (OpCode 1)

Tests connectivity. Any payload bytes sent are echoed back in the response.

```
Request:  AA 01 04 01 [payload] [crc]
Response: AA 02 05 01 01 [payload] [crc]
                     │
                     └── StatusCode: Ok
```

#### Stop (OpCode 2)

Immediately halts all motor motion. Use in emergency situations.

```
Request:  AA 01 03 02 [crc]
Response: AA 02 04 02 01 [crc]
```

#### Enable/Disable (OpCodes 3, 4)

Controls the motor driver enable signal. When disabled, motors are free to move manually.

```
Enable:  AA 01 03 04 [crc]  →  AA 02 04 04 01 [crc]
Disable: AA 01 03 03 [crc]  →  AA 02 04 03 01 [crc]
```

#### MoveRelative (OpCode 6)

Moves each joint by a relative offset. Payload uses `JointPosition_t` structure.
If any joint is still moving, returns `Busy` status with the busy axis index.

```
Request:  AA 01 1B 06 [JointPosition_t: 24 bytes] [crc]
Response: AA 02 04 06 01 [crc]           ← Success
Response: AA 02 05 06 03 [axis] [crc]    ← Busy (axis still moving)
```

#### MoveAbsolute (OpCode 7)

Moves joints to absolute step positions. Same payload format as MoveRelative.

#### CurrentPosition (OpCode 11)

Returns current position of all joints in `JointPosition_t` format.

```
Request:  AA 01 03 0B [crc]
Response: AA 02 1C 0B 01 [JointPosition_t: 24 bytes] [crc]
```

#### MoveSpeed (OpCode 12)

Continuous velocity mode - joints move at specified speeds until stopped.
Speed values are in steps per second.

### Extension OpCodes (17+)

Firmware-specific extensions defined in `include/OpCodesExtension.h`.

| OpCode | Name | Request Payload | Response Payload | Description |
|--------|------|-----------------|------------------|-------------|
| 17 | `MoveInterpolated` | 6 × int16 (12 bytes) | uint32 duration (4 bytes) | Synchronized multi-joint motion |
| 18 | `MoveIK` | 6 × int16 (12 bytes) | uint32 duration (4 bytes) | Inverse kinematics move (reserved) |
| 19 | `GetInterpolatorState` | None | state + progress (5 bytes) | Query interpolator status |

#### MoveInterpolated (OpCode 17)

Performs synchronized motion where all joints start and finish simultaneously using trapezoidal velocity profiles.

**Request Payload (12 bytes):**
```
┌────────────┬────────────┬────────────┬────────────┬────────────┬────────────┐
│   J1 Pos   │   J2 Pos   │   J3 Pos   │   J4 Pos   │   J5 Pos   │   J6 Pos   │
│  int16_t   │  int16_t   │  int16_t   │  int16_t   │  int16_t   │  int16_t   │
└────────────┴────────────┴────────────┴────────────┴────────────┴────────────┘
  Base         Shoulder     Elbow        LeftDiff     RightDiff    Gripper
  [steps]      [steps]      [steps]      [steps]      [steps]      [steps]
```

**Response Payload (4 bytes):**
```
┌────────────────────────────────────────────────────────────────┐
│                        Duration (ms)                           │
│                          uint32_t                              │
└────────────────────────────────────────────────────────────────┘
```

#### MoveIK (OpCode 18)

Reserved for inverse kinematics motion (Cartesian coordinates).

**Request Payload (12 bytes):**
```
┌────────────┬────────────┬────────────┬────────────┬────────────┬────────────┐
│     X      │     Y      │     Z      │   Pitch    │    Roll    │  Gripper   │
│  int16_t   │  int16_t   │  int16_t   │  int16_t   │  int16_t   │  int16_t   │
└────────────┴────────────┴────────────┴────────────┴────────────┴────────────┘
  [mm×10]      [mm×10]      [mm×10]      [deg×10]     [deg×10]     [steps]
```

#### GetInterpolatorState (OpCode 19)

Queries the current state of the motion interpolator.

**Response Payload (5 bytes):**
```
┌────────────┬─────────────────────────────────────────────────────┐
│   State    │                    Progress                         │
│   uint8_t  │                   float (4 bytes)                   │
└────────────┴─────────────────────────────────────────────────────┘
     │
     └── 0=Idle, 1=Accelerating, 2=Cruising, 3=Decelerating, 4=Complete
```

Progress is returned as a float (0.0 to 100.0) representing percentage complete.

## Communication Examples

### Example 1: Ping

Check connection with echo data `0x55`:

```
TX: AA 01 04 01 55 XX XX        (Request: Ping with payload 0x55)
RX: AA 02 05 01 01 55 XX XX     (Response: Ok, echo 0x55)
```

### Example 2: Enable Motors

```
TX: AA 01 03 04 XX XX           (Request: Enable)
RX: AA 02 04 04 01 XX XX        (Response: Ok)
```

### Example 3: Move Joint 1 to Position 1000

Using MoveAbsolute with only J1 position set:

```
TX: AA 01 1B 07                 (Request: MoveAbsolute)
    E8 03                       (J1 Pos = 1000, little-endian)
    00 00                       (J1 Speed = 0, use default)
    00 00 00 00                 (J2 Pos, Speed)
    00 00 00 00                 (J3 Pos, Speed)
    00 00 00 00                 (J4 Pos, Speed)
    00 00 00 00                 (J5 Pos, Speed)
    00 00 00 00                 (J6 Pos, Speed)
    XX XX                       (CRC)
RX: AA 02 04 07 01 XX XX        (Response: Ok)
```

### Example 4: Check if Moving

```
TX: AA 01 03 0A XX XX           (Request: IsMoving)
RX: AA 02 05 0A 01 00 XX XX     (Response: Ok, not moving)
RX: AA 02 05 0A 01 01 XX XX     (Response: Ok, moving)
```

### Example 5: Interpolated Move

Move J1=500, J2=300, J3=-200, J4=0, J5=0, J6=100:

```
TX: AA 01 0F 11                 (Request: MoveInterpolated)
    F4 01                       (J1 = 500)
    2C 01                       (J2 = 300)
    38 FF                       (J3 = -200, two's complement)
    00 00                       (J4 = 0)
    00 00                       (J5 = 0)
    64 00                       (J6 = 100)
    XX XX                       (CRC)
RX: AA 02 08 11 01              (Response: Ok)
    D0 07 00 00                 (Duration = 2000ms)
    XX XX                       (CRC)
```

## Troubleshooting

### No Response from Robot

| Symptom | Possible Cause | Solution |
|---------|----------------|----------|
| No response at all | SUPER not enabled | Add `-D ENABLE_SUPER=1` to build flags |
| No response at all | Wrong transport | Verify `SUPER_UDP`, `SUPER_TCP`, or `SUPER_SERIAL` is defined |
| No response at all | Wrong port | Check `SUPER_SERVICE_PORT` (default 10182) |
| CRC error responses | Incorrect CRC calculation | Verify CRC-16 algorithm and byte order |
| Frame not recognized | Missing sentinel | Ensure frame starts with `0xAA` |

### Network Issues (UDP/TCP)

| Symptom | Possible Cause | Solution |
|---------|----------------|----------|
| No network traffic | WiFi not enabled | Add `-D ENABLE_WIFI=1` to build flags |
| No network traffic | No IP address | Check WiFi credentials and connection status |
| UDP packets lost | Network congestion | Reduce command rate or switch to TCP |
| TCP connection refused | Client already connected | Disconnect other clients (TCP allows only one) |

### Motor Issues

| Symptom | Possible Cause | Solution |
|---------|----------------|----------|
| Motors disable unexpectedly | Watchdog timeout | Send commands more frequently or increase `WDT_TIMEOUT` |
| Motors won't enable | TCP disconnected | Reconnect TCP client |
| Motors won't enable | E-stop active | Clear E-stop condition and send Enable command |
| `Busy` status returned | Motion in progress | Wait for motion to complete or send Stop first |

### Debugging Tips

1. **Enable debug output**: Add `-D SHOW_FUNC_NAMES=1` to see function calls on serial monitor
2. **Check frame parsing**: Monitor serial output for frame reception states
3. **Verify byte order**: SUPER uses little-endian for multi-byte values
4. **Test with Ping**: Use OpCode 1 (Ping) to verify basic communication before complex commands

## See Also

- `doc/MODBUS_TCP.md`
- `doc/MODBUS_RTU.md`
