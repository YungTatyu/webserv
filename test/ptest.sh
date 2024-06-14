#!/bin/bash

readonly CWD=$(dirname "$0")
printf "${CWD}\n"

make fclean

python3 ${CWD}/run_tests.py
