#include "FileUtils.hpp"

bool FileUtils::isDirectory(const std::string& path)
{
	struct stat statbuf;
	if ( stat(path.c_str(), &statbuf) != 0 )
	{
		return false;
	}
	return S_ISDIR(statbuf.st_mode);
}

std::string FileUtils::readFile(const std::string& filePath)
{
	std::ifstream file(filePath.c_str());
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

std::string FileUtils::listDirectory(const std::string& directoryPath)
{
	std::stringstream buffer;
	buffer << "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>Directory listing for</title></head>";
	buffer << "<body><h1>Directory listing for " << directoryPath << "</h1>";
	buffer << "<hr>";
	buffer << "<ul>";

	DIR* dir = opendir(directoryPath.c_str());
	if (dir != NULL)
	{
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL)
		{
			std::string filename = entry->d_name;
			buffer << "<li><a href='" << directoryPath;
			if ( !directoryPath.empty() && directoryPath[directoryPath.size() - 1] != '/' )
				buffer << "/";
			buffer << filename << "'>" << filename << "</a>";
			if ( isDirectory(directoryPath + "/" + filename) )
				buffer << "/";
			buffer << "</li>";
		}
		closedir(dir);
	}

	buffer << "</ul>";
	buffer << "<hr>";
	buffer << "</body></html>";
	return buffer.str();
}

bool FileUtils::isExecutable(const char* filename)
{
	struct stat sbuf;
	if ( stat(filename, &sbuf) < 0 )
		return false;
	return S_ISREG(sbuf.st_mode) && (S_IXUSR & sbuf.st_mode);
}

bool FileUtils::isPHPExtension(const std::string& filename)
{
	std::string phpExt = ".php";
	if ( filename.length() < phpExt.length() )
		return false;
	return std::equal( phpExt.begin(), phpExt.end(), filename.end() - phpExt.length() );
}

