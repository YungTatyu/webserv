import socket
import time
import sys


def send_requests(address, port, request):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((address, port))
        s.sendall(request.encode())
        response = s.recv(1024)
        print(f"Received response: {response.decode()}")

    print("Finished sending requests and sleeping.")


if __name__ == "__main__":
    address = sys.argv[1]
    port = int(sys.argv[2])
    request = sys.argv[3]

    send_requests(address, port, request)
