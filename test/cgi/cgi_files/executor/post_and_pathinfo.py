#!/usr/bin/python3

import os
import sys
import html
import cgi
import cgitb
from urllib.parse import parse_qs
cgitb.enable()

# CGI環境変数からPATH_INFOを取得
path_info = os.environ.get('PATH_INFO', '/')

# リクエスト本文からデータを取得
content_length = int(os.environ.get('CONTENT_LENGTH', 0))
request_body = sys.stdin.read(content_length)

# リクエスト本文からフォームデータを解析
form_data = parse_qs(request_body)

print('Content-Type: text/html\r\n', end='')
print('Status: 200 OK\r\n\r\n', end='')
print('<!doctype html>')
print('<html>')
print('<body>')
# POSTデータがあれば処理をする
if form_data:
    name = form_data.get('name', [''])[0]
    if name:
        print('<h2>Hello, {}!</h2>'.format(html.escape(name)))

# formを表示
print('''
<h2>Enter your name</h2>
<form method="post" action="">
    <label for="name">Name:</label>
    <input type="text" id="name" name="name">
    <input type="submit" value="Submit">
</form>
''')

# ディレクトリ一覧の表示
if path_info.endswith('/'):
    try:
        dir_list = os.listdir('.' + path_info)
        dir_list = sorted(dir_list)
    except PermissionError:
        print('Permission denied')
    else:
        print('<ul>')
        for filename in dir_list:
            full_path = os.path.join('.' + path_info, filename)
            print('    <li><a href="{0}">{1}</a></li>'.format(html.escape(filename), html.escape(filename)))
        print('</ul>')
# ファイルの内容の表示
else:
    if not path_info:
        print('Path Info was not set')
    else:
        try:
            with open('.' + path_info, 'r') as f:
                content = f.read()
        except PermissionError:
            print('Permission denied')
        except FileNotFoundError:
            print('File not found')
        else:
            print('<pre>{}</pre>'.format(html.escape(content)))
print('</body>')
print('</html>')
