#!/usr/bin/python3

import time


def main():
    # ずっとスリープ
    time.sleep(60)
    print("Status: 200\r\nContent-Type: text/html\r\n\r\n", end="")


if __name__ == "__main__":
    main()
