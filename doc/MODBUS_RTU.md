# Modbus RTU Interface

This document describes the Modbus RTU (Remote Terminal Unit) serial interface for the Robko 01 robot controller.

## Overview

Modbus RTU is a serial communication protocol that uses the main UART (Serial) port for communication. This mode is ideal for direct wired connections to PLCs, HMIs, or other industrial automation equipment.

## Configuration

### Build Profile

Use the `modbus_rtu` environment to build firmware with Modbus RTU support:

```bash
pio run -e modbus_rtu
pio run -e modbus_rtu -t upload
```

### Default Settings

| Parameter | Default Value | Description |
|-----------|---------------|-------------|
| Baudrate | 9600 | Serial communication speed |
| Data bits | 8 | Number of data bits |
| Parity | None | Parity checking |
| Stop bits | 1 | Number of stop bits |
| Slave ID | 1 | Modbus device address |
| Timeout | 2000 ms | Response timeout |

### Custom Configuration

Override defaults in `platformio_modbus_rtu.ini`:

```ini
build_flags =
  -D MODBUS_RTU_BAUDRATE=19200
  -D MODBUS_SLAVE_ID=2
  -D MODBUS_RTU_TIMEOUT_MS=3000
```

## Hardware Connection

Connect to the ESP32's main UART pins:

| ESP32 Pin | Function | Connection |
|-----------|----------|------------|
| TX (GPIO1) | Transmit | RX on master device |
| RX (GPIO3) | Receive | TX on master device |
| GND | Ground | Common ground |

**Note:** When using Modbus RTU, the main Serial port is dedicated to Modbus communication. Debug output is disabled.

### RS-485 Connection (Optional)

For RS-485 networks, use a TTL to RS-485 converter:

```
ESP32 TX (GPIO1) --> DI (Driver Input)
ESP32 RX (GPIO3) --> RO (Receiver Output)
ESP32 GPIO (any) --> DE/RE (Direction control, active HIGH for TX)
```

## Register Map

### Holding Registers (Function Codes 03, 06, 16)

| Address | Name | Access | Description |
|---------|------|--------|-------------|
| 0 | CurrentPos_J1 | Read | Current position of Joint 1 (steps) |
| 1 | CurrentPos_J2 | Read | Current position of Joint 2 (steps) |
| 2 | CurrentPos_J3 | Read | Current position of Joint 3 (steps) |
| 3 | CurrentPos_J4 | Read | Current position of Joint 4 (steps) |
| 4 | CurrentPos_J5 | Read | Current position of Joint 5 (steps) |
| 5 | CurrentPos_J6 | Read | Current position of Joint 6 (steps) |
| 6 | TargetPos_J1 | R/W | Target position of Joint 1 (steps) |
| 7 | TargetPos_J2 | R/W | Target position of Joint 2 (steps) |
| 8 | TargetPos_J3 | R/W | Target position of Joint 3 (steps) |
| 9 | TargetPos_J4 | R/W | Target position of Joint 4 (steps) |
| 10 | TargetPos_J5 | R/W | Target position of Joint 5 (steps) |
| 11 | TargetPos_J6 | R/W | Target position of Joint 6 (steps) |
| 12 | MotorsEnabled | R/W | Motors enable state (0=disabled, 1=enabled) |
| 13 | RobotBusy | Read | Robot busy status (0=idle, 1=moving) |
| 14 | MaxSpeed_J1 | R/W | Maximum speed of Joint 1 (steps/sec) |
| 15 | MaxSpeed_J2 | R/W | Maximum speed of Joint 2 (steps/sec) |
| 16 | MaxSpeed_J3 | R/W | Maximum speed of Joint 3 (steps/sec) |
| 17 | MaxSpeed_J4 | R/W | Maximum speed of Joint 4 (steps/sec) |
| 18 | MaxSpeed_J5 | R/W | Maximum speed of Joint 5 (steps/sec) |
| 19 | MaxSpeed_J6 | R/W | Maximum speed of Joint 6 (steps/sec) |
| 20 | StartMotion | Write | Start interpolated motion (write 1) |
| 21 | StopMotion | Write | Stop all motion (write 1) |

### Supported Function Codes

