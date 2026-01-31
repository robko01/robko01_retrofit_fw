# TCM Protocol Interface

This document describes the TCM (Text Command Mode) protocol interface for the Robko 01 controller.

## Overview

**TCM** (Text Command Mode) is a human-readable, text-based protocol for controlling the Robko 01 robot. It uses simple ASCII commands over a serial connection, making it easy to test and debug without specialized tools.

### Key Characteristics

| Feature | Description |
|---------|-------------|
| Type | Text-based ASCII protocol |
| Transport | Serial (USB/UART) |
| Line ending | `\r\n` (CRLF) |
| Response format | `\r\n<data>\r\n` |
| Baud rate | 115200 (configurable) |

### Comparison with SUPER

| Feature | TCM | SUPER |
|---------|-----|-------|
| Format | Text (ASCII) | Binary |
| Speed | Slower (parsing overhead) | Faster |
| Debugging | Easy (human-readable) | Requires tools |
| Payload size | Variable | Fixed structures |
| CRC | None | CRC-16 |

## Configuration

### Build Profile

TCM is enabled in the `serial_tcm` environment:

```bash
pio run -e serial_tcm
pio run -e serial_tcm -t upload
```

### Build Flags

```ini
build_flags =
  -D ENABLE_TCM_COMMANDS=1
  -D ENABLE_MOTORS=1
  -D ENABLE_INTERPOLATOR=1  ; For @MOVEJ and @MOVEIK
```

## Command Format

Commands follow this format:

```
<COMMAND> [arg1] [arg2] ... [argN]\r\n
```

- Commands are case-sensitive
- Arguments are space-separated
- Numeric arguments can be integers or decimals
- Commands must end with `\r\n` (carriage return + line feed)

### Response Format

All responses follow this pattern:

```
\r\n<response_data>\r\n
```

- Success responses: `\r\nOK\r\n` or `\r\nOK <additional_data>\r\n`
- Data responses: `\r\n<data>\r\n`

## Supported Commands

### Motor Control Commands

| Command | Arguments | Description |
|---------|-----------|-------------|
| `FREE` | None | Disable motor drivers (de-energize) |
| `@CLOSE` | None | Enable motor drivers and close gripper |
| `@RESET` | None | Reset all position counters to zero |
| `@SET` | `<speed>` | Set global motor speed (steps/sec) |

### Motion Commands

| Command | Arguments | Description |
|---------|-----------|-------------|
| `@STEP` | `<speed> <j1> <j2> <j3> <j4> <j5> <j6>` | Move to absolute positions |
| `@MOVEJ` | `<j1> <j2> <j3> <j4> <j5> <j6>` | Synchronized interpolated joint move |
| `@MOVEIK` | `<x> <y> <z> <pitch> <roll> <gripper>` | Inverse kinematics move (Cartesian) |

### Status Commands

| Command | Arguments | Description |
|---------|-----------|-------------|
| `@READ` | None | Read current joint positions and limit switches |

## Detailed Command Descriptions

### FREE

Disables motor drivers, allowing joints to move freely.

```
TX: FREE\r\n
RX: \r\nOK\r\n
```

### @CLOSE

Enables motor drivers. If limit switches are configured, performs gripper homing sequence.

```
TX: @CLOSE\r\n
RX: \r\nOK\r\n
```

### @READ

Returns current positions of all 6 joints and limit switch states.

**Response format:** `J1, J2, J3, J4, J5, J6, LimitSwitches`

```
TX: @READ\r\n
RX: \r\n100, -50, 200, 0, 0, 150, 0\r\n
```

| Field | Description |
|-------|-------------|
| J1-J6 | Current position in steps (int16) |
| LimitSwitches | Bitmask of limit switch states |

### @RESET

Resets all joint position counters to zero (software home).

```
TX: @RESET\r\n
RX: \r\nOK\r\n
```

### @SET

Sets the global motor speed for subsequent moves.

**Arguments:**
- `speed`: Motor speed in steps per second

```
TX: @SET 500\r\n
RX: \r\nOK\r\n
```

### @STEP

Moves all joints to absolute positions at the specified speed.

**Arguments:**
- `speed`: Movement speed in steps per second
- `j1-j6`: Target positions in steps for each joint

```
TX: @STEP 200 1000 500 -300 0 0 100\r\n
RX: \r\nOK\r\n
```

