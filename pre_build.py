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
# import ini
import sys

print(sys.version_info)

def load_env_file(path):
    envs = {}
    if not os.path.exists(path):
        return envs
    with open(path, "r") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            if "=" not in line:
                continue
            key, value = line.split("=", 1)
            envs[key.strip()] = value.strip().strip('"').strip("'")
    return envs

# Load variables from .env file in project root
cwf = os.path.dirname(os.path.abspath("pre_build.py"))
dotenv_path = os.path.join(cwf, ".env")
variables = load_env_file(dotenv_path)

# Merge into build flags
for name in variables:
    print(name, variables[name])
    os.environ[name] = variables[name]


for name in os.environ:
    print(name, os.environ[name])
