<?php
if ($_SERVER["REQUEST_METHOD"] == "POST")
{
    // 標準入力からデータを読み込む
    $stdin = fopen('php://stdin', 'r');
    $input = '';
    while($line = fgets($stdin)) {
        $input .= $line;
    }
    fclose($stdin);
    
    // データを解析し、変数に分解する
    parse_str($input, $data);
    
    // 解析した結果を出力する
    var_dump($data);
    
    // 名前とEメールを取得する
    $name = urldecode($data['name']);
    $email = urldecode($data['email']);
    
    // 出力する
    echo "Welcome $name!<br>Your email is: $email";
}
?>