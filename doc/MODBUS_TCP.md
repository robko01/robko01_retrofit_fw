# Modbus TCP Interface

This document describes the Modbus TCP/IP network interface for the Robko 01 robot controller.

## Overview

Modbus TCP is an Ethernet-based protocol that allows remote control of the robot over a network. The ESP32 acts as a Modbus TCP server, accepting connections from Modbus clients (masters) such as SCADA systems, HMIs, or custom applications.

## Configuration

### Build Profile

Use the `modbus_tcp` environment to build firmware with Modbus TCP support:

```bash
pio run -e modbus_tcp
pio run -e modbus_tcp -t upload
```

### Default Settings

| Parameter | Default Value | Description |
|-----------|---------------|-------------|
| TCP Port | 502 | Standard Modbus TCP port |
| Max Clients | 4 | Maximum concurrent connections |
| Timeout | 20000 ms | Connection timeout |
| Slave ID | 1 | Modbus unit identifier |

### Custom Configuration

Override defaults in `platformio_modbus_tcp.ini`:

```ini
build_flags =
  -D MODBUS_TCP_PORT=5020
  -D MODBUS_TCP_MAX_CLIENTS=2
  -D MODBUS_TCP_TIMEOUT_MS=30000
  -D MODBUS_SLAVE_ID=1
```

### WiFi Configuration

WiFi credentials are loaded from `.env` file via `pre_build.py`:

```env
WIFI_SSID=YourNetworkName
WIFI_PASS=YourPassword
WIFI_HOST_NAME=robko01
```

## Network Setup

### Finding the Robot's IP Address

The robot obtains an IP address via DHCP. You can find it:

1. **Serial Monitor** (before Modbus RTU mode): Check startup messages
2. **mDNS**: Access via `robko01.local` (if ENABLE_MDNS is set)
3. **Router Admin**: Check DHCP client list

### Firewall Configuration

Ensure port 502 (or custom port) is accessible:
- Windows: Allow through Windows Firewall
- Linux: `sudo ufw allow 502/tcp`

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
from pymodbus.client import ModbusTcpClient

# Create TCP client
client = ModbusTcpClient(
    host='192.168.1.100',  # Robot IP address
    port=502,              # Modbus TCP port
    timeout=5
)

# Connect to the device
if client.connect():
    print("Connected to Robko 01")
else:
    print("Connection failed")
```

### Using mDNS Hostname

```python
from pymodbus.client import ModbusTcpClient

# Connect using mDNS name (requires zeroconf/avahi)
client = ModbusTcpClient(
    host='robko01.local',
    port=502,
    timeout=5
)

client.connect()
```

### Read Current Positions

```python
from pymodbus.client import ModbusTcpClient

client = ModbusTcpClient(host='192.168.1.100', port=502, timeout=5)
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
from pymodbus.client import ModbusTcpClient

client = ModbusTcpClient(host='192.168.1.100', port=502, timeout=5)
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
from pymodbus.client import ModbusTcpClient
import time

client = ModbusTcpClient(host='192.168.1.100', port=502, timeout=5)
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
from pymodbus.client import ModbusTcpClient
import time

def to_unsigned(value):
    """Convert signed int16 to unsigned for Modbus"""
    return value & 0xFFFF

client = ModbusTcpClient(host='192.168.1.100', port=502, timeout=5)
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
from pymodbus.client import ModbusTcpClient

client = ModbusTcpClient(host='192.168.1.100', port=502, timeout=5)
client.connect()

# Set max speed for all joints (registers 14-19)
speeds = [100, 100, 100, 100, 100, 100]  # steps/sec
client.write_registers(address=14, values=speeds, slave=1)

print("Joint speeds configured")
client.close()
```

### Emergency Stop

```python
from pymodbus.client import ModbusTcpClient

client = ModbusTcpClient(host='192.168.1.100', port=502, timeout=5)
client.connect()

# Stop all motion immediately (register 21 = 1)
client.write_register(address=21, value=1, slave=1)

# Disable motors
client.write_register(address=12, value=0, slave=1)

print("Emergency stop executed")
client.close()
```

### Async Client (for GUI applications)

```python
import asyncio
from pymodbus.client import AsyncModbusTcpClient

async def read_positions():
    """Async example for non-blocking I/O"""
    client = AsyncModbusTcpClient(
        host='192.168.1.100',
        port=502,
        timeout=5
    )

    await client.connect()

    # Read positions asynchronously
    result = await client.read_holding_registers(address=0, count=6, slave=1)

    if not result.isError():
        print(f"Positions: {result.registers}")

    client.close()

# Run the async function
asyncio.run(read_positions())
```

### Complete Robot Control Class

```python
from pymodbus.client import ModbusTcpClient
import time

