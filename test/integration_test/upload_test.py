#! /usr/bin/python3

"""
===== test 方針 =====

uploadするディレクトリを作成
serverを起動
test/integration_test/upload_test/upload.pyを実行するrequestを送る
そのcgiがrequest bodyに入れたファイルをupload
ファイルの存在と、その中身をテスト
1つのテストケースごとにuploadしたファイルを削除
すべてのテストが終わったらupload用のディレクトリを削除

"""

import os
import requests
import shutil
import pytest
import time

from server_res_header_test import run_server, expect_headers_exist

# test用のファイルがあるパスを定義
ROOT_FROM_WEBSERV = (
    "test/integration_test/test_files/upload_test"  # test fileへの相対パス
)
UPLOAD_DIR = "uploads"  # uploadするためのスクリプトで指定されているパス
UPLOAD_PATH = f"{ROOT_FROM_WEBSERV}/{UPLOAD_DIR}"  # uploadするためのスクリプトで指定されているパス


def assert_file_not_created(actual_path):
    assert not os.path.isfile(actual_path), f"File exist: {actual_path}"


def assert_file_content(actual_path, expect_path):
    assert os.path.isfile(actual_path), f"File does not exist: {actual_path}"

    with open(actual_path, "rb") as file:
        actual_content = file.read()
    with open(expect_path, "rb") as file:
        expect_content = file.read()

    assert (
        actual_content == expect_content
    ), f"File content does not match. Expected: {expect_content}, but got: {actual_file}"


def send_request(req_data):
    headers = {"host": req_data["host"]}

    req = (
        f"http://localhost:{req_data['port']}/{req_data['root']}/{req_data['request']}"
    )

    if req_data["method"] == "POST":
        files = {"file": open(f"{req_data['root']}/{req_data['file_name']}", "rb")}

    if req_data["path_info"]:
        req += f"{req_data['path_info']}"

    if req_data["method"] == "POST":
        r = requests.post(
            req,
            headers=headers,
            files=files,
            timeout=3,
        )
    elif req_data["method"] == "DELETE":
        r = requests.delete(
            req,
            headers=headers,
            timeout=3,
        )

    return r


def run_test(conf, req_data):
    CWD = os.path.dirname(os.path.abspath(__file__))
    PATH_WEBSERV = f"{CWD}/../../webserv"
    expect_path = f"{req_data['root']}/{req_data['file_name']}"
    actual_path = f"{req_data['root']}/{UPLOAD_DIR}/{req_data['file_name']}"
    # print(expect_path)
    # print(actual_path)

    try:
        WEBSERV = run_server(PATH_WEBSERV, f"{req_data['root']}/{conf}")
        actual = send_request(req_data)

        # test 実行
        expect_headers_exist(actual)

        if req_data["can_upload"]:
            assert (
                actual.status_code == 201
            ), f"Status code must be 201, but actual {actual.status_code}"
            assert_file_content(f"{actual_path}", f"{expect_path}")
        else:
            assert (
                actual.status_code == 200
            ), f"Status code must be 201, but actual {actual.status_code}"
            assert_file_not_created(f"{actual_path}")

    finally:
        WEBSERV.kill()


# テストに使用する設定ファイルのリスト
configs = [
    "upload_test.conf",
    "upload_test_poll.conf",
    "upload_test_select.conf",
]

# テストに使用するファイル名のリスト
file_names = [
    ("test.txt", True),
    ("index.html", True),
    ("reverse_shell.php", False),
]


@pytest.mark.parametrize(
    "conf, file_name, can_upload",
    [
        (conf, file_name, can_upload)
        for conf in configs
        for file_name, can_upload in file_names
    ],
)
def test_upload_file(conf, file_name, can_upload, fixture_session):
    run_test(
        conf,
        {
            "host": "test",
            "port": 4545,
            "request": "upload.py",
            "root": ROOT_FROM_WEBSERV,
            "method": "POST",
            "file_name": file_name,
            "path_info": "",
            "can_upload": can_upload,
        },
    )
    if os.path.exists(f"{UPLOAD_PATH}/{file_name}"):
        os.remove(f"{UPLOAD_PATH}/{file_name}")


@pytest.fixture(scope="session")
def fixture_session():
    os.makedirs(f"{UPLOAD_PATH}", exist_ok=True)
    time.sleep(1)
    yield
    shutil.rmtree(f"{UPLOAD_PATH}")
