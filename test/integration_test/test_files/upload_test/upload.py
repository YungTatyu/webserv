#!/usr/bin/env python3
import cgi
import os
import sys

# CGIヘッダーを出力
print("Content-Type: text/html")
print()

# HTMLの開始
print("<html><body>")

# フォームデータの解析
#form = cgi.FieldStorage()
form = cgi.FieldStorage()

# ファイルがアップロードされたか確認
if "file" in form:
    fileitem = form["file"]
    
    # ファイルが選択されているか確認
    if fileitem.filename:
        # ファイル名を取得
        fn = os.path.basename(fileitem.filename)
        
        try:
            # ファイルを保存
            with open("test/integration_test/test_files/upload_test/uploads/" + fn, "wb") as f:
                f.write(fileitem.file.read())
            print(f"<p>The file '{fn}' was uploaded</p>")

        except AssertionError as e:
            print(e)
        
    else:
        print("<p>You have to select file.</p>")
else:
    # アップロードフォームを表示
    print("""
    <form enctype="multipart/form-data" method="post">
    <input type="file" name="file">
    <input type="submit" value="upload">
    </form>
    """)

# HTMLの終了
print("</body></html>")
