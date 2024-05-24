#! /usr/bin/python3

import sys
import socket
import select


def replace_escape_sequences(input_str):
    escape_sequences = {r"\\n": "\n", r"\\r": "\r", r"\\t": "\t", r"\\\\": "\\"}

    for esc_seq, real_char in escape_sequences.items():
        input_str = input_str.replace(esc_seq, real_char)
    return input_str


def readline():
    message = sys.stdin.readline()
    return message[:-1]  # 最後の改行を削除


def send_request(cli_sock):
    print("request: ", end="", file=sys.stderr, flush=True)
    message = readline()
    message = replace_escape_sequences(message)
    cli_sock.sendall(message.encode("utf-8"))
    print(f'"{message}" sent', file=sys.stderr)


def recv_response(cli_sock):
    response = cli_sock.recv(1024)
    if not response:
        print("Server closed the connection.")
        sys.exit(1)
    print(f'response: "{response.decode()}"', flush=True)


def watch_events(cli_sock):
    inputs = [cli_sock, sys.stdin]
    while True:
        readable, _, _ = select.select(inputs, [], [])
        for sock in readable:
            if sock is sys.stdin:
                send_request(cli_sock)
            elif sock is cli_sock:
                recv_response(cli_sock)


def main():
    if len(sys.argv) < 3:
        print("Usage: python client.py <ip_address> <port>", file=sys.stderr)
        sys.exit(1)

    ip_address = "127.0.0.1" if sys.argv[1] == "localhost" else sys.argv[1]
    port = int(sys.argv[2])

    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((ip_address, port))
    watch_events(client_socket)
    return 1


if __name__ == "__main__":
    main()
