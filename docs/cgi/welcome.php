<?php
if ($_SERVER["REQUEST_METHOD"] == "POST")
{
    $name = $_POST['name'];
    $email = $_POST['email'];
    if (empty($name) || empty($email)) {
        echo "Name or email is empty";
    } else {
        echo "Welcome $name!<br>Your email is: $email";
    }
}
?>