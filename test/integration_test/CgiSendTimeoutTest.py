#!/usr/bin/env python3
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
CLIENT_PATH = os.path.join(ROOT_PATH, "test_files/TimeoutTestFiles/request_sender.py")
FILE_PATH = os.path.join(ROOT_PATH, "test_files/TimeoutTestFiles/html/big_buf.html")
GREEN = "\033[32m"
RED = "\033[31m"
RESET = "\033[0m"


def main():
    test_name = "CgiSendTimeoutTest"
    init(test_name)

    body_path = f"{FILE_PATH}"

    test_cases = [
        (
            "POST",
            "/timeout0/",
            body_path,
            4700,
            0,
            True,
            CLIENT_PATH,
            "request_sender.py",
        ),
        (
            "POST",
            "/timeout3/",
            body_path,
            4700,
            3,
            True,
            CLIENT_PATH,
            "request_sender.py",
        ),
        (
            "POST",
            "/timeout6/",
            body_path,
            4700,
            6,
            True,
            CLIENT_PATH,
            "request_sender.py",
        ),
        (
            "POST",
            "/timeout3/",
            body_path,
            4700,
            1,
            False,
            CLIENT_PATH,
            "request_sender.py",
        ),
        (
            "POST",
            "/timeout6/",
            body_path,
            4700,
            4,
            False,
            CLIENT_PATH,
            "request_sender.py",
        ),
    ]

    run_test("cgi_send.conf", "kqueue or epoll", test_cases)  # kqueue or epoll
    run_test("cgi_send_poll.conf", "poll", test_cases)  # poll
    run_test("cgi_send_select.conf", "select", test_cases)  # select

    failed = print_log(test_name)

    if failed != 0:
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
