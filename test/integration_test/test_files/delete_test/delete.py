#!/usr/bin/python3

import os
import sys
import cgi

# 許可されたディレクトリ
ALLOWED_DIRECTORY = "test/integration_test/test_files/delete_test/files"

# CGIヘッダーを出力
headers = []
body = ""

# PATH_INFOからファイルパスを取得
path_info = os.environ.get("PATH_INFO", "")
file_path = os.path.join(ALLOWED_DIRECTORY, path_info.lstrip("/"))

# DELETEメソッドかどうか確認
if os.environ.get("REQUEST_METHOD") == "DELETE":
    if os.path.exists(file_path):
        if os.access(file_path, os.W_OK):
            try:
                os.remove(file_path)
                headers.append("Status: 200 OK")
                body = f"File '{file_path}' deleted successfully."
            except Exception as e:
                headers.append("Status: 500 Internal Server Error")
                body = f"Error deleting file '{file_path}': {e}"
        else:
            headers.append("Status: 403 Forbidden")
            body = f"Permission denied for deleting file '{file_path}'."
    else:
        headers.append("Status: 404 Not Found")
        body = f"File '{file_path}' not found."
else:
    headers.append("Status: 405 Method Not Allowed")
    body = "DELETE method required."

# CGIヘッダーの出力
print("Content-Type: text/plain")
for header in headers:
    print(header)
print()

# ボディの出力
print(body)
