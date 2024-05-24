#!/usr/bin/python3

import os
import cgi

# CGIのContent-Typeヘッダー
print("Content-Type: text/html")
print("")

# PATH_INFO環境変数を取得
path_info = os.environ.get('PATH_INFO', '')

# HTMLの出力
print("<html><body>")
print("<h1>PATH_INFO Example</h1>")
print(f"<p>Path Info: {path_info}</p>")
print("</body></html>")
