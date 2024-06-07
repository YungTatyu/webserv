#!/usr/bin/python3
import os
import sys
from CgiRecvTimeoutTest import (
    init,
    kill_process,
    signal_handler,
    print_log,
    print_err,
    run_server,
    run_client,
    assert_test,
    run_test,
)

"""
～～テスト概要～～
1. テストするサーバーごとにwebservを実行する
2. テストケースごとにclientプログラムかをcgiを実行するリクエストを送る。そのcgiに対してわざと大きなデータを送りバッファを詰まらせる
3. 想定receivetimeout時間分sleepし、client processがtimeoutされているかどうか確認する
"""

# init variable
ROOT_PATH = os.path.dirname(os.path.abspath(__file__))
CLIENT_PATH = os.path.join(ROOT_PATH, "test_files/TimeoutTestFiles/client.py")
GREEN = "\033[32m"
RED = "\033[31m"
RESET = "\033[0m"
DISCONNECT = True
STAY_CONNECT = False


def main():
    test_name = "KeepaliveTimeoutTest"
    init(test_name)

    body_path = "none"

    test_cases = [
        (
            "GET",
            "/timeout0/",
            body_path,
            4242,
            0,
            DISCONNECT,
            CLIENT_PATH,
            "client.py",
        ),
        (
            "GET",
            "/timeout3/",
            body_path,
            4242,
            3,
            DISCONNECT,
            CLIENT_PATH,
            "client.py",
        ),
        (
            "GET",
            "/timeout6/",
            body_path,
            4242,
            6,
            DISCONNECT,
            CLIENT_PATH,
            "client.py",
        ),
        (
            "GET",
            "/timeout3/",
            body_path,
            4242,
            1,
            STAY_CONNECT,
            CLIENT_PATH,
            "client.py",
        ),
        (
            "GET",
            "/timeout6/",
            body_path,
            4242,
            4,
            STAY_CONNECT,
            CLIENT_PATH,
            "client.py",
        ),
    ]

    run_test("keepalive_timeout.conf", "kqueue or epoll", test_cases)  # kqueue or epoll
    run_test("keepalive_timeout_poll.conf", "poll", test_cases)  # poll
    run_test("keepalive_timeout_select.conf", "select", test_cases)  # select

    failed = print_log(test_name)

    if failed != 0:
        return 1


if __name__ == "__main__":
    main()
