<?php
  
  $host = "localhost";
  $user = "root";
  $pass = "";
  $banco = "irrigacao";
  $conn = mysql_connect ($host, $user, $pass) or die(mysql_error());
  mysql_select_db ($banco, $conn) or die(mysql_error());
  
  $query = sprintf("SELECT * FROM senseor"); 
  $dados = mysql_query($query, $conn) or die(mysql_error());  
  $linha = mysql_fetch_assoc($dados);
  $total = mysql_num_rows($dados);  
  
       
?>

<!DOCTYPE html>
<html lang="en">
<head>
<link rel="stylesheet" type="text/css" href="estilo.css" />
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>
    <div class="header">
        <img src="LogoIF.png" alt="" class="logoif">                    
    </div>
    <div class="titleHeader"><center><h1>Agricultura 4.0</h1><center>
        <h4>IF Goiano - Campus Ceres</h4> </center></div><br><br>
    <div><center><h1>Informações dos sensores</h1></center></div>
    <div class="content">
       <table border="1" class="tabela">
            <tr>
                 <td>ID</td>                
                <td>N° do sensor</td>
                <td>Temperatura<br>(C°)</td>                      
                <td>Umidade<br>(%)</td>
                <td>Data e horário<br>(AAAA/MM/DD)</td>                                
            </tr>
                
            <?php
    	if($total > 0) {
		do {
?>
            <tr>
                 <td><?php echo $linha['id'] ?></td>
                 <td><?php echo $linha['sensor'] ?></td>
                 <td><?php echo $linha['temperatura'] ?></td> 
                 <td><?php echo $linha['umidade'] ?></td>             
                 <td><?php echo $linha['dia'] ?></td>                 
            </tr>
            <?php
		}while($linha = mysql_fetch_assoc($dados));
	}
?>
        </table>     
    </div><br><br>

</body>
</html>