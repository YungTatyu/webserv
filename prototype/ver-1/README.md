# Echo Serverクラス OOP design ver.1
### 基本的な方針
サーバークラスにサーバーを操作するためのメソッドを持たせて、クラスを利用する側でメソッドを使用してもらう。echo_server.cをオブジェクト指向的に書き直した。

### Echo Serverクラスのメソッド
* setUp()
	> サーバーの立ち上げ
* acceptRequest()
	> リクエストを受け付ける
* sendResponse()
	> レスポンスを返す
* closeConnection()
	> クライアントとの接続を閉じる
* down()
	> サーバーのシャットダウン


### 使い方
```
c++ main.cpp EchoServer.cpp
```
実行すると、3001番ポートでクライアントからの接続を待つので、それにtelnetとかでアクセスしてください。
