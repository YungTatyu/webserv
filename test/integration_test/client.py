#! /usr/bin/python3

import sys
import socket
import select

g_test = False


# 特殊文字を置換
def replace_escape_sequences(input_str):
    escape_sequences = {r"\n": "\n", r"\r": "\r", r"\t": "\t", r"\\": "\\", r"\0": "\0"}

    for esc_seq, real_char in escape_sequences.items():
        input_str = input_str.replace(esc_seq, real_char)
    return input_str


def readline():
    message = sys.stdin.readline()
    return message[:-1]  # 最後の改行を削除


def receive_full_response(sock, connection, timeout=3):
    sock.settimeout(timeout)
    BUFFER_SIZE = 1024
    response = b""

    try:
        while True:
            buffer = sock.recv(BUFFER_SIZE)
            if len(buffer) == 0:
                print("recv() server closed connection", file=sys.stderr)
                if g_test and connection == "alive":
                    raise RuntimeError("server closed connection.")
                return ""
            response += buffer
            if len(buffer) < BUFFER_SIZE:
                break
    except sock.timeout:
        print("recv() timeout", file=sys.stderr)
        return ""
    return response.decode("utf-8")


def send_request(cli_sock):
    message = readline()
    message = replace_escape_sequences(message)
    if not message:
        return
    print(f'sending "{message}"', file=sys.stderr, flush=True)
    cli_sock.sendall(message.encode("utf-8"))


def recv_response(cli_sock, connection):
    response = receive_full_response(cli_sock, connection)
    if not response:
        return ""
    if g_test == False:
        print(f'response: "{response}"', flush=True)
    return response


def watch_events(cli_sock, request, connection):
    res_cnt = 0
    responses = ""
    timeout = 1.0 if g_test else None
    inputs = [cli_sock]
    if request:
        cli_sock.sendall(request.encode("utf-8"))
    else:  # 標準入力からリクエストを受信する場合
        inputs.append(sys.stdin)
    while True:
        readable, _, _ = select.select(inputs, [], [], timeout)
        if not readable:  # timeout event
            return responses
        for sock in readable:
            if sock is sys.stdin:
                send_request(cli_sock)
            elif sock is cli_sock:
                buffer = recv_response(cli_sock, connection)
                if not buffer:
                    return responses
                res_cnt += 1
                responses += buffer


def spawn_client(ip_address, port, request="", connection=True):
    global g_test
    if request:
        g_test = True
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((ip_address, port))
    responses = watch_events(client_socket, request, connection)
    # print(watch_events(client_socket, request_num, request))
    client_socket.close()
    return responses


def main():
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} <ip_address> <port>", file=sys.stderr)
        sys.exit(1)

    ip_address = "127.0.0.1" if sys.argv[1] == "localhost" else sys.argv[1]
    port = int(sys.argv[2])
    spawn_client(ip_address, port)
    return 1


if __name__ == "__main__":
    main()
