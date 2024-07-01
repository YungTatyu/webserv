#! /usr/bin/python3

"""
===== test 方針 =====

deleteするファイルやディレクトリを作成
serverを起動
test/integration_test/delete_test/delete.pyを実行するrequestを送る
そのcgiがpath_infoで指定されたpathのファイルをdelete
ファイルの存在とstatus_codeをテスト

"""

import os
import requests
import shutil
import pytest
import stat
import time

from upload_test import send_request
from server_res_header_test import run_server, expect_headers_exist


# test用のファイルがあるパスを定義
ROOT_FROM_WEBSERV = (
    "test/integration_test/test_files/delete_test"  # test fileへの相対パス
)
DELETE_DIR = "files"  # uploadするためのスクリプトで指定されているパス
DELETE_PATH = f"{ROOT_FROM_WEBSERV}/{DELETE_DIR}"  # uploadするためのスクリプトで指定されているパス


def run_test(conf, req_data):
    CWD = os.path.dirname(os.path.abspath(__file__))
    PATH_WEBSERV = f"{CWD}/../../webserv"
    delete_path = f"{req_data['root']}/{DELETE_DIR}/{req_data['file_name']}"

    try:
        WEBSERV = run_server(PATH_WEBSERV, f"{req_data['root']}/{conf}")
        actual = send_request(req_data)
        # print(f"{actual.text}")

        # test 実行
        expect_headers_exist(actual)

        assert (
            actual.status_code == req_data["expect_status"]
        ), f"Invalid status code , expect {req_data['expect_status']}, but actual {actual.status_code}"

        if req_data["can_delete"]:
            assert not os.path.isfile(delete_path), f"File exist: {delete_path}"
        else:
            if req_data["expect_status"] != 404:
                assert os.path.isfile(delete_path) or os.path.isdir(
                    delete_path
                ), f"File or directory does't exist: {delete_path}"

    finally:
        WEBSERV.kill()


# テストに使用する設定ファイルのリスト
configs = [
    "delete_test.conf",
    "delete_test_poll.conf",
    "delete_test_select.conf",
]

# テストに使用するファイル名のリスト
file_names = [
    ("index.html", True, 200),
    ("no_permission.html", False, 403),
    ("nothing.html", False, 404),
    ("test_dir/", False, 500),
]


@pytest.mark.parametrize(
    "conf, file_name, can_delete, expect_status",
    [
        (conf, file_name, can_delete, expect_status)
        for conf in configs
        for file_name, can_delete, expect_status in file_names
    ],
)
def test_delete(conf, file_name, can_delete, expect_status, fixture_session):
    # delete用のファイル作成
    if file_name == "index.html":
        with open(f"{DELETE_PATH}/{file_name}", "w") as file:
            file.write("Hello, world!")

    run_test(
        conf,
        {
            "host": "test",
            "port": 4646,
            "request": "delete.py",
            "root": ROOT_FROM_WEBSERV,
            "method": "DELETE",
            "file_name": file_name,
            "path_info": f"/{file_name}",
            "can_delete": can_delete,
            "expect_status": expect_status,
        },
    )


@pytest.fixture(scope="session")
def fixture_session():
    current_permissions = os.stat(f"{DELETE_PATH}/no_permission.html").st_mode
    new_permissions = current_permissions & ~stat.S_IWUSR
    os.chmod(f"{DELETE_PATH}/no_permission.html", new_permissions)
    time.sleep(1)
