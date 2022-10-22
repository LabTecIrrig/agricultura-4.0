<?php
	include "conexao.php";
		
	$sensor = 1;
	$temp = $_GET["temp"];
	
	//$umi = $_GET["umi"];
	

		

	if($sensor == null){		
		echo "Valor Nulo";
	}else{	
		$sql = mysql_query("INSERT INTO senseor (sensor, tempumi, dia) VALUES ('$sensor', '$temp', NOW())");

		if($sql){
			echo "Cadastrou";
		}else{
			echo "Não cadastrou";
		}	
	}
?>