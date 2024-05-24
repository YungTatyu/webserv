#!/usr/bin/python3

import os
import html
import cgitb

cgitb.enable()

# CGI環境変数からPATH_INFOを取得
path_info = os.environ.get("PATH_INFO", "/")

# ディレクトリ一覧の表示
if path_info.endswith("/"):
    try:
        dir_list = os.listdir("." + path_info)
        dir_list = sorted(dir_list)
    except PermissionError:
        print("Content-Type: text/plain\r\n", end="")
        print("Status: 404\r\n\r\n", end="")
        print("Permission denied")
    else:
        print("Content-Type: text/html\r\n", end="")
        print("Status: 200 OK\r\n\r\n", end="")
        print("<ul>\r\n", end="")
        for filename in dir_list:
            full_path = os.path.join("." + path_info, filename)
            print(
                '<li><a href="{0}">{1}</a></li>\r\n'.format(
                    html.escape(filename), html.escape(filename)
                ),
                end="",
            )
        print("</ul>\r\n", end="")

# ファイルの内容の表示
else:
    try:
        with open("." + path_info, "r") as f:
            content = f.read()
    except PermissionError:
        print("Content-Type: text/plain")
        print()
        print("Permission denied")
    except FileNotFoundError:
        print("Content-Type: text/plain")
        print()
        print("File not found")
    else:
        print("Content-Type: text/plain")
        print()
        print(content)
