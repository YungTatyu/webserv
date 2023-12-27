class Webserver
{
	public:
		setUp(); // confファイルを読み込んでサーバーの設定と立ち上げ
		acceptRequest(); // リクエストを受け付ける
		sendResponse(); // クライアントへレスポンスを送信する
	
	private:
		HTTPRequestParser *requestParser; // リクエストをパースする
		SessionManager *sessionManager; // セッション管理
		HTTPResponseGenerator *responseGenerator; // レスポンス生成する
		CGIHandler *cgiHandler; // CGIに対応する
};

class Data
{
	uuid sessionID;
	std::string sirname;
	std::string lastname;
};

class SessionManager
{
	public:
		createSession();
		readSession();
		updateSession();
		deleteSession();

	private:
		std::map<std::string sessionId, Data data> sessions[];
};

enum Method
{
	"GET",
	"POST",
	"HEAD",
	"DELETE"
};

class HTTPMessage
{
	private:
		Method method;
		Version version;
		Target target;
		std::string body;
};


class HTTPRequestParser
{
	public: HTTPRequest	parse( std::string rawRequest );
	
	private:
		parseHeader();
		parseBody();
};

class CGIHandler
{
	public:
		generateCGIRequest( HTTPRequest );
		generateCGIResponse();
};

class HTTPResponseGenerator
{
	public:
		generateHeader();
		generateBody();
};

