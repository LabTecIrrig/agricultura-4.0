<?php
	include "conexao.php";
		
	$inf = $_GET["inf"];

	if($inf == null){		
		echo "Valor Nulo";
	}else{	
		$sql = mysql_query("INSERT INTO arduino (inf) VALUES ('$inf')");
		if($sql){
			echo "Cadastrou";
		}else{
			echo "N�o cadastrou";
		}	
	}
?>