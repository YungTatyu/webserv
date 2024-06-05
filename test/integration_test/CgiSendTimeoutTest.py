#!/usr/bin/env python3
import os
import sys
from CgiRecvTimeoutTest import init, clean, kill_process, signal_handler, print_log, print_err, run_server, run_client, assert_test

"""
～～テスト概要～～
1. テストするサーバーごとにwebservを実行する
2. テストケースごとにclientプログラムかをcgiを実行するリクエストを送る。そのcgiに対してわざと大きなデータを送りバッファを詰まらせる
3. 想定receivetimeout時間分sleepし、client processがtimeoutされているかどうか確認する
"""

# init variable
FILE_PATH = os.path.join(SCRIPT_DIR, "test_files/TimeoutTestFiles/html/big_buf.html")


def run_test(conf, server_name):
    root = os.path.join(SCRIPT_DIR, "test/integration_test/test_files/TimeoutTestFiles/")
    # 大きな文字列をbodyにいれる
    body = ""
    with open(f"{FILE_PATH}", 'r') as file:
        body = file.read()

    print(f"\n{GREEN}<<< {server_name} server test >>>{RESET}")
    run_server(os.path.join(root, conf))

    # テスト実行
    assert_test("/timeout0/", body, 0, True, CLIENT_PATH, "request_sender.py")
    assert_test("/timeout3/", body, 3, True, CLIENT_PATH, "request_sender.py")
    assert_test("/timeout6/", body, 6, True, CLIENT_PATH, "request_sender.py")
    assert_test("/timeout3/", body, 1, False, CLIENT_PATH, "request_sender.py")
    assert_test("/timeout6/", body, 4, False, CLIENT_PATH, "request_sender.py")

    # サーバープロセスを終了
    WEBSERV_PROCESS.kill()


def main():
    test_name = "CgiSendTimeoutTest"
    init(test_name)

    run_test("cgi_send.conf", "kqueue or epoll")  # kqueue or epoll
    run_test("cgi_send_poll.conf", "poll")  # poll
    run_test("cgi_send_select.conf", "select")  # select

    print_log(test_name)

    clean(RESET)

    if FAILED_TESTS != 0:
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
