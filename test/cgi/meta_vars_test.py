#! /usr/bin/python3

import os
import sys
import time
import subprocess
import requests
import pytest

ROOT = "test/cgi/cgi_files/executor" # test file dir

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
  req = f"http://localhost:{req_data['port']}/{ROOT}/{req_data['cgi_file']}?{req_data['query_string']}"
  r = requests.get(req, headers=headers, data=req_data['body'], timeout=0.5)
  return r

def expect_status(response, expect):
  assert response.status_code == expect

def expect_body(response, req_data):
  expect = (
    f"AUTH_TYPE=\n"
    f"CONTENT_LENGTH={len(req_data['body'])}\n"
    f"CONTENT_TYPE={req_data['content_type']}\n"
    f"GATEWAY_INTERFACE=CGI/1.1\n"
    f"PATH_INFO=\n"
    f"PATH_TRANSLATED=\n"
    f"QUERY_STRING={req_data['query_string']}\n"
    f"REMOTE_ADDR=127.0.0.1\n"
    f"REMOTE_HOST=127.0.0.1\n"
    # f"REQUEST_METHOD={req_data['method']}\n"
    f"REQUEST_METHOD=GET\n"
    f"SCRIPT_NAME=/{ROOT}/{req_data['cgi_file']}\n"
    f"SERVER_NAME={req_data['host']}\n"
    f"SERVER_PORT={req_data['port']}\n"
    f"SERVER_PROTOCOL=HTTP/1.1\n"
    f"SERVER_SOFTWARE=webserv/1.0\n"
  )
  # print(response.content)
  assert response.text == expect

def run_test(conf, req_data):
  CWD = os.path.dirname(os.path.abspath(__file__))
  PATH_WEBSERV = f"{CWD}/../../webserv"

  WEBSERV = run_server(PATH_WEBSERV, f"{ROOT}/{conf}")
  try:
      res = send_reqest(req_data)
      expect_status(res, 200)
      expect_body(res, req_data)
  finally:
    WEBSERV.kill()

# テストの引数に渡される
@pytest.mark.parametrize("conf1", [
    "all_meta_vars1.conf",
    "all_meta_vars1_poll.conf",
    "all_meta_vars1_select.conf"
])
# 関数名がtestで始まる関数がテスト実行時に呼ばれる
def test_all_meta_vars1(conf1):
  run_test(conf1, {
    "host": "test",
    "content_type": "text",
    "body": "this is body message",
    "query_string": "a=a&b=b&c=c",
    "port": 4242,
    "cgi_file": "all_meta_vars.py"
  })


@pytest.mark.parametrize("conf2", [
    "all_meta_vars2.conf",
    "all_meta_vars2_poll.conf",
    "all_meta_vars2_select.conf"
])
def test_all_meta_vars2(conf2):
  run_test(conf2, {
    "host": "tachu",
    "content_type": "text/html",
    "body": "body body body   body body body   body body body   body body body",
    "query_string": "location=japan&user=guest",
    "port": 4343,
    "cgi_file": "all_meta_vars.py"
  })
