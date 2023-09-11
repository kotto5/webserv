<?php
echo ("Content-Type: text/html; charset=utf-8\r\n");
echo ("\r\n");
?>
<!DOCTYPE html>
<html lang="ja">
<head>
<meta charset="utf-8">
<title>PHPテスト</title>
</head>
<body>
<?php
    echo 'here is index.php.  It is $_SERVER list! <br/>';
	echo 'SERVER_NAME          :', $_SERVER['SERVER_NAME'];
	echo "<br/>";
	echo 'SERVER_PORT          :', $_SERVER['SERVER_PORT'];
	echo "<br/>";
	echo 'SERVER_PROTOCOL          :', $_SERVER['SERVER_PROTOCOL'];
	echo "<br/>";
	echo 'SERVER_SOFTWARE          :', $_SERVER['SERVER_SOFTWARE'];
	echo "<br/>";
	echo 'GATEWAY_INTERFACE          :', $_SERVER['GATEWAY_INTERFACE'];
	echo "<br/>";
	echo 'PATH_TRANSLATED          :', $_SERVER['PATH_TRANSLATED'];
	echo "<br/>";
	echo 'SCRIPT_FILENAME          :', $_SERVER['SCRIPT_FILENAME'];
	echo "<br/>";
	echo 'SCRIPT_NAME          :', $_SERVER['SCRIPT_NAME'];
	echo "<br/>";
	echo 'PATH_INFO          :', $_SERVER['PATH_INFO'];
	echo "<br/>";
	echo 'QUERY_STRING          :', $_SERVER['QUERY_STRING'];
	echo "<br/>";
	echo 'REMOTE_ADDR          :', $_SERVER['REMOTE_ADDR'];
	echo "<br/>";
	echo 'REMOTE_HOST          :', $_SERVER['REMOTE_HOST'];
	echo "<br/>";
	echo 'REMOTE_PORT          :', $_SERVER['REMOTE_PORT'];
	echo "<br/>";
	echo 'REMOTE_USER          :', $_SERVER['REMOTE_USER'];
	echo "<br/>";
	echo 'REQUEST_METHOD          :', $_SERVER['REQUEST_METHOD'];
	echo "<br/>";
	echo 'HTTP_ACCEPT          :', $_SERVER['HTTP_ACCEPT'];
	echo "<br/>";
	echo 'HTTP_CACHE_CONTROL          :', $_SERVER['HTTP_CACHE_CONTROL'];
	echo "<br/>";
	echo 'HTTP_CONNECTION          :', $_SERVER['HTTP_CONNECTION'];
	echo "<br/>";
	echo 'HTTP_HOST          :', $_SERVER['HTTP_HOST'];
	echo "<br/>";
	echo 'HTTP_USER_AGENT          :', $_SERVER['HTTP_USER_AGENT'];
	echo "<br/>";
	echo 'AUTH_TYPE          :', $_SERVER['AUTH_TYPE'];
	echo "<br/>";
	?>
	</body>
</html>
';
