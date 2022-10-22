<?php
    $host='localhost';
    $user='root';
    $pass='';
    $banco='aula';
    $con=mysql_connect($host, $user, $pass) or die(mysql_error());
    mysql_select_db($banco, $con) or die(myql_error());
?>