**Note:** This command returns immediately. Use `@READ` to monitor position.

### @MOVEJ

Performs synchronized interpolated motion where all joints arrive simultaneously using trapezoidal velocity profiles.

**Arguments:**
- `j1-j6`: Target positions in steps for each joint

**Response:** `OK DUR:<milliseconds>` - includes motion duration

```
TX: @MOVEJ 1000 500 -300 0 0 100\r\n
RX: \r\nOK DUR:2500\r\n
```

**Features:**
- All joints start and finish together
- Smooth acceleration/deceleration
- Returns estimated motion duration

### @MOVEIK

Performs motion to Cartesian coordinates using inverse kinematics.

**Arguments:**
- `x, y, z`: Cartesian position in mm (scaled by 10)
- `pitch, roll`: Orientation in degrees (scaled by 10)
- `gripper`: Gripper position in steps

```
TX: @MOVEIK 1500 0 2000 450 0 100\r\n
RX: \r\nOK DUR:3000\r\n
```

**Note:** Requires `ENABLE_INTERPOLATOR` build flag.

## Communication Examples

### Basic Motion Sequence

```
TX: @CLOSE\r\n              (Enable motors)
RX: \r\nOK\r\n

TX: @RESET\r\n              (Zero position counters)
RX: \r\nOK\r\n

TX: @SET 300\r\n            (Set speed to 300 steps/sec)
RX: \r\nOK\r\n

TX: @STEP 300 500 0 0 0 0 0\r\n  (Move J1 to 500)
RX: \r\nOK\r\n

TX: @READ\r\n               (Check position)
RX: \r\n500, 0, 0, 0, 0, 0, 0\r\n

TX: FREE\r\n                (Disable motors)
RX: \r\nOK\r\n
```

### Interpolated Motion Sequence

```
TX: @CLOSE\r\n
RX: \r\nOK\r\n

TX: @MOVEJ 1000 500 -200 0 0 50\r\n
RX: \r\nOK DUR:2000\r\n

(wait 2000ms for motion to complete)

TX: @READ\r\n
RX: \r\n1000, 500, -200, 0, 0, 50, 0\r\n

TX: @MOVEJ 0 0 0 0 0 0\r\n   (Return to home)
RX: \r\nOK DUR:2000\r\n
```

## Python Examples

### Basic Serial Client

```python
import serial
import time

class TCMClient:
    """TCM protocol serial client for Robko 01 robot."""

    def __init__(self, port: str, baudrate: int = 115200, timeout: float = 1.0):
        self.serial = serial.Serial(port, baudrate, timeout=timeout)
        time.sleep(2)  # Wait for Arduino reset

    def _send_command(self, command: str) -> str:
        """Send command and receive response."""
        # Clear input buffer
        self.serial.reset_input_buffer()

        # Send command with CRLF
        self.serial.write(f"{command}\r\n".encode())

        # Read response (skip empty lines)
        response = ""
        while True:
            line = self.serial.readline().decode().strip()
            if line:
                response = line
                break

        return response

    def enable(self) -> bool:
        """Enable motor drivers."""
        return self._send_command("@CLOSE") == "OK"

    def disable(self) -> bool:
        """Disable motor drivers."""
        return self._send_command("FREE") == "OK"

    def reset(self) -> bool:
        """Reset position counters to zero."""
        return self._send_command("@RESET") == "OK"

    def set_speed(self, speed: int) -> bool:
        """Set global motor speed."""
        return self._send_command(f"@SET {speed}") == "OK"

    def read_position(self) -> dict:
        """Read current joint positions."""
        response = self._send_command("@READ")
        values = [int(x.strip()) for x in response.split(',')]
        return {
            'j1': values[0],
            'j2': values[1],
            'j3': values[2],
            'j4': values[3],
            'j5': values[4],
            'j6': values[5],
            'limits': values[6] if len(values) > 6 else 0
        }

    def step(self, speed: int, j1=0, j2=0, j3=0, j4=0, j5=0, j6=0) -> bool:
        """Move to absolute positions."""
        cmd = f"@STEP {speed} {j1} {j2} {j3} {j4} {j5} {j6}"
        return self._send_command(cmd) == "OK"

    def movej(self, j1=0, j2=0, j3=0, j4=0, j5=0, j6=0) -> int:
        """Interpolated joint move. Returns duration in ms."""
        cmd = f"@MOVEJ {j1} {j2} {j3} {j4} {j5} {j6}"
        response = self._send_command(cmd)
        if response.startswith("OK DUR:"):
            return int(response.split(":")[1])
        return -1

    def moveik(self, x=0, y=0, z=0, pitch=0, roll=0, gripper=0) -> int:
        """Inverse kinematics move. Returns duration in ms."""
        cmd = f"@MOVEIK {x} {y} {z} {pitch} {roll} {gripper}"
        response = self._send_command(cmd)
        if response.startswith("OK DUR:"):
            return int(response.split(":")[1])
        return -1

    def wait_for_motion(self, duration_ms: int):
        """Wait for motion to complete."""
        time.sleep(duration_ms / 1000.0 + 0.1)  # Add small buffer

    def close(self):
        """Close serial connection."""
        self.serial.close()
```

