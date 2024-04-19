#! /usr/bin/python3

import os
import sys
import time
import subprocess
import requests
import pytest

ROOT = "test/integration_test/test_files/server_res_test" # test file dir
SERVER = "Server"
DATE = "Date"
CONTENT_LENGTH = "Content-Length"
CONNECTION = "Connection"
KEEP_ALIVE = "keep-alive"
CLOSE = "close"

def run_server(webserv, conf):
  try:
    server_proc = subprocess.Popen([webserv, conf])
    time.sleep(0.1) # server起動を待つ
    return server_proc
  except Exception as e:
    print(f"{e}", file=sys.stderr)
    sys.exit(1)

def send_reqest(req_data):
  headers = {
    'host': req_data['host'],
    'content-type': req_data['content_type']
  }
  req = f"http://localhost:{req_data['port']}/{ROOT}/{req_data['request']}"
  r = requests.get(req, headers=headers, data=f"{req_data['body']}", timeout=0.5, params=f"{req_data['query_string']}")
  return r

def expect_headers_exist(response):
  expect_headers = [SERVER, DATE, CONTENT_LENGTH, CONNECTION]
  print(response.headers)
  assert all(response.headers.get(header) is not None for header in expect_headers)

def expect_header(actual, expect):
  assert actual == expect

def run_test(conf, req_data):
  CWD = os.path.dirname(os.path.abspath(__file__))
  PATH_WEBSERV = f"{CWD}/../../webserv"
  expects = {
    SERVER: "webserv/1.0",
    CONTENT_LENGTH: len(req_data),
    CONNECTION: KEEP_ALIVE
  }

  WEBSERV = run_server(PATH_WEBSERV, f"{ROOT}/{conf}")
  try:
      res = send_reqest(req_data)
      expect_headers_exist(res)
      all(expect_header(res.headers.get(header), value) for header, value in expects.items())
  finally:
    WEBSERV.kill()

# テストの引数に渡される
@pytest.mark.parametrize("conf1", [
    "server_res_test.conf",
    "server_res_test_poll.conf",
    "server_res_test_select.conf"
])
# 関数名がtestで始まる関数がテスト実行時に呼ばれる
def test_header1(conf1):
  run_test(conf1, {
    "host": "test",
    "content_type": "text",
    "body": "this is body message",
    "query_string": "a=a&b=b&c=c",
    "port": 4242,
    "request": "static/index.html",
    "Connection": KEEP_ALIVE,
  })
