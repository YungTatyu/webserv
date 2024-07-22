#! /usr/bin/python3
import os

current_dir = os.getcwd()
dir_name = os.path.basename(current_dir)

assert dir_name == "cgi_test", f"Expected 'cgi_test', but got '{dir_name}'"

print("\n\n", end="")
