#! /usr/bin/python3

import sys
import socket
import select


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
                print("recv() server closed connection", file=sys.stderr)
                return ""
            response += buffer
            if len(buffer) < BUFFER_SIZE:
                break
    except sock.timeout:
        print("recv() timeout", file=sys.stderr)
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


def recv_response(cli_sock, request_num):
    response = receive_full_response(cli_sock)
    if not response:
        return ""
    if request_num == 0:
        print(f'response: "{response}"', flush=True)
    return response


def watch_events(cli_sock, request_num, request=""):
    if not hasattr(watch_events, "cnt"):
        watch_events.cnt = 0
    inputs = [cli_sock]
    if request_num == 0:
        inputs.append(sys.stdin)
    else:
        cli_sock.sendall(request.encode("utf-8"))
        # print(f'request sent: "{request}"')
    responses = ""
    timeout = 1.0 if request_num != 0 else None
    try:
        while True:
            readable, _, _ = select.select(inputs, [], [], timeout)
            if not readable:
                return responses
            for sock in readable:
                if sock is sys.stdin:
                    send_request(cli_sock)
                elif sock is cli_sock:
                    buffer = recv_response(cli_sock, request_num)
                    # print(f'buffer:"{buffer}"')
                    if not buffer:
                        return
                    watch_events.cnt += 1
                    responses += buffer
                    # print(f"cnt={watch_events.cnt}")
                    if request_num != 0 and watch_events.cnt >= request_num:
                        return responses
    finally:
        watch_events.cnt = 0


def spawn_client(ip_address, port, request_num=0, request=""):
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((ip_address, port))
    responses = watch_events(client_socket, request_num, request)
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
    # print(
    #     spawn_client(
    #         ip_address,
    #         port,
    #         2,
    #         "GET /test/integration_test/test_files/multiple_requests/index.py HTTP/1.1\nhost:tt\n\nGET /test/integration_test/test_files/multiple_requests/index.py HTTP/1.1\nhost:tt\n\n",
    #     )
    # )
    return 1


if __name__ == "__main__":
    main()
