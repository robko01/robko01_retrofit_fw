# SUPER Protocol Interface

This document describes the SUPER protocol interface for the Robko 01 controller.

## Overview

**SUPER** stands for:
- **S** - Synchronous
- **U** - Unified
- **P** - Protocol for
- **E** - Extendable
- **R** - Robots

SUPER is a lightweight binary control protocol designed for high-speed, real-time robot control. It provides a simple request-response communication model with checksum validation for data integrity.

### Key Characteristics

| Feature | Description |
|---------|-------------|
| Type | Binary protocol (not text-based) |
| Model | Request-response |
| Integrity | XOR checksum validation on all frames |
| Transports | UDP, TCP, or Serial |
| Max payload | 26 bytes |
| Byte order | Little-endian |

## Frame Structure

All SUPER communication uses a fixed frame format. Both requests and responses follow the same structure.

### Frame Format

```
┌──────────┬──────────┬────────┬────────┬─────────────┬───────────┐
│ Sentinel │ FrameType│ Length │ OpCode │   Payload   │ Checksum  │
│  1 byte  │  1 byte  │ 1 byte │ 1 byte │  0-26 bytes │  2 bytes  │
└──────────┴──────────┴────────┴────────┴─────────────┴───────────┘
     0xAA     0x01/02    N+3      0x01+     [data]      [chk0][chk1]
```

### Frame Fields

| Offset | Field | Size | Description |
|--------|-------|------|-------------|
| 0 | Sentinel | 1 byte | Always `0xAA` - marks the start of a frame |
| 1 | FrameType | 1 byte | `0x01` = Request, `0x02` = Response |
| 2 | Length | 1 byte | Total bytes from OpCode to end of Checksum (inclusive) |
| 3 | OpCode | 1 byte | Operation code (see OpCodes section) |
| 4 | StatusCode | 1 byte | **Response only** - status of the operation |
| 4/5 | Payload | 0-26 bytes | Command-specific data |
| N-2 | Checksum | 2 bytes | XOR checksum of bytes from Sentinel through Payload |

### Frame Constraints

- **Minimum frame length**: 6 bytes (sentinel + type + length + opcode + checksum)
- **Maximum frame length**: 32 bytes
- **Maximum payload length**: 26 bytes

### Request Frame

```
┌──────────┬──────────┬────────┬────────┬─────────────┬───────────┐
│   0xAA   │   0x01   │ Length │ OpCode │   Payload   │ Checksum  │
└──────────┴──────────┴────────┴────────┴─────────────┴───────────┘
```

### Response Frame

Response frames include a StatusCode byte after the OpCode:

```
┌──────────┬──────────┬────────┬────────┬────────────┬─────────────┬───────────┐
│   0xAA   │   0x02   │ Length │ OpCode │ StatusCode │   Payload   │ Checksum  │
└──────────┴──────────┴────────┴────────┴────────────┴─────────────┴───────────┘
```

### Status Codes

| Value | Name | Description |
|-------|------|-------------|
| 1 | `Ok` | Operation completed successfully |
| 2 | `Error` | An error occurred (e.g., invalid parameters) |
| 3 | `Busy` | Robot is busy with another operation |
| 4 | `TimeOut` | Operation timed out |

### Checksum Calculation

The protocol uses a simple XOR checksum for frame validation. The checksum consists of 2 bytes calculated over all bytes from the Sentinel through the end of the Payload (excluding the checksum bytes themselves):

- **Byte 0**: XOR of all even-indexed bytes (indices 0, 2, 4, ...)
- **Byte 1**: XOR of all odd-indexed bytes (indices 1, 3, 5, ...)

```
Frame:    [AA] [01] [03] [04]
Index:      0    1    2    3

Checksum[0] = 0xAA ^ 0x03 = 0xA9  (indices 0, 2)
Checksum[1] = 0x01 ^ 0x04 = 0x05  (indices 1, 3)
```

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
           │ Chksum│◄─────────────│    Data      │
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
Request:  AA 01 04 01 [payload] [chk]
Response: AA 02 05 01 01 [payload] [chk]
                     │
                     └── StatusCode: Ok
