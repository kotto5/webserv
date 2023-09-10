<?php
// ステータス行
echo "Content-Type: text/html\r\n";
echo "Status: 200 OK\r\n";
echo "\r\n";
?>

<!DOCTYPE html>
<html lang="ja">
<head>
<meta charset="utf-8">
<title>PHPテスト</title>
</head>
<body>
	<b>PHPテスト</b>
	<b>here is index.php.  It is $_SERVER list! <br/></b>
	<h2>Basic HTML Form</h2>
	<form action="/cgi-bin/welcome.php" method="post">
		Name: <input type="text" name="name"><br>
		Email: <input type="text" name="email"><br>
	<input type="submit">
	</form>
</body>
</html>