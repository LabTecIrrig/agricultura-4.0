<?php
	include "conexao.php";
		
	$sensor = $_POST["sensor"];
	$temperatura = $_POST["temperatura"];
	$umidade = $_POST["umidade"];
		

	if($sensor == null){		
		echo "Valor Nulo";
	}else{	
		$sql = mysql_query("INSERT INTO senseor (sensor, temperatura, umidade, dia) 
		VALUES ('$sensor', '$temperatura', '$umidade', NOW())");
		if($sql){
			echo "Cadastrou";
		}else{
			echo "N�o cadastrou";
		}	
	}
?>