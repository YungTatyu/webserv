#include "conf.hpp"
#include "Lexer.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <dirent.h>

bool	config::init_config(const std::string& file_path)
{
	// directory かどうか
	if (opendir(file_path.c_str()))
	{
		std::cerr << file_path << " is a directory." << std::endl;
		return false;
	}

	// file が存在するかどうか
	if (access(file_path.c_str(), F_OK))
	{
		std::cerr << "There isn't " << file_path << "." << std::endl;
		return false;
	}

	// file の読み取り権限があるかどうか？ 
	if (access(file_path.c_str(), R_OK))
	{
		std::cerr << "We cannot read " << file_path << "." << std::endl;
		return false;
	}

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

