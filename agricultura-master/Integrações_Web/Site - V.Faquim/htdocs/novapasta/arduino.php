<?php
    include "conexao.php";

    $inf = $_GET["inf"];

    $sql=mysqli_query("INSERT INTO arduino (inf) VALUES ('$inf')");

    if($inf==NULL){
        echo "Valor nulo";
    }else{
        $sql=mysqli_query("INSERT INTO arduino (inf) VALUES ('$inf')");
        if ($sql) {
            echo "Cadastro realizado";
        }else {
            echo "Erro no cadastro";
        }

    }
?>


