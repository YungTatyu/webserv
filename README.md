# Webserveeeee OOP design
### 基本的な方針
大元のサーバークラスは、イベントを検知して、適切なハンドラーを呼び出すだけ。なるべくクラスの責任が分散するようにした。今はeventLoopでクライアントからの接続イベントしか検知しないので、次はselectかepollとか使う。
コンストラクタで依存性の注入を行う。
HTTPモジュールを追加した。

### ウェブサーバークラスが保有するコンポーネント
* NetworkIOHandlerクラス
	> クライアントとデータの送受信を行う
* RequestHandlerクラス
	> NetworkIOHandlerで受け取ったデータに応じた処理をする
	> HttpMessageクラスを用いて、リクエストのパースとレスポンスの生成を行う。
* ConnectionManagerクラス
	> コネクションが疎通したソケットととその直前のデータの管理をする。NetworkIOHandlerクラスとRequestHandlerクラスのデータ受け渡しのインターフェースとなる。
* ServerConfigクラス
	> 設定ファイルをパースして管理する

### HttpMessageクラス
* HTTPリクエストとレスポンスのエンティティクラスを保持し、リクエストのパース、レスポンス生成を行う。

### 使い方
```
make
```
```
telnet localhost 3001
> GET path/to/resource HTTP/1.1
```
3001番ポートでクライアントからの接続を待つので、それにtelnetとかでアクセスしてください。
