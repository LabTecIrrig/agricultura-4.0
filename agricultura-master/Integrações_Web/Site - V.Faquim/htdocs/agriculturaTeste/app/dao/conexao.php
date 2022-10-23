<?php
//--------------Seta os dados para a conexa com o banco de dados-------------------
  define('HOSTNAME', 'localhost');          
  define('USERNAME', 'root');
  define('PASSWORD', null);
  define('DATABASE', 'agricultura');
  define('CHARSET', 'utf8');
//-------------------Realiza a conexão conexão com o banco de dados----------------
  $mysqli=new mysqli (HOSTNAME, USERNAME, PASSWORD,DATABASE);

  if($mysqli -> connect_error){
    die("Erro: A conexão com o banco falhou" . $mysqli -> connect_error);

  }


  
  
  /*private $host = 'localhost'; //Conexao local para usar no predio de pós
  private $db = 'agricultura';
  private $usuario = 'root';
  private $senha = '';

  
  private $host = 'localhost';
  private $db = 'adrianob_agricultura40';
  private $usuario = 'adrianob_agricultura';
  private $senha = '@gricultur@10';  */





 /* public function conectar(){
    try{
      $conexao = new PDO(
        "mysql:host=$this->host;dbname=$this->db",
        "$this->usuario", "$this->senha"
      );
      return $conexao;
    } catch (PDOException $e){
      echo $e->getMessage();
    }
  }*/


?>
