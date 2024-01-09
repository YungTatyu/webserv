class Webserver
{
	public:
		Webserver(); // 依存性注入
		eventLoop(); // イベントを受け付けて適切なハンドラーを呼び出す
	
	private:
		NetworkIOHandler *ioHandler; // ソケットの入出力を管理する
		RequestHandler *requestHandler; // リクエストを処理する
		ConnectionManager *connManager // コネクション管理（クライアントと疎通したソケットとそのコンテクスト（直前のリクエストメッセージ))
		SessionManager *sessionManager; // セッション管理
		CookieManager *cookieManager; // クッキー管理
		CGIHandler *cgiHandler; // CGIに対応する
		ServerConfig *serverConfig; // Confファイルを読み込んで設定
		Logger *logger; // ログ管理
};
