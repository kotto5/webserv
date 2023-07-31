#include "Autoindex.hpp"
#include <dirent.h>
#include "Logger.hpp"
#include  <sstream>
#include "FileInfo.hpp"
#include <sys/types.h>
#include <sys/stat.h>

// Constructors
Autoindex::Autoindex(const std::string& path): _path(path)
{
	parseDirectory();
}

Autoindex::Autoindex(const Autoindex &other)
{
	*this = other;
}

// Destructor
Autoindex::~Autoindex()
{
}

// Operators
Autoindex & Autoindex::operator=(const Autoindex &rhs)
{
	if (this != &rhs)
	{
		_fileInfo = rhs._fileInfo;
	}
	return *this;
}

/**
 * @brief autoindexのHTMLを生成する
 *
 * @param path
 * @return std::string
 */
std::string Autoindex::generateAutoindex()
{
	std::stringstream ss;

	// ヘッダー生成
	ss << "<!DOCTYPE html><html><head><title>Index of " << _path
		<< "</title></head><body><h1>Index of " << _path << "</h1><hr>";

	// ファイル情報をテーブルに追加
	ss << "<table><colgroup><col style=\"width:250px\"><col style=\"width:250px\">\
	<col style=\"width:150px\"></colgroup><tr><th>Name</th><th>Last Modified</th><th>Size</th></tr>";

	std::vector<FileInfo>::const_iterator it = _fileInfo.begin();
	for (; it != _fileInfo.end(); it++)
	{
		std::string e;
		e += createElement(createHyperlink(it->fileName, it->filepath), "td");
		e += createElement(convertTimeToDate(it->lastModified), "td");
		e += createElement(std::to_string(it->fileSize), "td");
		ss << createElement(e, "tr");
	}

	// フッター生成
	ss << "</table></body></html>";
	return ss.str();
}

/**
 * @brief ディレクトリを解析する
 *
 * @param path
 * @return const std::string&
 */
void	Autoindex::parseDirectory()
{
	// ディレクトリストリームを開く　
	DIR *dir = opendir(_path.c_str());
	if (!dir)
	{
		exit(0);
	}
	// ディレクトリエントリー
	struct dirent* ent;
	while ((ent = readdir(dir)) != NULL)
	{
		// ディレクトリ・ファイルの場合
		if (ent->d_type == DT_DIR || ent->d_type == DT_REG)
		{
			// ファイル名を取得
			std::string name = ent->d_name;
			//　ファイルパスを取得
			std::string full_path = _path + name;
			struct stat st;
			stat(full_path.c_str(), &st);
			_fileInfo.push_back(FileInfo(name, full_path, (time_t)st.st_mtime, st.st_size));
		}
	}
	// ディレクトリストリームを閉じる
	closedir(dir);
}

/**
 * @brief 任意のタグで囲まれた要素を生成する
 *
 * @param content
 * @param tag
 * @return std::string
 */
std::string Autoindex::createElement(const std::string &content, const std::string &tag)
{
	return ("<" + tag + ">" + content + "</" + tag + ">");
}

/**
 * @brief ハイパーリンクを生成する
 *
 * @param content
 * @param url
 * @return std::string
 */
std::string Autoindex::createHyperlink(const std::string &content, const std::string &url)
{
	return ("<a href=\"" + url + "\">" + content + "</a>");
}

/**
 * @brief time_t型を日付に変換する
 *
 * @param time
 * @return std::string
 */
std::string Autoindex::convertTimeToDate(time_t time)
{
	struct tm *tm;
	char buf[200];

	tm = localtime(&time);
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
	return std::string(buf);
}

// Not use
Autoindex::Autoindex()
{
}
