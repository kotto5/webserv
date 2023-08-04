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
	ss << "<table><colgroup><col style=\"25px\"><col style=\"width:250px\"><col style=\"width:250px\">\
	<col style=\"width:50px\"></colgroup><tr><th></th><th>Name</th><th>Last Modified</th><th>Size</th></tr>";

	// テーブルの整形と生成
	std::vector<FileInfo>::const_iterator it = _fileInfo.begin();
	for (; it != _fileInfo.end(); it++)
	{
		std::string e;
		if (it->isDirectory)
			e += createElement("<img src=\"docs/folder.png\">" , "td", "align=\"center\"");
		else
			e += createElement("<img src=\"docs/file.png\">" , "td", "align=\"center\"");
		e += createElement(createElement("\t" + it->fileName, "a", "href=\"" + it->filepath + "\""), "td", "align=\"left\"");
		e += createElement(convertTimeToDate(it->lastModified), "td", "align=\"center\"");
		e += createElement(formatSize(it->fileSize), "td", "align=\"right\"");
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
	bool is_directory;

	// ディレクトリストリームを開く　
	DIR *dir = opendir(_path.c_str());
	if (!dir)
	{
		Logger::instance()->writeErrorLog(ErrorCode::AUTO_FILE_NOT_OPEN);
		return ;
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
			// ディレクトリの場合は末尾にスラッシュを追加
			ent->d_type == DT_DIR ? name += "/" : name;
			// ファイルパスを取得
			std::string full_path = _path + name;
			// ディレクトリの判定
			ent->d_type == DT_DIR ? is_directory = true : is_directory = false;
			struct stat st;
			stat(full_path.c_str(), &st);
			_fileInfo.push_back(FileInfo(name, full_path, (time_t)st.st_mtime, st.st_size, is_directory));
		}
	}
	// ディレクトリストリームを閉じる
	closedir(dir);
}

/**
 * @brief 任意のタグで囲まれた要素を生成する
 *
 * @param content　要素の内容
 * @param tag　タグ
 * @param attr 属性値（任意）
 * @return std::string
 */
std::string Autoindex::createElement(const std::string &content, const std::string &tag, const std::string &attr)
{
	return ("<" + tag + " " + attr + " >" + content + "</" + tag + ">");
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

/**
 * @brief ファイルサイズ（バイト）を適切な単位に変換する
 *
 * @param byte
 * @return std::string
 */
std::string Autoindex::formatSize(long long byte)
{
	std::stringstream ss;

	if (byte < 1024)
	{
		ss <<  byte << "B";
	}
	else if (byte < 1024 * 1024)
	{
		ss << byte / 1024 << "KB";
	}
	return ss.str();
}

// Not use
Autoindex::Autoindex()
{
}
