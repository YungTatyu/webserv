#! /usr/bin/python3

import os
import sys
import time
import subprocess
import requests
import pytest

ROOT = "test/cgi/cgi_files/executor"  # test file dir
HOST = "host"
CONTENT_TYPE = "content-type"
CONTENT_LENGTH = "content-length"
BODY = "body"
QUERY_STRING = "query-string"
PORT = "port"
CGI_FILE = "cgi-file"
PATH_INFO = "path-info"
HEADERS = "headers"


def run_server(webserv, conf):
    try:
        server_proc = subprocess.Popen([webserv, conf])
        time.sleep(0.1)  # server起動を待つ
        return server_proc
    except Exception as e:
        print(f"{e}", file=sys.stderr)
        sys.exit(1)


def send_request(req_data):
    headers = {HOST: req_data[HOST], CONTENT_TYPE: req_data[CONTENT_TYPE]}
    try:
        for key, value in req_data[HEADERS].items():
            headers[key] = value
    except KeyError:
        pass
    req = f"http://localhost:{req_data[PORT]}/{ROOT}/{req_data[CGI_FILE]}{req_data[PATH_INFO]}?{req_data[QUERY_STRING]}"
    r = requests.get(req, headers=headers, data=f"{req_data[BODY]}", timeout=0.5)
    return r


def expect_status(response, expect):
    assert response.status_code == expect


def expect_body(response, expect):
    assert response.text == expect


def run_test(conf, req_data, expect):
    CWD = os.path.dirname(os.path.abspath(__file__))
    PATH_WEBSERV = f"{CWD}/../../webserv"

    WEBSERV = run_server(PATH_WEBSERV, f"{ROOT}/{conf}")
    try:
        res = send_request(req_data)
        expect_status(res, 200)
        expect_body(res, expect)
    finally:
        WEBSERV.kill()


# テストの引数に渡される
@pytest.mark.parametrize(
    "conf1",
    ["all_meta_vars.conf", "all_meta_vars_poll.conf", "all_meta_vars_select.conf"],
)
# 関数名がtestで始まる関数がテスト実行時に呼ばれる
def test_all_meta_vars1(conf1):
    req_data = {
        HOST: "test",
        CONTENT_TYPE: "text",
        BODY: "this is body message",
        QUERY_STRING: "a=a&b=b&c=c",
        PORT: 4242,
        CGI_FILE: "all_meta_vars.py",
        PATH_INFO: "",
    }
    run_test(
        conf1,
        req_data,
        (
            f"AUTH_TYPE=\n"
            f"CONTENT_LENGTH={len(req_data[BODY])}\n"
            f"CONTENT_TYPE={req_data[CONTENT_TYPE]}\n"
            f"GATEWAY_INTERFACE=CGI/1.1\n"
            f"PATH_INFO={req_data[PATH_INFO]}\n"
            f"PATH_TRANSLATED=\n"
            f"QUERY_STRING={req_data[QUERY_STRING]}\n"
            f"REMOTE_ADDR=127.0.0.1\n"
            f"REMOTE_HOST=127.0.0.1\n"
            # f"REQUEST_METHOD={req_data['method']}\n"
            f"REQUEST_METHOD=GET\n"
            f"SCRIPT_NAME=/{ROOT}/{req_data[CGI_FILE]}\n"
            f"SERVER_NAME={req_data[HOST]}\n"
            f"SERVER_PORT={req_data[PORT]}\n"
            f"SERVER_PROTOCOL=HTTP/1.1\n"
            f"SERVER_SOFTWARE=webserv/1.0\n"
        ),
    )


