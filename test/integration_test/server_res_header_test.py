#! /usr/bin/python3

import os
import sys
import time
import subprocess
import requests
import pytest

ROOT = "test/integration_test/test_files/server_res_test"  # test file dir
SERVER = "Server"
DATE = "Date"
CONTENT_LENGTH = "Content-Length"
CONNECTION = "Connection"
KEEP_ALIVE = "keep-alive"
CLOSE = "close"


def run_server(webserv, conf):
    try:
        server_proc = subprocess.Popen([webserv, conf])
        time.sleep(0.1)  # server起動を待つ
        return server_proc
    except Exception as e:
        print(f"{e}", file=sys.stderr)
        sys.exit(1)


def send_request(req_data):
    headers = {"host": req_data["host"], "content-type": req_data["content_type"]}
    req = f"http://localhost:{req_data['port']}/{ROOT}/{req_data['request']}"
    r = requests.get(
        req,
        headers=headers,
        data=f"{req_data['body']}",
        timeout=0.5,
        params=f"{req_data['query_string']}",
    )
    return r


def expect_headers_exist(response):
    expect_headers = [SERVER, DATE, CONTENT_LENGTH, CONNECTION]
    for header in expect_headers:
        assert response.headers.get(header) is not None


def expect_headers(actual, expect):
    for header, value in expect.items():
        assert actual.headers.get(header) == value


def run_test(conf, req_data, test_headers={}):
    CWD = os.path.dirname(os.path.abspath(__file__))
    PATH_WEBSERV = f"{CWD}/../../webserv"
    try:
        WEBSERV = run_server(PATH_WEBSERV, f"{ROOT}/{conf}")
        res = send_request(req_data)
        expects = {
            SERVER: "webserv/1.0",
            CONTENT_LENGTH: f"{len(res.text)}",
            CONNECTION: req_data[CONNECTION],
        }
        """
        任意のheaderをテストに追加可能
        """
        for header, value in test_headers.items():
            expects[header] = value

        expect_headers_exist(res)
        expect_headers(res, expects)
    finally:
        WEBSERV.kill()


# テストの引数に渡される
@pytest.mark.parametrize(
    "conf",
    [
        "server_res_test.conf",
        "server_res_test_poll.conf",
        "server_res_test_select.conf",
    ],
)
# 関数名がtestで始まる関数がテスト実行時に呼ばれる
class TestClass:
    def test_header1(self, conf):
        run_test(
            conf,
            {
                SERVER: "webserv/1.0",
                "host": "test",
                "content_type": "text",
                "body": "this is body message",
                "query_string": "a=a&b=b&c=c",
                "port": 4242,
                "request": "static/index.html",
                CONNECTION: KEEP_ALIVE,
            },
        )

    def test_header2(self, conf):
        run_test(
            conf,
            {
                SERVER: "webserv/1.0",
                "host": "test",
                "content_type": "text",
                "body": "",
                "query_string": "",
                "port": 4242,
                "request": "dynamic/document_response.py",
                CONNECTION: KEEP_ALIVE,
            },
        )

    def test_header3(self, conf):
        run_test(
            conf,
            {
                SERVER: "webserv/1.0",
                "host": "test",
                "content_type": "text",
                "body": "",
                "query_string": "",
                "port": 4242,
                "request": "static/not_found",
                CONNECTION: CLOSE,
            },
        )


@pytest.mark.parametrize(
    "conf_no_keepalive",
    [
        "server_res_no_keepalive_test.conf",
        "server_res_no_keepalive_test_poll.conf",
        "server_res_no_keepalive_test_select.conf",
    ],
)
def test_header4(conf_no_keepalive):
    run_test(
        conf_no_keepalive,
        {
            SERVER: "webserv/1.0",
            "host": "test",
            "content_type": "text",
            "body": "",
            "query_string": "",
            "port": 4242,
            "request": "dynamic/body_res.py",
            CONNECTION: CLOSE,
        },
    )