### Usage Examples

```python
# Connect to robot
robot = TCMClient('COM7', baudrate=115200)  # Windows
# robot = TCMClient('/dev/ttyUSB0', baudrate=115200)  # Linux

# Enable motors
robot.enable()

# Reset position counters
robot.reset()

# Read current position
pos = robot.read_position()
print(f"J1: {pos['j1']}, J2: {pos['j2']}")

# Simple move using @STEP
robot.set_speed(300)
robot.step(300, j1=500, j2=200)

# Wait and check position
import time
time.sleep(2)
pos = robot.read_position()
print(f"After move - J1: {pos['j1']}, J2: {pos['j2']}")

# Interpolated move (all joints synchronized)
duration = robot.movej(j1=1000, j2=500, j3=-200, j4=0, j5=0, j6=50)
print(f"Motion will take {duration} ms")
robot.wait_for_motion(duration)

# Return to home
duration = robot.movej(0, 0, 0, 0, 0, 0)
robot.wait_for_motion(duration)

# Disable motors
robot.disable()
robot.close()
```

### Interactive Testing Script

```python
#!/usr/bin/env python3
"""Interactive TCM testing script."""

import serial
import sys

def main():
    port = sys.argv[1] if len(sys.argv) > 1 else 'COM7'

    ser = serial.Serial(port, 115200, timeout=1)
    print(f"Connected to {port}")
    print("Enter commands (FREE, @CLOSE, @READ, @RESET, @SET, @STEP, @MOVEJ)")
    print("Type 'quit' to exit\n")

    try:
        while True:
            cmd = input("> ").strip()
            if cmd.lower() == 'quit':
                break
            if not cmd:
                continue

            ser.write(f"{cmd}\r\n".encode())

            # Read response
            response = ser.readline().decode().strip()
            while not response:
                response = ser.readline().decode().strip()
            print(f"< {response}")

    except KeyboardInterrupt:
        pass
    finally:
        ser.close()
        print("\nDisconnected")

if __name__ == "__main__":
    main()
```

## Troubleshooting

### No Response

| Symptom | Possible Cause | Solution |
|---------|----------------|----------|
| No response | TCM not enabled | Add `-D ENABLE_TCM_COMMANDS=1` to build flags |
| No response | Wrong COM port | Check Device Manager for correct port |
| No response | Wrong baud rate | Verify `monitor_speed` matches serial config |
| Garbled response | Baud rate mismatch | Use 115200 baud |

### Motion Issues

| Symptom | Possible Cause | Solution |
|---------|----------------|----------|
| Motors don't move | Motors disabled | Send `@CLOSE` to enable |
| Jerky motion | Speed too high | Reduce speed with `@SET` |
| `@MOVEJ` not recognized | Interpolator not enabled | Add `-D ENABLE_INTERPOLATOR=1` |
| Position drift | No hardware homing | Use `@RESET` to set software home |

### Debugging Tips

1. **Test with serial monitor**: Open PlatformIO serial monitor to see raw communication
2. **Check line endings**: Ensure your terminal sends `\r\n` (CRLF)
3. **Start simple**: Test `FREE` and `@CLOSE` before complex moves
4. **Monitor positions**: Use `@READ` frequently to track robot state

## See Also

- `doc/SUPER.md` - Binary protocol for high-speed control
- `doc/MODBUS_TCP.md` - Modbus TCP protocol
- `doc/MODBUS_RTU.md` - Modbus RTU protocol
