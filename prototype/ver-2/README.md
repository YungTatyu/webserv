# Echo Serverクラス OOP design ver.2
### 基本的な方針
大元のサーバークラスは、イベントを検知して、適切なハンドラーを呼び出すだけ。なるべくクラスの責任が分散するようにした。今はeventLoopでクライアントからの接続イベントしか検知しないので、次はselectかepollとか使う。
コンストラクタで依存性の注入を行う。

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
