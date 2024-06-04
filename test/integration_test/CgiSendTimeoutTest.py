#!/usr/bin/env python3
import os
import subprocess
import signal
import time
import sys
from CgiRecvTimeoutTest.py import init, clean, kill_process, signal_handler, print_log, print_err, run_server, run_client

"""
～～テスト概要～～
1. テストするサーバーごとにwebservを実行する
2. テストケースごとにclientプログラムかをcgiを実行するリクエストを送る。そのcgiに対してわざと大きなデータを送りバッファを詰まらせる
3. 想定receivetimeout時間分sleepし、client processがtimeoutされているかどうか確認する
"""

# init variable
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
WEBSERV_PATH = os.path.join(SCRIPT_DIR, "../../webserv")
CLIENT_PATH = os.path.join(SCRIPT_DIR, "test_files/TimeoutTestFiles/request_sender.py")
TEST_NAME = "CGIRecvTimeout Test"
TOTAL_TESTS = PASSED_TESTS = FAILED_TESTS = 0
GREEN = "\033[32m"
RED = "\033[31m"
RESET = "\033[0m"

def assert_test(uri, expect_sec, expect_result, client_executable, executable_name):
    SCHEME = "http"
    HOST = "127.0.0.1"
    PORT = "4400"
    global TOTAL_TESTS, PASSED_TESTS, FAILED_TESTS
    TOTAL_TESTS += 1
    url = f"{SCHEME}://{HOST}:{PORT}{uri}"
    request = f"GET {uri} HTTP/1.1i\nHost: \n\n"
    # 大きな文字列をbodyにいれる
    with open('big_buf.html', 'r') as file:
        body = file.read()
        request += body

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
    root = os.path.join(SCRIPT_DIR, "test/integration_test/test_files/TimeoutTestFiles/")

    print(f"\n{GREEN}<<< {server_name} server test >>>{RESET}")
    run_server(os.path.join(root, conf))

    # テスト実行
    assert_test("/timeout0/", 0, True, CLIENT_PATH, "request_sender.py")
    assert_test("/timeout3/", 3, True, CLIENT_PATH, "request_sender.py")
    assert_test("/timeout6/", 6, True, CLIENT_PATH, "request_sender.py")
    assert_test("/timeout3/", 1, False, CLIENT_PATH, "request_sender.py")
    assert_test("/timeout6/", 4, False, CLIENT_PATH, "request_sender.py")

    # サーバープロセスを終了
    WEBSERV_PROCESS.kill()


def main():
    init()

    run_test("cgi_send.conf", "kqueue or epoll")  # kqueue or epoll
    run_test("cgi_send_poll.conf", "poll")  # poll
    run_test("cgi_send_select.conf", "select")  # select

    print_log()

    clean(RESET)

    if FAILED_TESTS != 0:
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
