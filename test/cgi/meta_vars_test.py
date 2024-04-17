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
  req = f"http://localhost:{req_data['port']}/{ROOT}/{req_data['cgi_file']}"
  r = requests.get(req)
  return r

def expect_status(response, expect):
  assert response.status_code == expect

def expect_body(response, req_data):
  assert response.status_code == 200

def run_test(conf, req_data):
  CWD = os.path.dirname(os.path.abspath(__file__))
  PATH_WEBSERV = f"{CWD}/../../webserv"

  WEBSERV = run_server(PATH_WEBSERV, f"{ROOT}/{conf}")
  res = send_reqest(req_data)
  try:
      expect_status(res, 200)
      expect_body(res, req_data)
  finally:
    WEBSERV.kill()

# テストの引数に渡される
@pytest.mark.parametrize("conf1", [
    # "all_meta_vars.conf",
    "all_meta_vars_poll.conf",
    "all_meta_vars_select.conf"
])

# 関数名がtestで始まる関数がテスト実行時に呼ばれる
def test_all_meta_vars1(conf1):
  run_test(conf1, {
    "host": "test",
    "content_type": "text",
    "body": "this is body message",
    "port": 4242,
    "cgi_file": "all_meta_vars.py"
  })
