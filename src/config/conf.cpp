#include "conf.hpp"
#include "Lexer.hpp"
#include <fstream>
#include <iostream>

bool	config::init_config(const std::string& file_path)
{
	// ifstreamはファイルがない場合は.ファイルを開く事に失敗する
	std::ifstream file(file_path.c_str());
	if (!file.is_open())
	{
		std::cerr << "can't open" << std::endl;
		return false;
	}

	// 問題なければ、ストリームを閉じる
	file.close();

	// 拡張子が正しいかどうか
	unsigned int dotPosition = file_path.find_last_of('.');

    // ドットが見つからない場合または、拡張子が存在しない場合
    if (dotPosition == std::string::npos || dotPosition == file_path.length() - 1)
	{
		std::cerr << "No extention." << std::endl;
		return false;
	}

    std::string actualExtension = file_path.substr(dotPosition);

    // 拡張子が期待されるものと一致するかどうかを確認
    if (actualExtension != ".conf")
	{
		std::cerr << "Unexpected extension." << std::endl;
		return false;
	}

	config::Lexer lexer(file_path);
	lexer.tokenize();
	std::cout << "Finish tokenize" << std::endl;

	return true;
}

