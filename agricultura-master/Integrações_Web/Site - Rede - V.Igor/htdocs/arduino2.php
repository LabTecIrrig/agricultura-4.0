<?php
	include "conexao.php";
		
	$sensor = 1;
	//$temp = $_GET["temp"];
	$umi = $_GET["umi"];
	

		

	if($sensor == null){		
		echo "Valor Nulo";
	}else{	
		$sql = mysql_query("INSERT INTO umidade (umi) VALUES ('$umi')");
		//$sql = mysql_query("INSERT INTO temp (sensor, temperatura, umidade, dia) VALUES ('$temp', '$umi')");
		if($sql){
			echo "Cadastrou Umidade";
		}else{
			echo "N�o cadastrou";
		}	
	}
?>