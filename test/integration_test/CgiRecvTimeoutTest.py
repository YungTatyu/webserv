#!/usr/bin/env python3
import os
import subprocess
import signal
import time
import sys

"""
～～テスト概要～～
1. テストするサーバーごとにwebservを実行する
2. テストケースごとにclientプログラムかをcgiを実行するリクエストを送る。そのcgiはわざとserverにレスポンスを送るのを遅らせる。
3. 想定receivetimeout時間分sleepし、client processがtimeoutされているかどうか確認する
"""


# init variable
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
WEBSERV_PATH = os.path.join(SCRIPT_DIR, "../../webserv")
CLIENT_PATH = os.path.join(SCRIPT_DIR, "test_files/TimeoutTestFiles/request_sender.py")
CLIENT_NAME = "request_sender.py"
CGI_NAME1 = "send_partial.cgi"
CGI_NAME2 = "sleeping.cgi"
GREEN = "\033[32m"
RED = "\033[31m"
RESET = "\033[0m"
DISCONNECT = True
STAY_CONNECT = False
g_total_tests = g_passed_tests = g_failed_tests = 0
g_webserv_pid = None
g_client_process = None


def init(test_name):
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

    print(f"|------------------ {test_name} start ------------------|\n")

    signal.signal(signal.SIGHUP, signal_handler)
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGQUIT, signal_handler)
    signal.signal(signal.SIGABRT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)


def kill_process(target_name, target_process, color):
    if target_process:
        target_process.kill()
        print_err(f"{color}kill {target_name}.{RESET}")


def kill_by_name(target_name):
    try:
        result = subprocess.run(["pgrep", "-f", target_name], stdout=subprocess.PIPE)
        pids = result.stdout.decode().split()
        for pid in pids:
            os.kill(int(pid), signal.SIGKILL)

    except Exception as e:
        print(f"{RED}Failed to kill process {target_name}: {e}{RESET}")


def signal_handler(sig, frame):
    print_err(f"\n\n{RED} interrupted: Signal received.{RESET}")
    kill_process("webserv", g_webserv_pid, f"{RED}")
    kill_process("client", g_client_process, f"{RED}")
    sys.exit(1)


def print_log(test_name):
    print(f"\n|------------------ {test_name} results ------------------|\n")
    print(f"[========]    {g_total_tests} tests ran")
    print(f"[ {GREEN}PASSED{RESET} ]    {g_passed_tests} tests")
    print(f"[ {RED}FAILED{RESET} ]    {g_failed_tests} tests")

    return g_failed_tests


def print_err(msg):
    print(msg, file=sys.stderr)


def run_server(conf):
    global g_webserv_pid
    try:
        g_webserv_pid = subprocess.Popen(
            [WEBSERV_PATH, conf], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL
        )
        time.sleep(1)
    except Exception as e:
        print(f"{e}", file=sys.stderr)
        sys.exit(1)


def run_client(client_executable, server_ip, server_port, request, body_path):
    global g_client_process
    try:
        g_client_process = subprocess.Popen(
            [
                sys.executable,
                client_executable,
                server_ip,
                str(server_port),
                request,
                body_path,
            ],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
    except Exception as e:
        print(f"{e}", file=sys.stderr)
        sys.exit(1)


def assert_test(
    method,
    uri,
    body_path,
    port,
    expect_sec,
    expect_result,
    client_executable,
    executable_name,
    cgi_name="none",
):
    global g_total_tests, g_passed_tests, g_failed_tests
    g_total_tests += 1
    scheme = "http"
    host = "127.0.0.1"
    url = f"{scheme}://{host}:{port}{uri}"
    request = f"{method} {uri} HTTP/1.1\nHost: _\n"

    print(f"[  test{g_total_tests}  ]\n{url}: ", end="")

    # program 実行
    run_client(client_executable, host, port, request, body_path)
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

    if not client_running:  # client does't exist
        if expect_result:
            print(f"{GREEN}passed.{RESET}\nServer closed the connection")
            g_passed_tests += 1
        else:
            print_err(f"{RED}failed.{RESET}\nServer closed the connection")
            g_failed_tests += 1
    else:
        kill_by_name(f"{executable_name}")
        kill_by_name(f"{cgi_name}")
        if expect_result:
            print_err(f"{RED}failed.{RESET}\nServer did not timeout")
            g_failed_tests += 1
        else:
            print(f"{GREEN}passed.{RESET}\nServer did not timeout\n")
            g_passed_tests += 1
    print()


def run_test(conf, server_name, test_cases):
    root = os.path.join(SCRIPT_DIR, "test_files/TimeoutTestFiles/")

    print(f"\n{GREEN}<<< {server_name} server test >>>{RESET}")
    run_server(os.path.join(root, conf))

    # テスト実行
    for test_case in test_cases:
        assert_test(*test_case)

    # サーバープロセスを終了
    g_webserv_pid.kill()


def main():
    test_name = "CgiRecvTimeoutTest"
    init(test_name)

    body_path = "none"

    test_cases = [
        (
            "GET",
            "/timeout0/",
            body_path,
            4600,
            2,
            STAY_CONNECT,
            CLIENT_PATH,
            CLIENT_NAME,
            CGI_NAME1,
        ),
        (
            "GET",
            "/timeout3/",
            body_path,
            4600,
            3,
            DISCONNECT,
            CLIENT_PATH,
            CLIENT_NAME,
            CGI_NAME1,
        ),
        (
            "GET",
            "/timeout6/",
            body_path,
            4600,
            6,
            DISCONNECT,
            CLIENT_PATH,
            CLIENT_NAME,
            CGI_NAME1,
        ),
        (
            "GET",
            "/timeout3/",
            body_path,
            4600,
            2,
            STAY_CONNECT,
            CLIENT_PATH,
            CLIENT_NAME,
            CGI_NAME1,
        ),
        (
            "GET",
            "/timeout6/",
            body_path,
            4600,
            4,
            STAY_CONNECT,
            CLIENT_PATH,
            CLIENT_NAME,
            CGI_NAME1,
        ),
        (
            "GET",
            "/no-send/",
            body_path,
            4600,
            3,
            DISCONNECT,
            CLIENT_PATH,
            CLIENT_NAME,
            CGI_NAME2,
        ),
        (
            "GET",
            "/no-send/",
            body_path,
            4600,
            1,
            STAY_CONNECT,
            CLIENT_PATH,
            CLIENT_NAME,
            CGI_NAME2,
        ),
    ]

    run_test("cgi_recv.conf", "kqueue or epoll", test_cases)  # kqueue or epoll
    run_test("cgi_recv_poll.conf", "poll", test_cases)  # poll
    run_test("cgi_recv_select.conf", "select", test_cases)  # select

    print_log(test_name)

    if g_failed_tests != 0:
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
