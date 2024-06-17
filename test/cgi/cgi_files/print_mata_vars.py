#! /usr/bin/python3
import os

print("\n\n", end="")

env_vars = os.environ

for key, value in env_vars.items():
    print(f"{key}: {value}")