| Code | Name | Description |
|------|------|-------------|
| 03 | Read Holding Registers | Read one or more registers |
| 06 | Write Single Register | Write a single register |
| 16 | Write Multiple Registers | Write multiple registers |

## Python Examples (pymodbus)

### Installation

```bash
pip install pymodbus
```

### Basic Connection

```python
from pymodbus.client import ModbusSerialClient

# Create RTU client
client = ModbusSerialClient(
    port='COM7',           # Windows: COM7, Linux: /dev/ttyUSB0
    baudrate=9600,
    bytesize=8,
    parity='N',
    stopbits=1,
    timeout=2
)

# Connect to the device
if client.connect():
    print("Connected to Robko 01")
else:
    print("Connection failed")
```

### Read Current Positions

```python
from pymodbus.client import ModbusSerialClient

client = ModbusSerialClient(port='COM7', baudrate=9600, timeout=2)
client.connect()

# Read all 6 joint positions (registers 0-5)
result = client.read_holding_registers(address=0, count=6, slave=1)

if not result.isError():
    positions = result.registers
    for i, pos in enumerate(positions):
        # Convert unsigned to signed (16-bit)
        if pos > 32767:
            pos -= 65536
        print(f"Joint {i+1}: {pos} steps")
else:
    print(f"Error: {result}")

client.close()
```

### Enable Motors

```python
from pymodbus.client import ModbusSerialClient

client = ModbusSerialClient(port='COM7', baudrate=9600, timeout=2)
client.connect()

# Enable motors (register 12 = 1)
result = client.write_register(address=12, value=1, slave=1)

if not result.isError():
    print("Motors enabled")
else:
    print(f"Error: {result}")

client.close()
```

### Move Single Joint

```python
from pymodbus.client import ModbusSerialClient
import time

client = ModbusSerialClient(port='COM7', baudrate=9600, timeout=2)
client.connect()

# Enable motors
client.write_register(address=12, value=1, slave=1)

# Set target position for Joint 1 (register 6)
target_position = 500  # steps
client.write_register(address=6, value=target_position & 0xFFFF, slave=1)

# Wait for motion to complete
while True:
    result = client.read_holding_registers(address=13, count=1, slave=1)
    if not result.isError() and result.registers[0] == 0:
        print("Motion complete")
        break
    time.sleep(0.1)

client.close()
```

### Move All Joints (Synchronized)

```python
from pymodbus.client import ModbusSerialClient
import time

def to_unsigned(value):
    """Convert signed int16 to unsigned for Modbus"""
    return value & 0xFFFF

client = ModbusSerialClient(port='COM7', baudrate=9600, timeout=2)
client.connect()

# Enable motors
client.write_register(address=12, value=1, slave=1)

# Target positions for all 6 joints
targets = [100, 200, 150, -50, 75, 0]  # steps

# Write all target positions at once (registers 6-11)
unsigned_targets = [to_unsigned(t) for t in targets]
client.write_registers(address=6, values=unsigned_targets, slave=1)

# Trigger interpolated motion (register 20 = 1)
client.write_register(address=20, value=1, slave=1)

# Wait for motion to complete
while True:
    result = client.read_holding_registers(address=13, count=1, slave=1)
    if not result.isError() and result.registers[0] == 0:
        print("Interpolated motion complete")
        break
    time.sleep(0.1)

client.close()
```

### Set Joint Speeds

```python
from pymodbus.client import ModbusSerialClient

client = ModbusSerialClient(port='COM7', baudrate=9600, timeout=2)
client.connect()

# Set max speed for all joints (registers 14-19)
speeds = [100, 100, 100, 100, 100, 100]  # steps/sec
client.write_registers(address=14, values=speeds, slave=1)

print("Joint speeds configured")
client.close()
```

### Emergency Stop

```python
from pymodbus.client import ModbusSerialClient

client = ModbusSerialClient(port='COM7', baudrate=9600, timeout=2)
client.connect()

# Stop all motion immediately (register 21 = 1)
client.write_register(address=21, value=1, slave=1)

# Disable motors
client.write_register(address=12, value=0, slave=1)

print("Emergency stop executed")
client.close()
```