```

#### Stop (OpCode 2)

Immediately halts all motor motion. Use in emergency situations.

```
Request:  AA 01 03 02 [chk]
Response: AA 02 04 02 01 [chk]
```

#### Enable/Disable (OpCodes 3, 4)

Controls the motor driver enable signal. When disabled, motors are free to move manually.

```
Enable:  AA 01 03 04 [chk]  →  AA 02 04 04 01 [chk]
Disable: AA 01 03 03 [chk]  →  AA 02 04 03 01 [chk]
```

#### MoveRelative (OpCode 6)

Moves each joint by a relative offset. Payload uses `JointPosition_t` structure.
If any joint is still moving, returns `Busy` status with the busy axis index.

```
Request:  AA 01 1B 06 [JointPosition_t: 24 bytes] [chk]
Response: AA 02 04 06 01 [chk]           ← Success
Response: AA 02 05 06 03 [axis] [chk]    ← Busy (axis still moving)
```

#### MoveAbsolute (OpCode 7)

Moves joints to absolute step positions. Same payload format as MoveRelative.

#### CurrentPosition (OpCode 11)

Returns current position of all joints in `JointPosition_t` format.

```
Request:  AA 01 03 0B [chk]
Response: AA 02 1C 0B 01 [JointPosition_t: 24 bytes] [chk]
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
    XX XX                       (Checksum)
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
    XX XX                       (Checksum)
RX: AA 02 08 11 01              (Response: Ok)
    D0 07 00 00                 (Duration = 2000ms)
    XX XX                       (Checksum)
```

## Troubleshooting

### No Response from Robot

| Symptom | Possible Cause | Solution |
|---------|----------------|----------|
| No response at all | SUPER not enabled | Add `-D ENABLE_SUPER=1` to build flags |
| No response at all | Wrong transport | Verify `SUPER_UDP`, `SUPER_TCP`, or `SUPER_SERIAL` is defined |
| No response at all | Wrong port | Check `SUPER_SERVICE_PORT` (default 10182) |
| Checksum error responses | Incorrect checksum calculation | Verify XOR checksum algorithm |
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

## Python Examples

### Checksum Calculation

The SUPER protocol uses a simple XOR checksum (not CRC):

```python
def checksum(data: bytes) -> bytes:
    """Calculate XOR checksum for SUPER protocol.

    Byte 0: XOR of all even-indexed bytes (0, 2, 4, ...)
    Byte 1: XOR of all odd-indexed bytes (1, 3, 5, ...)
    """
    chk = [0, 0]
    for i, byte in enumerate(data):
        chk[i % 2] ^= byte
    return bytes(chk)
```

### SUPER Client Class (UDP)

```python
import socket
import struct

