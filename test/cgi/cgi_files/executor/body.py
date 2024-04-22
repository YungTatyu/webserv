#! /usr/bin/python3

import os
import sys


def printMetaVars():
    print("~~~ ALL ENVIRON ~~~", file=sys.stderr)
    for key, value in os.environ.items():
        print(key, ": ", value, file=sys.stderr)
    print("~~~ ALL ENVIRON printed ~~~", file=sys.stderr)


def main():
    print("Status: 200\r\nContent-Type: text/html\r\n\r\n", end="")
    # printMetaVars()
    kLength = os.environ.get("CONTENT_LENGTH")
    if kLength is None:
        print("CONTENT_LENGTH not found")
        exit(1)
    body = sys.stdin.read(int(kLength))
    print(body, end="")


if __name__ == "__main__":
    main()
