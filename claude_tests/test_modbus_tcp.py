#!/usr/bin/env python3
"""
Basic Modbus TCP test for Robko01.

Defaults to read-only checks. Use --write to toggle MotorsEnabled (register 12)
and restore the original value. Use --move to set targets and start motion.
"""

import argparse
import sys
import time
from typing import List

from pymodbus.client import ModbusTcpClient


REG_CURRENT_POS = 0
REG_TARGET_POS = 6
REG_MOTORS_ENABLED = 12
REG_ROBOT_BUSY = 13
REG_START_MOTION = 20

DEFAULT_TARGETS = [100, 100, 100, 0, 0, 0]


def _to_signed(value: int) -> int:
    return value - 65536 if value > 32767 else value


def _read_holding(client: ModbusTcpClient, addr: int, count: int, slave: int) -> List[int]:
    resp = client.read_holding_registers(address=addr, count=count, slave=slave)
    if resp.isError():
        raise RuntimeError(f"Read error @ {addr}: {resp}")
    return resp.registers


def _write_single(client: ModbusTcpClient, addr: int, value: int, slave: int) -> None:
    resp = client.write_register(address=addr, value=value, slave=slave)
    if resp.isError():
        raise RuntimeError(f"Write error @ {addr}: {resp}")


def _write_multiple(client: ModbusTcpClient, addr: int, values: List[int], slave: int) -> None:
    resp = client.write_registers(address=addr, values=values, slave=slave)
    if resp.isError():
        raise RuntimeError(f"Write error @ {addr}: {resp}")


def main() -> int:
    parser = argparse.ArgumentParser(description="Robko01 Modbus TCP test")
    parser.add_argument("--host", default="192.168.1.100", help="Robot IP or hostname")
    parser.add_argument("--port", type=int, default=502, help="Modbus TCP port")
    parser.add_argument("--slave", type=int, default=1, help="Modbus slave/unit ID")
    parser.add_argument("--timeout", type=float, default=5.0, help="Timeout seconds")
    parser.add_argument("--write", action="store_true", help="Enable safe write test")
    parser.add_argument("--move", action="store_true", help="Write targets and start motion")
    parser.add_argument(
        "--targets",
        default=",".join(str(v) for v in DEFAULT_TARGETS),
        help="Comma-separated target positions (6 values)",
    )
    parser.add_argument("--wait", action="store_true", help="Wait for motion to complete")
    args = parser.parse_args()

    print("Robko01 Modbus TCP test")
    print(f"Host: {args.host}:{args.port}  Slave: {args.slave}")

    client = ModbusTcpClient(host=args.host, port=args.port, timeout=args.timeout)
    if not client.connect():
        print("Connection failed")
        return 1

    try:
        positions = _read_holding(client, REG_CURRENT_POS, 6, args.slave)
        positions_signed = [_to_signed(p) for p in positions]
        print(f"Current positions (steps): {positions_signed}")

        busy = _read_holding(client, REG_ROBOT_BUSY, 1, args.slave)[0]
        print(f"Robot busy: {busy}")

        if args.write:
            current = _read_holding(client, REG_MOTORS_ENABLED, 1, args.slave)[0]
            new_value = 0 if current else 1
            _write_single(client, REG_MOTORS_ENABLED, new_value, args.slave)
            verify = _read_holding(client, REG_MOTORS_ENABLED, 1, args.slave)[0]
            print(f"MotorsEnabled toggled {current} -> {verify}")
            _write_single(client, REG_MOTORS_ENABLED, current, args.slave)
            print("MotorsEnabled restored")

        if args.move:
            raw_targets = [v.strip() for v in args.targets.split(",") if v.strip()]
            if len(raw_targets) != 6:
                raise RuntimeError("Expected 6 comma-separated targets")
            targets = [int(v) for v in raw_targets]
            unsigned_targets = [t & 0xFFFF for t in targets]
            print(f"Setting targets: {targets}")
            _write_multiple(client, REG_TARGET_POS, unsigned_targets, args.slave)
            _write_single(client, REG_START_MOTION, 1, args.slave)
            print("Motion started")
            if args.wait:
                print("Waiting for motion to complete...")
                while True:
                    busy = _read_holding(client, REG_ROBOT_BUSY, 1, args.slave)[0]
                    if busy == 0:
                        break
                    time.sleep(0.1)
                print("Motion complete")

        print("TCP test OK")
        return 0
    finally:
        client.close()


if __name__ == "__main__":
    raise SystemExit(main())
