#! /usr/bin/python3

import os


def main():
    print("content-type: text/html\r\nStatus: 200 OK\r\n\r\n", end="")
    method = os.environ.get("METHOD")
    # body = input()
    # print(body, end="")


if __name__ == "__main__":
    main()
