#! /usr/bin/python3

import sys
import socket
import select
import os
import time


def replace_escape_sequences(input_str):
    escape_sequences = {r"\n": "\n", r"\r": "\r", r"\t": "\t", r"\\": "\\", r"\0": "\0"}

    for esc_seq, real_char in escape_sequences.items():
        input_str = input_str.replace(esc_seq, real_char)
    return input_str


def readline():
    message = sys.stdin.readline()
    return message[:-1]  # 最後の改行を削除


def receive_full_response(sock, timeout=3):
    sock.settimeout(timeout)
    BUFFER_SIZE = 1024
    response = b""

    try:
        while True:
            buffer = sock.recv(BUFFER_SIZE)
            if len(buffer) == 0:
                print("server closed connection")
                return ""
            response += buffer
            if len(buffer) < BUFFER_SIZE:
                break
    except sock.timeout:
        print("recv() timeout")
        return ""
    return response.decode("utf-8")


def send_request(cli_sock):
    # どうしてもbufferがフラッシュされないのでコメントアウト
    # print("request: ", end="", file=sys.stderr, flush=True)
    # os.fsync(sys.stderr.fileno())
    # time.sleep(0.1)
    message = readline()
    message = replace_escape_sequences(message)
    if not message:
        return
    print(f'sending "{message}"', file=sys.stderr, flush=True)
    cli_sock.sendall(message.encode("utf-8"))


def recv_response(cli_sock):
    response = receive_full_response(cli_sock)
    if not response:
        sys.exit(1)
    print(f'response: "{response}"', flush=True)


def watch_events(cli_sock):
    inputs = [cli_sock, sys.stdin]
    while True:
        readable, _, _ = select.select(inputs, [], [])
        for sock in readable:
            if sock is sys.stdin:
                send_request(cli_sock)
            elif sock is cli_sock:
                recv_response(cli_sock)


def send_and_recv(cli_sock, request):
    cli_sock.sendall(request.encode("utf-8"))
    return receive_full_response(cli_sock)


def client(ip_address, port, request=""):
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((ip_address, port))
    if request:
        return print(send_and_recv(client_socket, request))
    watch_events(client_socket)


def main():
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} <ip_address> <port>", file=sys.stderr)
        sys.exit(1)

    ip_address = "127.0.0.1" if sys.argv[1] == "localhost" else sys.argv[1]
    port = int(sys.argv[2])
    client(ip_address, port)
    return 1


if __name__ == "__main__":
    main()