### Complete Robot Control Class

```python
from pymodbus.client import ModbusSerialClient
import time

class RobkoModbusRTU:
    """Modbus RTU client for Robko 01 robot controller"""

    def __init__(self, port='COM7', baudrate=9600, slave_id=1):
        self.client = ModbusSerialClient(
            port=port,
            baudrate=baudrate,
            bytesize=8,
            parity='N',
            stopbits=1,
            timeout=2
        )
        self.slave_id = slave_id

    def connect(self):
        return self.client.connect()

    def disconnect(self):
        self.client.close()

    def _to_signed(self, value):
        """Convert unsigned uint16 to signed int16"""
        return value - 65536 if value > 32767 else value

    def _to_unsigned(self, value):
        """Convert signed int16 to unsigned uint16"""
        return value & 0xFFFF

    def enable_motors(self, enable=True):
        """Enable or disable motors"""
        result = self.client.write_register(
            address=12, value=1 if enable else 0, slave=self.slave_id
        )
        return not result.isError()

    def get_positions(self):
        """Read current positions of all joints"""
        result = self.client.read_holding_registers(
            address=0, count=6, slave=self.slave_id
        )
        if result.isError():
            return None
        return [self._to_signed(p) for p in result.registers]

    def get_targets(self):
        """Read target positions of all joints"""
        result = self.client.read_holding_registers(
            address=6, count=6, slave=self.slave_id
        )
        if result.isError():
            return None
        return [self._to_signed(p) for p in result.registers]

    def is_busy(self):
        """Check if robot is currently moving"""
        result = self.client.read_holding_registers(
            address=13, count=1, slave=self.slave_id
        )
        if result.isError():
            return None
        return result.registers[0] != 0

    def set_target(self, joint, position):
        """Set target position for a single joint (0-5)"""
        result = self.client.write_register(
            address=6 + joint,
            value=self._to_unsigned(position),
            slave=self.slave_id
        )
        return not result.isError()

    def set_targets(self, positions):
        """Set target positions for all joints"""
        values = [self._to_unsigned(p) for p in positions]
        result = self.client.write_registers(
            address=6, values=values, slave=self.slave_id
        )
        return not result.isError()

    def start_motion(self):
        """Start interpolated motion to target positions"""
        result = self.client.write_register(
            address=20, value=1, slave=self.slave_id
        )
        return not result.isError()

    def stop(self):
        """Stop all motion immediately"""
        result = self.client.write_register(
            address=21, value=1, slave=self.slave_id
        )
        return not result.isError()

    def move_to(self, positions, wait=True):
        """Move all joints to specified positions"""
        if not self.set_targets(positions):
            return False
        if not self.start_motion():
            return False
        if wait:
            while self.is_busy():
                time.sleep(0.05)
        return True

    def home(self):
        """Move all joints to zero position"""
        return self.move_to([0, 0, 0, 0, 0, 0])


# Usage example
if __name__ == "__main__":
    robot = RobkoModbusRTU(port='COM7')

    if robot.connect():
        print("Connected to Robko 01")

        # Enable motors
        robot.enable_motors(True)

        # Read current positions
        positions = robot.get_positions()
        print(f"Current positions: {positions}")

        # Move to a new position
        robot.move_to([100, 200, 150, 0, 50, 0])
        print("Motion complete")

        # Return home
        robot.home()
        print("Homed")

        # Disable motors
        robot.enable_motors(False)

        robot.disconnect()
    else:
        print("Connection failed")
```

## Troubleshooting

### No Response from Device

1. Check serial port connection and cable
2. Verify baudrate matches firmware configuration
3. Confirm slave ID is correct (default: 1)
4. Ensure proper TX/RX wiring (may need crossover)

### Timeout Errors

1. Increase timeout value in client configuration
2. Check for electrical noise on the serial line
3. Verify ground connection between devices

### CRC Errors

1. Check cable quality and length
2. Reduce baudrate if experiencing noise
3. Ensure proper termination for RS-485 networks

## See Also

- [MODBUS_TCP.md](MODBUS_TCP.md) - TCP/IP interface documentation
- [pymodbus documentation](https://pymodbus.readthedocs.io/)
