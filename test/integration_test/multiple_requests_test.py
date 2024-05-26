#! /usr/bin/python3

import pytest
import os
import sys
import time
import subprocess

from client import spawn_client

ROOT = "test/integration_test/test_files/multiple_requests"  # test file dir
REQUESTS = "requests"
REQUEST_NUM = "request_num"
ADDRESS = "address"
PORT = "port"
EXPECTS = "expects"


def run_server(webserv, conf):
    try:
        server_proc = subprocess.Popen([webserv, conf])
        time.sleep(0.1)  # server起動を待つ
        return server_proc
    except Exception as e:
        print(f"{e}", file=sys.stderr)
        sys.exit(1)


def parse_status_code(response):
    status_line = response.splitlines()[0].strip()
    status_code = int(status_line.split()[1])
    return status_code


def expect_status(res, expects):
    actuals = [parse_status_code(status) for status in res]
    for expect, actual in zip(expects, actuals):
        assert expect == actual


def run_test(conf, test_data):
    CWD = os.path.dirname(os.path.abspath(__file__))
    PATH_WEBSERV = f"{CWD}/../../webserv"

    WEBSERV = run_server(PATH_WEBSERV, f"{ROOT}/{conf}")
    for i in range(len(test_data[REQUESTS])):
        request = test_data[REQUESTS][i]
        request_num = test_data[REQUEST_NUM][i]
        address = test_data[ADDRESS]
        port = test_data[PORT]
        expect = test_data[EXPECTS][i]
        res = spawn_client(address, port, request_num, request)
        try:
            expect_status(res, expect)
        except Exception as e:
            print(f"{e}")
            break

    WEBSERV.kill()


@pytest.mark.parametrize(
    "conf",
    ["test.conf", "test_poll.conf", "test_select.conf"],
)
def test(conf):
    test_data = {
        REQUESTS: [
            f"GET {ROOT}/index.html, HTTP/1.1\nhost:tt\n\nPOST {ROOT}/index.py, HTTP/1.1\nhost:tt\n\n",
            f"POST {ROOT}/index.py, HTTP/1.1\nhost:tt\n\nDELETE {ROOT}/index.html, HTTP/1.1\nhost:tt\n\n",
        ],
        REQUEST_NUM: [2, 2],
        EXPECTS: [[200, 302], [302, 200]],
        ADDRESS: "127.0.0.1",
        PORT: 8000,
    }
    run_test(conf, test_data)
