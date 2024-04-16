#! /usr/bin/python3

import os
import sys
import subprocess
import requests
import pytest

ROOT = "test/cgi/cgi_files/executor" # test file dir

def run_server(webserv, conf):
  try:
    server_proc = subprocess.Popen([webserv, conf])
    return server_proc
  except Exception as e:
    print(f"{e}", file=sys.stderr)
    sys.exit(1)

def test_response(expect_status, req_data):
  req = f"http://localhost:{req_data['port']}/{ROOT}/{req_data['cgi_file']}"
  r = requests.get(req)
  assert r.status_code == expect_status

def run_test(conf, req_data):
  CWD = os.path.dirname(os.path.abspath(__file__))
  PATH_WEBSERV = f"{CWD}/../../webserv"

  WEBSERV = run_server(PATH_WEBSERV, f"{ROOT}/{conf}")
  test_response(200, req_data)

  WEBSERV.kill()


def main():
  run_test("all_meta_vars_poll.conf", {
    "host": "test",
    "content_type": "text",
    "body": "this is body message",
    "port": 4242,
    "cgi_file": "all_meta_vars.py"
  })
  # r = requests.get('https://google.com')
  # print(r.status_code)
  # print(r.content)

if __name__ == "__main__":
    sys.exit(main())