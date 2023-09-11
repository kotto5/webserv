<?php
echo ("Status: 200 OK\r\n");
echo ("Content-Type: text/html; charset=utf-8\r\n");
echo ("content-length: 20000000\r\n");
echo ("\r\n");
$dataFile = 'board.txt';

// ファイルが存在しない場合は作成
if (!file_exists($dataFile)) {
    touch($dataFile);
}

// メッセージがPOSTされた場合
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
	// POSTされたメッセージを取得
	$input = file_get_contents("php://stdin");
	parse_str($input, $parsed);

	$message = ($parsed['message'] === '') ? ' ' : $parsed['message'];
	$user = ($parsed['user'] === '') ? '名無しさん' : $parsed['user'];
	$date = date('Y-m-d H:i:s');

	$newData = $user . "\t" . $date . "\t" . $message . PHP_EOL;

	// メッセージをファイルに保存
	file_put_contents($dataFile, $newData, FILE_APPEND);
}

// データを読み込んでHTMLに出力
$posts = file($dataFile, FILE_IGNORE_NEW_LINES);
$posts = array_reverse($posts);
?>

<!DOCTYPE html>
<html lang="ja">
<head>
	<meta charset="UTF-8">
	<title>掲示板</title>
</head>
<body>
	<h1>掲示板</h1>
	<form action="" method="post">
		メッセージ: <input type="text" name="message">
		ユーザー名: <input type="text" name="user">
	<input type="submit" value="投稿">
	</form>
	<h2>投稿一覧（<?php echo count($posts); ?>件）</h2>
	<ul>
	<?php if (count($posts)) : ?>
	<?php foreach ($posts as $post) : ?>
	<?php list($user, $date, $message) = explode("\t", $post); ?>
		<li><?php echo htmlspecialchars($user, ENT_QUOTES, 'UTF-8'); ?> (<?php echo $date; ?>) : <?php echo htmlspecialchars($message, ENT_QUOTES, 'UTF-8'); ?></li>
	<?php endforeach; ?>
	<?php else : ?>
		<li>まだ投稿はありません。</li>
	<?php endif; ?>
	</ul>
</body>
</html>
