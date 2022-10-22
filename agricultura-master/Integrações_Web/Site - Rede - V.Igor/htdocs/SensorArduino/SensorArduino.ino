#include <EtherCard.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_AM2320.h"

Adafruit_AM2320 am2320 = Adafruit_AM2320();

static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
byte Ethernet::buffer[700];
static uint32_t timer;

float temp;
String testtemp;
String testumi,junta;
float umi;
char mensagem[50];
char mensagem2[50];
int it,iu;


const char website[] PROGMEM = "192.168.253.179";
static byte myip[] = { 192,168,253,180};
static byte gwip[] = { 192,168,253,200 };

static void my_callback (byte status, word off, word len)
{
   Ethernet::buffer[off+len] = 0;    //mensagem do retorno do servidor
   //Serial.print((const char*) Ethernet::buffer + off);  //mostra todo o retorno
   Serial.print((const char*) Ethernet::buffer + off + 157); //mostra somente ok
}

void setup (){
  delay(1000);
  Serial.begin(9600);
  am2320.begin(); 
  Serial.println("Obtendo IP:");
  
  
  if (ether.begin(sizeof Ethernet::buffer, mymac,53) == 0)
    Serial.println( "Falha ao acessar o controlador Ethernet");
  ether.staticSetup(myip, gwip);
  ether.printIp("IP: ", ether.myip);
  ether.printIp("Gatway: ", ether.gwip);
  ether.printIp("Sevidor DNS: ", ether.dnsip);
  
   if (!ether.dnsLookup(website))     //IP do servidor por DNS
    Serial.println("Falha no DNS");
  ether.parseIp(ether.hisip, "192.168.253.179"); //IP do servidor manual
  ether.printIp("Servidor: ", ether.hisip);
  Serial.println(" ");
  
}

void loop (){
  ether.packetLoop(ether.packetReceive());
  if (millis() > timer){
    timer = millis() + 4000;
    ether.persistTcpConnection(true);
    temp=am2320.readTemperature(); //Leitura dos sensores
    umi=am2320.readHumidity();
    
    //testtemp.concat(temp); //conversao das leituras para uma variavel String
    //testumi.concat(umi);
    junta.concat(temp);
    junta.concat("/");
    junta.concat(umi);
  
   it=junta.length();  //it e ut recebem o tamabnho das strings para criar um char do mesmo tamanho
   char t[it+1];    
   //char u[iu+1]; //cria o char do mesmo tamanho da string
   
   strcpy(t,junta.c_str());
   //strcpy(u,testumi.c_str()); //copia a leitura da String para a variavel tipo char

   
   sprintf(mensagem, "?temp=%s",t); //passa os valores para as variaveis de conexao com o banco
   //sprintf(mensagem2, "?umi=%s",u);

    ether.browseUrl(PSTR("/arduino.php"), mensagem, website, my_callback);
    
  // ether.browseUrl(PSTR("/arduino2.php"), mensagem2, website, my_callback);
    junta="";  // zera as variaveis de conversao
   // testumi="";       
  }
} 
