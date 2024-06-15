#!/usr/bin/env python3

"""
このスクリプトは、webservをバックグラウンドで起動し、指定されたk6テストスクリプトを実行します。

ex:
    python stress_test.py test_script.js 50 10s

args:
    script (str): 実行するk6テストスクリプトのパス
    vus (int): 仮想ユーザーの数
    duration (int + "s"): テストの継続時間

WARNING:
    このスクリプトは、webservディレクトリがルートディレクトリの直下に存在することを前提としています。
"""

import os
import sys
import subprocess
import time

DIR_PATH = os.path.dirname(os.path.realpath(__file__))
ROOT = os.path.join(DIR_PATH, "../..")
WEBSERV_PATH = os.path.join(ROOT, "webserv")


def init(path):
    process = subprocess.Popen(["make", "-j", "-C", path])
    exit_status = process.wait()

    if exit_status != 0:
        sys.exit(exit_status)


def err(message):
    print(message, file=sys.stderr)


def run_server(webserv, conf):
    try:
        server_proc = subprocess.Popen([webserv, conf])
        time.sleep(0.1)  # server起動を待つ
        return server_proc
    except Exception as e:
        print(f"{e}", file=sys.stderr)
        sys.exit(1)


def run_test(script, virtual_client, duration):
    script_path = os.path.join(DIR_PATH, os.path.basename(script))
    subprocess.run(
        ["k6", "run", script_path, "--vus", virtual_client, "--duration", duration]
    )


def main():
    if len(sys.argv) < 4:
        err("Usage: {} <script> <vus> <duration>".format(sys.argv[0]))
        return 1

    init(ROOT)
    script = sys.argv[1]
    virtual_client = sys.argv[2]
    duration = sys.argv[3]

    WEBSERV = run_server(WEBSERV_PATH, f"{DIR_PATH}/file/test.conf")
    try:
        run_test(script, virtual_client, duration)
    finally:
        WEBSERV.kill()

    return 0


if __name__ == "__main__":
    sys.exit(main())
