#!/usr/bin/python3
import cgi
import os
import sys

# アップロードを許可する拡張子
ALLOWED_EXTENSIONS = ["html", "txt", "pdf", "jpg", "jpeg"]

# CGIヘッダーを出力
header = "Content-Type: text/html\n"
body = "<html><body>\n"

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
        extension = os.path.splitext(fn)[1][1:].strip().lower()

        # 許可する拡張子かどうかチェック
        if extension in ALLOWED_EXTENSIONS:
            try:
                save_dir = "test/integration_test/test_files/upload_test/uploads"
                if os.path.exists(save_dir):
                    # ファイルを保存
                    save_path = f"{save_dir}/{fn}"
                    with open(save_path, "wb") as f:
                        f.write(fileitem.file.read())
                    header += "Status: 201 Created\n"
                    body += f"<p>The file '{fn}' was uploaded successfully</p>\n"
                else:
                    body += f"<p>You have to create a directory({save_dir}) to upload files.</p>\n"

            except Exception as e:
                body += f"<p>Error uploading file: {e}</p>\n"

        else:
            body += f"<p>File '{fn}' has an invalid extension. Allowed extensions are: {', '.join(ALLOWED_EXTENSIONS)}</p>\n"

    else:
        body += "<p>You have to select file.</p>\n"
else:
    # アップロードフォームを表示
    body += """
    <form enctype="multipart/form-data" method="post">
    <input type="file" name="file">
    <input type="submit" value="upload">
    </form>

"""

# HTMLの終了
body += "</body></html>\n"

print(header)
print()
print(body)
