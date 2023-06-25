#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>


//現在の設定ファイル
// # server 1
// server {
// 	listen 80;
// 	server_name webserve1;

// 	location / {
// 		alias ./docs;
// 		index index.html;
// 	}

// 	location /error_page/ {
//         alias ./docs/error_page/;
//         error_page 404 /error_page/404.html;
//     }
// }


//一つのサーバ設定を保存するクラス。一つのサーバに関するパラメタの集合。
//現在の設定ファイルにあるパラメタを保存する
//設定ファイルのパラメタは以下の通り
// listen
// server_name
// location
// index
// error_page
class ServerConfig
{
	

	