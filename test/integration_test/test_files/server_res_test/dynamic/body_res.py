#! /usr/bin/python3

import os
import sys

def	main():
      print("Status: 200\r\nContent-Type: text/html\r\n\r\n", end="")
      kLength = os.environ.get("CONTENT_LENGTH")
      if kLength is None:
            print("Status: 400\r\n\r\n", end="")
            exit(1)
      body = sys.stdin.read(int(kLength))
      print(body, end="")

if __name__ == "__main__":
       main()