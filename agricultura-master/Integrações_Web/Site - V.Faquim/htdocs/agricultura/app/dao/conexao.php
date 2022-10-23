<?php

class Conexao{
  private $host = 'localhost'; //Conexao local para usar no predio de pós
  private $db = 'agricultura';
  private $usuario = 'root';
  private $senha = '';

  /*
  private $host = 'localhost';
  private $db = 'adrianob_agricultura40';
  private $usuario = 'adrianob_agricultura';
  private $senha = '@gricultur@10';  */


  public function conectar(){
    try{
      $conexao = new PDO(
        "mysql:host=$this->host;dbname=$this->db",
        "$this->usuario", "$this->senha"
      );
      return $conexao;
    } catch (PDOException $e){
      echo $e->getMessage();
    }
  }
}

?>
