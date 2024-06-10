#!/usr/bin/python3
import socket
import time
import sys
import os


# 特殊文字を置換
def replace_escape_sequences(input_str):
    escape_sequences = {r"\n": "\n", r"\r": "\r", r"\t": "\t", r"\\": "\\", r"\0": "\0"}

    for esc_seq, real_char in escape_sequences.items():
        input_str = input_str.replace(esc_seq, real_char)
    return input_str


def send_requests(address, port, request, body_file):
    body = ""
    if body_file and os.path.exists(body_file) and os.path.getsize(body_file) > 0:
        with open(body_file, "r") as f:
            body += f.read()

    if body:
        request += f"Content-Length: {len(body)}\r\n"
    request += "\r\n"
    print("request", request)
    request += body
    request = replace_escape_sequences(request)

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((address, port))
        s.sendall(request.encode())
        print("Finished sending requests.")
        while True:
            try:
                response = s.recv(1024)
                if not response:
                    break
                print(f"Received response: \n{response.decode()}")
            except ConnectionResetError:
                # サーバーが接続をリセットした場合
                print("Connection reset by peer.")
                break
            except Exception as e:
                # その他のエラーをキャッチして出力
                print("An error occurred:", e)
                break


if __name__ == "__main__":
    address = sys.argv[1]
    port = int(sys.argv[2])
    request = sys.argv[3]
    body_file = sys.argv[4]

    send_requests(address, port, request, body_file)
