#! /usr/bin/python3

import pytest
import os
import sys
import time
import subprocess
import re

from client import spawn_client
from itertools import zip_longest

TEST_DIR = "test_files/multiple_requests"
ROOT = f"test/integration_test/{TEST_DIR}"  # test file dir
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
    # status_line = response.splitlines()[0].strip()
    # status_code = int(status_line.split()[1])
    # return status_code
    pattern = re.compile(r"^HTTP/1\.1 (\d+) .+$", re.MULTILINE)
    status_codes = [int(code) for code in pattern.findall(response)]
    return status_codes


def expect_status(res, expects):
    actuals = parse_status_code(res)
    for expect, actual in zip_longest(expects, actuals, fillvalue=None):
        # print(f"expect={expect}")
        # print(f"actual={actual}")
        assert expect == actual, f'expected: "{expect}"\nactual  : "{actual}"'


def run_test(conf, test_data):
    CWD = os.path.dirname(os.path.abspath(__file__))
    PATH_WEBSERV = f"{CWD}/../../webserv"

    WEBSERV = run_server(PATH_WEBSERV, f"{CWD}/{TEST_DIR}/{conf}")
    try:
        for i in range(len(test_data[REQUESTS])):
            request = test_data[REQUESTS][i]
            request_num = test_data[REQUEST_NUM][i]
            address = test_data[ADDRESS]
            port = test_data[PORT]
            expect = test_data[EXPECTS][i]
            res = spawn_client(address, port, request_num, request)
            assert res is not None, "Response cannot be None"
            print(f'response: "{res}"')
            expect_status(res, expect)
    finally:
        WEBSERV.kill()


@pytest.mark.parametrize(
    "conf",
    ["test.conf", "test_poll.conf", "test_select.conf"],
)
def test(conf):
    test_data = {
        REQUESTS: [
            f"GET /{ROOT}/index.html HTTP/1.1\nhost:tt\n\nPOST /{ROOT}/index.py HTTP/1.1\nhost:tt\n\n",  # static res test
            f"POST /{ROOT}/index.py HTTP/1.1\nhost:tt\n\nGET /{ROOT}/index.html HTTP/1.1\nhost:tt\n\n",  # dynamic, static res test
            f"POST /{ROOT}/index.py HTTP/1.1\nhost:tt\n\nDELETE /{ROOT}/index.py HTTP/1.1\nhost:tt\n\n",  # dynamic res test
            f"POST /{ROOT}/index.py HTTP/1.1\nhost:tt\ncontent-length: 3\n\n333GET /{ROOT}/index.html HTTP/1.1\nhost:tt\n\n",  # content-length test
            f"GET /{ROOT}/index.py HTTP/1.1\nhost:tt\ntransfer-encoding: chunked\n\n3\n333\n0\n\nGET /{ROOT}/index.html HTTP/1.1\nhost:tt\n\n",  # chunked test
            f"POST /{ROOT}/index.py HTTP/1.1\nhost:tt\n\nDELETE ",  # dynamic res with uncomplite request
            f"GET /{ROOT}/index.html HTTP/1.1\nhost:tt\n\nDELETE ",  # dynamic res with uncomplite request
            f"GET /{ROOT}/not_found.html HTTP/1.1\nhost:tt\n\nGET /{ROOT}/index.html HTTP/1.1\nhost:tt\n\n",  # error status code
            f"POST /{ROOT}/index.py HTTP/1.1\nhost:tt\ncontent-length: 12\n\nthis is test"
            + f"GET /{ROOT}/index.html HTTP/1.1\nhost:tt\n\n"
            + f"GET /{ROOT}/index.py HTTP/1.1\nhost:tt\ntransfer-encoding: chunked\n\nd\nthis is body.\n0\n\n",  # d is 13 bytes in hex
            # three requests test
        ],
        REQUEST_NUM: [
            2,
            2,
            2,
            2,
            2,
            1,
            1,
            1,
            3,
        ],
        EXPECTS: [
            [200, 302],
            [302, 200],
            [302, 302],
            [302, 200],
            [302, 200],
            [302],
            [200],
            [404],
            [302, 200, 302],
        ],
        ADDRESS: "127.0.0.1",
        PORT: 4242,
    }
    run_test(conf, test_data)
