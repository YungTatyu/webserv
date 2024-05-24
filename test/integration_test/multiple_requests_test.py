#! /usr/bin/python3

import os
import sys
import subprocess
import time
import socket 

def run_server(webserv, conf):
    try:
        server_proc = subprocess.Popen([webserv, conf])
        time.sleep(0.1)  # server起動を待つ
        return server_proc
    except Exception as e:
        print(f"{e}", file=sys.stderr)
        sys.exit(1)

def receive_full_response(sock):
    buffer = b""
    while True:
        data = sock.recv(4096)
        buffer += data
    return buffer

def send_request():
    request = (
        "GET /html/index.html HTTP/1.1\r\n"
        "Host: tt\r\n"
        "Content-Length: 2\r\n"
        "\r\n"
        "itGET /html/index.html HTTP/1.1\r\n"
        "Host: tt\r\n"
        "\r\n"
        "\r\n"
    )
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect(("localhost", 8005))
        s.sendall(request.encode())

        response = receive_full_response(s)
        print(response)

        #        response_text1 = s.recv(4096)
        #print(f"Response:\n{response_text1}")

        #response_text2 = s.recv(4096)
        #print(f"Response:\n{response_text2}")

def main():
    CWD = os.path.dirname(os.path.abspath(__file__))
    PATH_WEBSERV = f"{CWD}/../../webserv"
    WEBSERV = run_server(PATH_WEBSERV, "conf/webserv.conf")
    send_request()
    WEBSERV.kill()

if __name__ == "__main__":
    main()
