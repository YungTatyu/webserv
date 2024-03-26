#! /usr/bin/python3

import os

def	main():
      print("Content-type: text/html\r\nStatus: 200 OK\r\n\r\n", end="")
      method = os.environ.get("METHOD")
      if method == "POST":
            body = input()
            print(input, end="")


if __name__ == "__main__":
       main()