class SUPERClient:
    """SUPER protocol UDP client for Robko 01 robot."""

    SENTINEL = 0xAA
    REQUEST = 0x01
    RESPONSE = 0x02

    # OpCodes
    OP_PING = 1
    OP_STOP = 2
    OP_DISABLE = 3
    OP_ENABLE = 4
    OP_CLEAR = 5
    OP_MOVE_RELATIVE = 6
    OP_MOVE_ABSOLUTE = 7
    OP_DO = 8
    OP_DI = 9
    OP_IS_MOVING = 10
    OP_CURRENT_POSITION = 11
    OP_MOVE_SPEED = 12
    OP_MOVE_INTERPOLATED = 17
    OP_GET_INTERPOLATOR_STATE = 19

    # Status codes
    STATUS_OK = 1
    STATUS_ERROR = 2
    STATUS_BUSY = 3
    STATUS_TIMEOUT = 4

    def __init__(self, host: str, port: int = 10182, timeout: float = 1.0):
        self.host = host
        self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.settimeout(timeout)

    def _checksum(self, data: bytes) -> bytes:
        """Calculate XOR checksum.

        Byte 0: XOR of all even-indexed bytes
        Byte 1: XOR of all odd-indexed bytes
        """
        chk = [0, 0]
        for i, byte in enumerate(data):
            chk[i % 2] ^= byte
        return bytes(chk)

    def _build_frame(self, opcode: int, payload: bytes = b'') -> bytes:
        """Build a SUPER request frame."""
        length = len(payload) + 3  # opcode + payload + 2 checksum bytes
        header = bytes([self.SENTINEL, self.REQUEST, length, opcode])
        frame_without_checksum = header + payload
        chk = self._checksum(frame_without_checksum)
        return frame_without_checksum + chk

    def _send_receive(self, opcode: int, payload: bytes = b'') -> tuple:
        """Send request and receive response."""
        frame = self._build_frame(opcode, payload)
        self.sock.sendto(frame, (self.host, self.port))

        response, _ = self.sock.recvfrom(64)

        # Parse response
        if len(response) < 6 or response[0] != self.SENTINEL:
            raise ValueError("Invalid response frame")

        resp_opcode = response[3]
        status = response[4]
        resp_payload = response[5:-2]  # Exclude checksum

        return status, resp_opcode, resp_payload

    def ping(self, data: bytes = b'\x55') -> bool:
        """Test connection."""
        status, _, payload = self._send_receive(self.OP_PING, data)
        return status == self.STATUS_OK and payload == data

    def enable(self) -> bool:
        """Enable motor drivers."""
        status, _, _ = self._send_receive(self.OP_ENABLE)
        return status == self.STATUS_OK

    def disable(self) -> bool:
        """Disable motor drivers."""
        status, _, _ = self._send_receive(self.OP_DISABLE)
        return status == self.STATUS_OK

    def stop(self) -> bool:
        """Emergency stop."""
        status, _, _ = self._send_receive(self.OP_STOP)
        return status == self.STATUS_OK

    def clear(self) -> bool:
        """Reset position counters to zero."""
        status, _, _ = self._send_receive(self.OP_CLEAR)
        return status == self.STATUS_OK

    def is_moving(self) -> bool:
        """Check if robot is in motion."""
        status, _, payload = self._send_receive(self.OP_IS_MOVING)
        if status == self.STATUS_OK and len(payload) >= 1:
            return payload[0] == 1
        return False

    def get_position(self) -> dict:
        """Get current joint positions."""
        status, _, payload = self._send_receive(self.OP_CURRENT_POSITION)
        if status == self.STATUS_OK and len(payload) >= 24:
            values = struct.unpack('<6h6h', payload[:24])
            return {
                'j1': {'pos': values[0], 'speed': values[1]},
                'j2': {'pos': values[2], 'speed': values[3]},
                'j3': {'pos': values[4], 'speed': values[5]},
                'j4': {'pos': values[6], 'speed': values[7]},
                'j5': {'pos': values[8], 'speed': values[9]},
                'j6': {'pos': values[10], 'speed': values[11]},
            }
        return None

    def move_absolute(self, j1=0, j2=0, j3=0, j4=0, j5=0, j6=0,
                      s1=100, s2=100, s3=100, s4=100, s5=100, s6=100) -> bool:
        """Move to absolute positions."""
        payload = struct.pack('<12h', j1, s1, j2, s2, j3, s3, j4, s4, j5, s5, j6, s6)
        status, _, _ = self._send_receive(self.OP_MOVE_ABSOLUTE, payload)
        return status == self.STATUS_OK

    def move_interpolated(self, j1=0, j2=0, j3=0, j4=0, j5=0, j6=0) -> int:
        """Move with synchronized interpolation. Returns duration in ms."""
        payload = struct.pack('<6h', j1, j2, j3, j4, j5, j6)
        status, _, resp = self._send_receive(self.OP_MOVE_INTERPOLATED, payload)
        if status == self.STATUS_OK and len(resp) >= 4:
            return struct.unpack('<I', resp[:4])[0]
        return -1

    def get_interpolator_state(self) -> dict:
        """Get interpolator state and progress."""
        status, _, payload = self._send_receive(self.OP_GET_INTERPOLATOR_STATE)
        if status == self.STATUS_OK and len(payload) >= 5:
            state = payload[0]
            progress = struct.unpack('<f', payload[1:5])[0]
            state_names = ['Idle', 'Accelerating', 'Cruising', 'Decelerating', 'Complete']
            return {
                'state': state,
                'state_name': state_names[state] if state < len(state_names) else 'Unknown',
                'progress': progress
            }
        return None

    def close(self):
        """Close the socket."""
        self.sock.close()
```

### Usage Examples

```python
# Connect to robot
robot = SUPERClient('192.168.1.100', port=10182)

# Test connection
if robot.ping():
    print("Robot connected!")

# Enable motors
robot.enable()

# Get current position
pos = robot.get_position()
print(f"J1 position: {pos['j1']['pos']} steps")

# Move to absolute position
robot.move_absolute(j1=1000, j2=500, j3=-200)

# Wait for motion to complete
import time
while robot.is_moving():
    time.sleep(0.1)

# Synchronized interpolated move
duration = robot.move_interpolated(j1=500, j2=300, j3=-100, j4=0, j5=0, j6=50)
print(f"Motion will take {duration} ms")

# Monitor interpolation progress
while True:
    state = robot.get_interpolator_state()
    print(f"State: {state['state_name']}, Progress: {state['progress']:.1f}%")
    if state['state_name'] in ['Idle', 'Complete']:
        break
    time.sleep(0.05)

# Disable motors and close
robot.disable()
robot.close()
```

### TCP Client Variant

```python
class SUPERClientTCP(SUPERClient):
    """SUPER protocol TCP client."""

    def __init__(self, host: str, port: int = 10182, timeout: float = 1.0):
        self.host = host
        self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(timeout)
        self.sock.connect((host, port))

    def _send_receive(self, opcode: int, payload: bytes = b'') -> tuple:
        """Send request and receive response over TCP."""
        frame = self._build_frame(opcode, payload)
        self.sock.sendall(frame)

        response = self.sock.recv(64)

        if len(response) < 6 or response[0] != self.SENTINEL:
            raise ValueError("Invalid response frame")

        resp_opcode = response[3]
        status = response[4]
        resp_payload = response[5:-2]

        return status, resp_opcode, resp_payload
```

## See Also

- `doc/MODBUS_TCP.md`
- `doc/MODBUS_RTU.md`
