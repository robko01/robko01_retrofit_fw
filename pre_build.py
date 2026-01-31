#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
@file pre_build.py
@brief PlatformIO pre-build script for loading environment variables.

This script is executed before the build process and loads environment
variables from a .env file in the project root directory. The variables
are then merged into the build environment, making them available to
the PlatformIO build system.

@note Created with the help of Ivanushka (ChatGPT)
"""

import os
import re

Import("env")

_NUMERIC_RE = re.compile(r"^-?\d+(\.\d+)?$")


def _load_env_file(path):
    envs = {}
    if not os.path.exists(path):
        return envs
    with open(path, "r") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#") or line.startswith(";"):
                continue
            if "=" not in line:
                continue
            key, value = line.split("=", 1)
            envs[key.strip()] = value.strip().strip('"').strip("'")
    return envs


def _coerce_value(raw_value):
    if _NUMERIC_RE.match(raw_value):
        try:
            return int(raw_value)
        except ValueError:
            return float(raw_value)
    return raw_value


def _inject_defines(env_map):
    defines = []
    for key in _DEFINE_KEYS:
        value = env_map.get(key)
        if value is None:
            continue
        defines.append((key, _coerce_value(value)))
    if defines:
        env.Append(CPPDEFINES=defines)


_DEFINE_KEYS = [
    "WIFI_SSID",
    "WIFI_PASS",
    "OTA_PORT",
    "OTA_HOST_NAME",
    "OTA_PASS_HASH",
    "WG_ENDPOINT",
    "WG_LOCAL_IP",
    "WG_PRIVATE_KEY",
    "WG_PUBLIC_KEY",
    "PS4_MAC",
]

# Load variables from .env file in project root. External environment values
# take precedence when provided.
project_dir = env.get("PROJECT_DIR", os.getcwd())
dotenv_path = os.path.join(project_dir, ".env")
file_vars = _load_env_file(dotenv_path)
merged = dict(file_vars)
merged.update({k: v for k, v in os.environ.items() if k in _DEFINE_KEYS})

_inject_defines(merged)
