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

std::vector<std::string> FileUtils::getDirectoryContents(const std::string& directoryPath)
{
	std::vector<std::string> contents;
	DIR* dir = opendir(directoryPath.c_str());
	if (dir != NULL)
	{
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL)
		{
			std::string filename = entry->d_name;
			if (filename != "." && filename != "..")
			{
				if (FileUtils::isDirectory(directoryPath + "/" + filename))
					filename += "/";
				contents.push_back(filename);
			}
		}
		closedir(dir);
	}
	return contents;
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

