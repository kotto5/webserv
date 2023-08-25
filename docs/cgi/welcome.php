<?php
echo ("HTTP/1.1 200 OK\r\n");
echo ("Content-Type: text/html; charset=utf-8\r\n");
echo ("content-length: 500\r\n");
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
// echo $_SERVER["REQUEST_METHOD"] , "<br>";
// if ($_SERVER["REQUEST_METHOD"] == "POST")
if (true)
{
    // 標準入力からデータを読み込む
    $stdin = fopen('php://stdin', 'r');
    $input = '';
    while($line = fgets($stdin)) {
        $input .= $line;
    }
    fclose($stdin);

    echo $input , "<br>";
    // データを解析し、変数に分解する
    parse_str($input, $data);
    
    // 解析した結果を出力する
    var_dump($data);
    var_dump($_GET);

    // 名前とEメールを取得する
    $name = urldecode($data['name']);
    $email = urldecode($data['email']);
    
    // // 出力する
    echo "Welcome $name!<br>Your email is: $email";
}
?>
</body>
</html>