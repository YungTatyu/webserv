#!/usr/bin/python3

import os
import sys
import time


def main():
    print(
        "Status: 200\r\nContent-Type: text/html\r\nContent-Length: 500\r\n\r\n", end=""
    )
    print("<html><body>")
    print("sending partial response ...")

    sys.stdout.flush()

    # ずっとスリープ
    while True:
        time.sleep(60)
