#!/usr/bin/env python3

import cgi
import cgitb

cgitb.enable()

headers = []
body = ""

headers.append("Content-Type: text/html")

form = cgi.FieldStorage()

# DELETEメソッドの処理
if os.environ['REQUEST_METHOD'] == 'DELETE':
    headers.append("Status: 405 Method Not Allowed")
    body = "<html><body><h1>Method Not Allowed</h1></body></html>"

# POSTメソッドの処理
elif os.environ['REQUEST_METHOD'] == 'POST':
    if "key" in form:
        key = form.getvalue("key")
        headers.append("Status: 200 OK")
        body = f"<html><body><h1>Received key: {key}</h1></body></html>"
    else:
        headers.append("Status: 400")
        body = "<html><body><h1>Missing key</h1></body></html>"


for header in headers:
    print(header)
print()
print(body)
