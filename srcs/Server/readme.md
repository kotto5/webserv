サーバークラス

サーバーソケットを持つ。
リクエストを受理（accept）し、client と接続開始する
client と、メッセージの受信　送信をする
受信したリクエストがcgi の呼び出しを求めていれば、cgiプログラムを起動し、そのプログラムともソケットを通して送信　受信する
cgi で無ければ、適切なHandlerクラスを呼び出し、レスポンスを生成する。
通信が終わり次第、fd をclose する。
