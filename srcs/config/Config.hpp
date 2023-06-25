#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <map>
#include "ServerConfig.hpp"

// 設定ファイルを読み、その情報を内部データ構造に保存するクラス
class Config
{
	public:
		Config();
		~Config();
		void readFile(const std::string &filepath);
		void addServer(const ServerConfig &server_config);
		const std::map<int, std::vector<ServerConfig> > &server() const;
	private:
		std::map<int, std::vector<ServerConfig> > server_;
};

#endif