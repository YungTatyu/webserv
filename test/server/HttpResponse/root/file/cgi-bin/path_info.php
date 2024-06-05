<?php
// path_info.php

// PATH_INFOが設定されているか確認します
if (isset($_SERVER['PATH_INFO'])) {
    // PATH_INFOを取得
    $pathInfo = $_SERVER['PATH_INFO'];

    // パス情報をスラッシュで分割
    $pathSegments = explode('/', trim($pathInfo, '/'));

    // 各セグメントを表示
    echo "<h1>PATH_INFO</h1>";
    echo "<p>Raw PATH_INFO: $pathInfo</p>";
    echo "<h2>Path Segments</h2>";
    echo "<ul>";
    foreach ($pathSegments as $segment) {
        echo "<li>$segment</li>";
    }
    echo "</ul>";
} else {
    echo "<p>No PATH_INFO available.</p>";
}
?>

