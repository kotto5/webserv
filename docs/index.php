<?php
// ステータス行
echo "HTTP/1.1 200 OK\r\n";
// ヘッダー
echo "Content-Type: text/html\r\n";
echo "\r\n";
// ボディ
echo '<!DOCTYPE html>
<html>
	<body>
		<h2>Basic HTML Form</h2>
			<form action="welcome.php" method="post">
  				Name: <input type="text" name="name"><br>
  				Email: <input type="text" name="email"><br>
  			<input type="submit">
			</form>
	</body>
</html>
';
?>