class RobkoModbusTCP:
    """Modbus TCP client for Robko 01 robot controller"""

    def __init__(self, host='192.168.1.100', port=502, slave_id=1):
        self.client = ModbusTcpClient(
            host=host,
            port=port,
            timeout=5
        )
        self.slave_id = slave_id

    def connect(self):
        return self.client.connect()

    def disconnect(self):
        self.client.close()

    def is_connected(self):
        return self.client.is_socket_open()

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

    def set_speeds(self, speeds):
        """Set max speeds for all joints"""
        result = self.client.write_registers(
            address=14, values=speeds, slave=self.slave_id
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
    robot = RobkoModbusTCP(host='192.168.1.100')

    if robot.connect():
        print("Connected to Robko 01")

        # Enable motors
        robot.enable_motors(True)

        # Read current positions
        positions = robot.get_positions()
        print(f"Current positions: {positions}")

        # Set speeds
        robot.set_speeds([100, 100, 100, 100, 100, 100])

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

### Connection Monitoring with Reconnect

```python
from pymodbus.client import ModbusTcpClient
import time

class RobkoModbusTCPReconnect:
    """Modbus TCP client with automatic reconnection"""

    def __init__(self, host, port=502, slave_id=1, reconnect_delay=5):
        self.host = host
        self.port = port
        self.slave_id = slave_id
        self.reconnect_delay = reconnect_delay
        self.client = None

    def connect(self):
        """Connect with retry logic"""
        self.client = ModbusTcpClient(
            host=self.host,
            port=self.port,
            timeout=3
        )
        return self.client.connect()

    def ensure_connected(self):
        """Ensure connection is active, reconnect if needed"""
        if self.client is None or not self.client.is_socket_open():
            print(f"Reconnecting to {self.host}...")
            if self.connect():
                print("Reconnected successfully")
                return True
            else:
                print(f"Reconnection failed, retrying in {self.reconnect_delay}s")
                time.sleep(self.reconnect_delay)
                return False
        return True

    def read_positions_safe(self):
        """Read positions with connection handling"""
        if not self.ensure_connected():
            return None

        try:
            result = self.client.read_holding_registers(
                address=0, count=6, slave=self.slave_id
            )
            if result.isError():
                return None
            return result.registers
        except Exception as e:
            print(f"Read error: {e}")
            return None


# Usage with reconnection
robot = RobkoModbusTCPReconnect(host='192.168.1.100')
robot.connect()

# Continuous monitoring loop
while True:
    positions = robot.read_positions_safe()
    if positions:
        print(f"Positions: {positions}")
    time.sleep(1)
```

## Network Best Practices

### Static IP Configuration

For production environments, configure a static IP on the robot:

```ini
; In platformio_modbus_tcp.ini or .env
-D WIFI_STATIC_IP=\"192.168.1.100\"
-D WIFI_GATEWAY=\"192.168.1.1\"
-D WIFI_SUBNET=\"255.255.255.0\"
```

### Multiple Client Access

The server supports up to 4 concurrent clients. Consider:
- Using connection pooling in client applications
- Implementing proper connection management
- Avoiding holding connections open indefinitely

### Latency Optimization

For real-time control applications:
1. Use persistent connections (keep-alive)
2. Reduce polling interval based on network latency
3. Consider combining multiple register reads/writes
4. Use wired Ethernet if possible (via ESP32 Ethernet module)

## Troubleshooting

### Connection Refused

1. Verify robot is connected to WiFi (check serial output)
2. Confirm IP address is correct
3. Ensure port 502 is not blocked by firewall
4. Check if another client has all connections

### Timeout Errors

1. Increase client timeout value
2. Check network stability and latency
3. Verify robot is not busy with other operations
4. Reduce number of concurrent connections

### Intermittent Disconnections

1. Check WiFi signal strength
2. Move robot closer to access point
3. Use static IP instead of DHCP
4. Consider using WireGuard VPN for remote access

### Register Read Returns Unexpected Values

1. Verify register addresses (0-based indexing)
2. Check signed/unsigned conversion for positions
3. Ensure slave ID matches configuration

## Security Considerations

Modbus TCP has no built-in authentication or encryption. For secure deployments:

1. Use network segmentation (separate VLAN)
2. Enable WireGuard VPN for remote access
3. Implement firewall rules to restrict access
4. Consider using Modbus TCP over TLS (requires custom implementation)

## See Also

- [MODBUS_RTU.md](MODBUS_RTU.md) - Serial RTU interface documentation
- [pymodbus documentation](https://pymodbus.readthedocs.io/)
- [Modbus TCP/IP Specification](https://modbus.org/specs.php)
