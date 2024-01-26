# Echo Servelクラス OOP design ver.3
### 基本的な方針
設計はver.2と同じ。IO多重化を導入して複数クライアントとの接続を許容する。
複数クライアントとの接続を確認するためには、１リクエスト/レスポンスで接続が切れない必要があるのでそこも修正する。

### エコーサーバークラスが保有するコンポーネント
* NetworkIOHandlerクラス
	> クライアントとデータの送受信を行う
* RequestHandlerクラス
	> NetworkIOHandlerで受け取ったデータに応じた処理をする
* ConnectionManagerクラス
	> コネクションが疎通したソケットととその直前のデータの管理をする。NetworkIOHandlerクラスとRequestHandlerクラスのデータ受け渡しのインターフェースとなる。
* ServerConfigクラス
	> 設定ファイルをパースして管理する

### 使い方
```
c++ main.cpp EchoServer.cpp
```
実行すると、3001番ポートでクライアントからの接続を待つので、それにtelnetとかでアクセスしてください。
