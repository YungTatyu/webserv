#!/usr/bin/env python3
import os
import subprocess
import signal
import time
import sys

"""
～～テスト概要～～
1. テストするサーバーごとにwebservを実行する
2. テストケースごとにclientを実行し、webservにリクエストをそもそも送らないか、一部だけ送って途中で止める。
3. 想定receivetimeout時間分sleepし、client processがtimeoutされているかどうか確認する
"""

# init variable
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
WEBSERV_PATH = os.path.join(SCRIPT_DIR, "../../webserv")
CLIENT_PATH = os.path.join(SCRIPT_DIR, "test_files/TimeoutTestFiles/request_sender.py")
TEST_NAME = "CGIRecvTimeout Test"
TOTAL_TESTS = PASSED_TESTS = FAILED_TESTS = 0
SCHEME = "http"
HOST = "127.0.0.1"
PORT = "4400"
GREEN = "\033[32m"
RED = "\033[31m"
RESET = "\033[0m"
WEBSERV_PROCESS = None
CLIENT_PROCESS = None


def init():
    # make webserv
    print(f"{GREEN}make executable ......{RESET}\n")
    subprocess.run(
        ["make", "-j", "-C", f"{SCRIPT_DIR}/../../"],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    if not os.path.exists(WEBSERV_PATH):
        print("Build webserv failed")
        sys.exit(1)
    print(f"|------------------ {TEST_NAME} start ------------------|\n")
    signal.signal(signal.SIGHUP, signal_handler)
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGQUIT, signal_handler)
    signal.signal(signal.SIGABRT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)


def clean(color):
    print(f"{color}make fclean webserv & client.{RESET}")
    subprocess.run(
        ["make", "fclean", "-C", f"{SCRIPT_DIR}/../../"],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )


def kill_process(target_name, target_process, color):
    if target_process:
        target_process.kill()
        print_err(f"{color}kill {target_name}.{RESET}")


def signal_handler(sig, frame):
    print_err(f"\n\n{RED}{TEST_NAME} interrupted: Signal received.{RESET}")
    kill_process("webserv", WEBSERV_PROCESS, f"{RED}")
    kill_process("client", CLIENT_PROCESS, f"{RED}")
    clean(f"{RED}")
    sys.exit(1)


def print_log():
    print(f"\n|------------------ {TEST_NAME} results ------------------|\n")
    print(f"[========]    {TOTAL_TESTS} tests ran")
    print(f"[ {GREEN}PASSED{RESET} ]    {PASSED_TESTS} tests")
    print(f"[ {RED}FAILED{RESET} ]    {FAILED_TESTS} tests")


def print_err(msg):
    print(msg, file=sys.stderr)


def run_server(conf):
    global WEBSERV_PROCESS
    WEBSERV_PROCESS = subprocess.Popen(
        [WEBSERV_PATH, conf], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL
    )
    time.sleep(1)


def run_client(
    client_executable, server_ip, server_port, sleep_time, request
):
    global CLIENT_PROCESS
    CLIENT_PROCESS = subprocess.Popen(
        [
            client_executable,
            server_ip,
            server_port,
            request,
        ],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )


def assert_test(uri, expect_sec, expect_result, client_executable, executable_name):
    global TOTAL_TESTS, PASSED_TESTS, FAILED_TESTS
    TOTAL_TESTS += 1
    url = f"{SCHEME}://{HOST}:{PORT}{uri}"
    request = f"GET {uri} HTTP/1.1i\nHost: \n\n"

    print(f"[  test{TOTAL_TESTS}  ]\n{url}: ", end="")

    # program 実行
    run_client(client_executable, HOST, PORT, request)
    time.sleep(expect_sec + 1)

    # 判定
    client_running = (
        subprocess.run(
            ["pgrep", "-f", executable_name],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        ).returncode
        == 0
    )
    if not client_running:
        if expect_result:
            print(f"{GREEN}passed.{RESET}\nServer closed the connection")
            PASSED_TESTS += 1
        else:
            print_err(f"{RED}failed.{RESET}\nServer closed the connection")
            FAILED_TESTS += 1
    else:
        subprocess.run(
            ["pkill", "-f", executable_name],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
        if expect_result:
            print_err(f"{RED}failed.{RESET}\nServer did not timeout")
            FAILED_TESTS += 1
        else:
            print(f"{GREEN}passed.{RESET}\nServer did not timeout\n")
            PASSED_TESTS += 1
    print()


def run_test(conf, server_name):
    root = os.path.join(SCRIPT_DIR, "test/integration_test/test_files/TimeoutTestFiles")

    print(f"\n{GREEN}<<< {server_name} server test >>>{RESET}")
    run_server(os.path.join(root, conf))

    # テスト実行
    assert_test("/timeout0/", 0, True, CLIENT_PATH, "request_sender.py")
    assert_test("/timeout3/", 3, True, CLIENT_PATH, "request_sender.py")
    assert_test("/timeout6/", 6, True, CLIENT_PATH, "request_sender.py")
    assert_test("/timeout3/", 1, False, CLIENT_PATH, "request_sender.py")
    assert_test("/timeout6/", 4, False, CLIENT_PATH, "request_sender.py")
    assert_test("/no-send/", 3, True, CLIENT_PATH, "request_sender.py")

    # サーバープロセスを終了
    WEBSERV_PROCESS.kill()


def main():
    init()

    run_test("cgi_recv.conf", "kqueue or epoll")  # kqueue or epoll
    run_test("cgi_recv_poll.conf", "poll")  # poll
    run_test("cgi_recv_select.conf", "select")  # select

    print_log()

    clean(RESET)

    if FAILED_TESTS != 0:
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
