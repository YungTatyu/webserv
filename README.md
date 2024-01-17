# Webserveeeee OOP design
### 基本的な方針
大元のサーバークラスは、イベントを検知して、適切なハンドラーを呼び出すだけ。なるべくクラスの責任が分散するようにした。今はeventLoopでクライアントからの接続イベントしか検知しないので、次はselectかepollとか使う。
コンストラクタで依存性の注入を行う。
HTTPモジュールを追加した。
autoindexとエラーページを追加しました。

### ウェブサーバークラスが保有するコンポーネント
* NetworkIOHandlerクラス
	> クライアントとデータの送受信を行う
* RequestHandlerクラス
	> NetworkIOHandlerで受け取ったデータに応じた処理をする
	> HttpMessageクラスを用いて、リクエストのパースとレスポンスの生成を行う。
* ConnectionManagerクラス
	> コネクションが疎通したソケットととその直前のデータの管理をする。NetworkIOHandlerクラスとRequestHandlerクラスのデータ受け渡しのインターフェースとなる。
* EventManagerクラス
	> struct pollfdを管理する。
* ServerConfigクラス
	> 設定ファイルをパースして管理する

### HttpMessageクラス
* HTTPリクエストとレスポンスのエンティティクラスを保持し、リクエストのパース、レスポンス生成を行う。

### SysCallWrapper名前空間
* socket(), bind(), listen()などのシステムコールの実行とエラーをハンドルするラッパー関数Socket(), Bind(), Listen()などを定義。

### 使い方
```
make && ./webserv
```
```
telnet localhost 3001
> GET path/to/resource HTTP/1.1
```
3001番ポートでクライアントからの接続を待つので、それにtelnetとかでアクセスしてください。\
ブラウザ（Chromeでは確認しました。）からも動きます!!\
autoindexがあるのでリクエストURIがディレクトリでかつその配下にindex.htmlがあればindex.htmlを表示し、なければディレクトリ配下のファイル一覧が表示されます。