@pytest.mark.parametrize(
    "conf2",
    ["all_meta_vars.conf", "all_meta_vars_poll.conf", "all_meta_vars_select.conf"],
)
def test_all_meta_vars2(conf2):
    req_data = {
        HOST: "tachu",
        CONTENT_TYPE: "text/html",
        BODY: "body body body   body body body   body body body   body body body",
        QUERY_STRING: "location=japan&user=guest",
        PORT: 4242,
        CGI_FILE: "all_meta_vars.py",
        PATH_INFO: "",
    }
    run_test(
        conf2,
        req_data,
        (
            f"AUTH_TYPE=\n"
            f"CONTENT_LENGTH={len(req_data[BODY])}\n"
            f"CONTENT_TYPE={req_data[CONTENT_TYPE]}\n"
            f"GATEWAY_INTERFACE=CGI/1.1\n"
            f"PATH_INFO={req_data[PATH_INFO]}\n"
            f"PATH_TRANSLATED=\n"
            f"QUERY_STRING={req_data[QUERY_STRING]}\n"
            f"REMOTE_ADDR=127.0.0.1\n"
            f"REMOTE_HOST=127.0.0.1\n"
            # f"REQUEST_METHOD={req_data['method']}\n"
            f"REQUEST_METHOD=GET\n"
            f"SCRIPT_NAME=/{ROOT}/{req_data[CGI_FILE]}\n"
            f"SERVER_NAME={req_data[HOST]}\n"
            f"SERVER_PORT={req_data[PORT]}\n"
            f"SERVER_PROTOCOL=HTTP/1.1\n"
            f"SERVER_SOFTWARE=webserv/1.0\n"
        ),
    )


@pytest.mark.parametrize(
    "conf3",
    ["all_meta_vars.conf", "all_meta_vars_poll.conf", "all_meta_vars_select.conf"],
)
def test_all_meta_vars3(conf3):
    req_data = {
        HOST: "_",
        CONTENT_TYPE: "text/plain",
        BODY: "\n\n\n\n\n\n\n\n",
        QUERY_STRING: "key1=&key2=value2",  # key1の値は空
        PORT: 4242,
        CGI_FILE: "all_meta_vars.py",
        PATH_INFO: "/path/info",
    }
    run_test(
        conf3,
        req_data,
        (
            f"AUTH_TYPE=\n"
            f"CONTENT_LENGTH={len(req_data[BODY])}\n"
            f"CONTENT_TYPE={req_data[CONTENT_TYPE]}\n"
            f"GATEWAY_INTERFACE=CGI/1.1\n"
            f"PATH_INFO={req_data[PATH_INFO]}\n"
            f"PATH_TRANSLATED=\n"
            f"QUERY_STRING={req_data[QUERY_STRING]}\n"
            f"REMOTE_ADDR=127.0.0.1\n"
            f"REMOTE_HOST=127.0.0.1\n"
            # f"REQUEST_METHOD={req_data['method']}\n"
            f"REQUEST_METHOD=GET\n"
            f"SCRIPT_NAME=/{ROOT}/{req_data[CGI_FILE]}\n"
            f"SERVER_NAME={req_data[HOST]}\n"
            f"SERVER_PORT={req_data[PORT]}\n"
            f"SERVER_PROTOCOL=HTTP/1.1\n"
            f"SERVER_SOFTWARE=webserv/1.0\n"
        ),
    )


@pytest.mark.parametrize(
    "conf4",
    ["all_meta_vars.conf", "all_meta_vars_poll.conf", "all_meta_vars_select.conf"],
)
def test_all_meta_vars4(conf4):
    req_data = {
        HOST: "test::::test",
        CONTENT_TYPE: "text/plain",
        BODY: "",
        QUERY_STRING: "",  # key1の値は空
        PORT: 4242,
        CGI_FILE: "http_meta_vars.py",
        PATH_INFO: "",
        HEADERS: {
            "test": "test",
            "TEST-": "TEST-",
            "t-e-S_T": "t-e-S_T",
            "test-test": "test-test",
            "a-bc_DEF": "a-bc_DEF",
        },
    }
    run_test(
        conf4,
        req_data,
        (
            f"HTTP_HOST=test::::test\n"
            f"HTTP_TEST=test\n"
            f"HTTP_TEST_=TEST-\n"
            f"HTTP_T_E_S_T=t-e-S_T\n"
            f"HTTP_TEST_TEST=test-test\n"
            f"HTTP_A_BC_DEF=a-bc_DEF\n"
        ),
    )
