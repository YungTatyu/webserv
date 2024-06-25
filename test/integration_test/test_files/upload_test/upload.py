#!/usr/bin/env python3
import cgi
import os
import sys

# アップロードを許可する拡張子
ALLOWED_EXTENSIONS = ["html", "txt", "pdf", "jpg", "jpeg"]

# CGIヘッダーを出力
print("Content-Type: text/html")
print()

# HTMLの開始
print("<html><body>")

# フォームデータの解析
form = cgi.FieldStorage()

# ファイルがアップロードされたか確認
if "file" in form:
    fileitem = form["file"]

    # ファイルが選択されているか確認
    if fileitem.filename:
        # ファイル名を取得
        fn = os.path.basename(fileitem.filename)

        # ファイルの拡張子を取得
        ext = os.path.splitext(fn)[1][1:].strip().lower()

        # 許可する拡張子かどうかチェック
        if ext in ALLOWED_EXTENSIONS:
            try:
                # ファイルを保存
                save_path = "test/integration_test/test_files/upload_test/uploads/" + fn
                with open(save_path, "wb") as f:
                    f.write(fileitem.file.read())
                print(f"<p>The file '{fn}' was uploaded successfully</p>")

            except Exception as e:
                print(f"<p>Error uploading file: {e}</p>")

        else:
            print(
                f"<p>File '{fn}' has an invalid extension. Allowed extensions are: {', '.join(ALLOWED_EXTENSIONS)}</p>"
            )

    else:
        print("<p>You have to select file.</p>")
else:
    # アップロードフォームを表示
    print(
        """
    <form enctype="multipart/form-data" method="post">
    <input type="file" name="file">
    <input type="submit" value="upload">
    </form>
    """
    )

# HTMLの終了
print("</body></html>")
