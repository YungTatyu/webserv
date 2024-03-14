#include "CGIHandler.hpp"
#include "FileUtils.hpp"
#include "SysCallWrapper.hpp"
#include <cstdlib>

/**
 * @brief cgi実行fileか
 * cgi実行ファイルの拡張子
 * 1. *.php
 * 2. *.cig
 * 3. *.py
 * 
 * @param cgi_path 
 * @return true 
 * @return false 
 */
bool cgi::CGIHandler::isCgi(const std::string& cgi_path)
{
	return (
		FileUtils::isExtensionFile(cgi_path, ".php")
		|| FileUtils::isExtensionFile(cgi_path, ".cgi")
		|| FileUtils::isExtensionFile(cgi_path, ".py")
	) && FileUtils::isExecutable(cgi_path.c_str());
}

