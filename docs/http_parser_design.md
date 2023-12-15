案1
```
class HttpRequestParser
{
	public:
		HttpRequestParser();
		~HttpRequestParser();

		readRequestline();
		std::map<std::string, std::string> readHeaderField();
		readMessageBody();
		
	private:
		HttpRequestParser(const HttpRequestParser& other);
		HttpRequestParser& HttpRequestParser(const HttpRequestParser& other);

		// staticクラスにしないのならば
		std::string method;
		std::string target;
		std::map<std::string, std::string> field_line;
		std::string body;
};
```
requestParserクラスのメソッドとして、requestLine及び、messageBodyのパーサーを実装する。
・readRequestLine():HTTPリクエストメッセージの一行目が構文に沿っているかを確認する。
method SP request-target SP HTTP-versionに沿っているか。
・readHeaderField():二行目から下記の構文に沿っているか確認しながら、フィールド名と値のペアを読み込んでいく。空行が来るまで。
field-line = filed-name ":" OWS field-value OWS
・readMessageBody();Content-Length, Transfer-Encodingヘッダーフィールドがあればメッセージボディがある。ので読み込む。Content-Lengthかコネクションがクローズするまで読む。特にフォーマットはない。

案2
requestParserクラスの他に、requestLineParserクラス、messageBodyParserクラスを実装して継承する。

案3
・オブジェクト指向的にいくんだったら、HttpRequestクラスがあってそれに対してparserメソッドがあるのがいいのかもしれない。
```
class HttpRequest
{
	public:
		parser();
	
	private:
		std::string method;
		std::string target;
		[...]
}
```
その場合だとこのクラスをインターフェースとしてオブジェクトをサーバーに渡せばよくなるからいいかも。

悩み中
・staticクラスにしてしまう？それともインスタンス化したい？
・HTTPリクエストメッセージをオブジェクトとして管理したかったらインスタンス化するのもいいかも。その場合はnewのオーバーヘッドはどれくらいだろう。
・HTTPのversionが1.1以外の時は弾いてしまう？
