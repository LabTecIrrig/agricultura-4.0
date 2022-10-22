#include <EtherCard.h>

static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
byte Ethernet::buffer[700];
static uint32_t timer;

char mensagem[50];

const char website[] PROGMEM = "192.168.11.2";
static byte myip[] = { 192,168,11,3 };
static byte gwip[] = { 192,168,11,1 };

static void my_callback (byte status, word off, word len)
{
   Ethernet::buffer[off+len] = 0;    //mensagem do retorno do servidor
   //Serial.print((const char*) Ethernet::buffer + off);  //mostra todo o retorno
   Serial.print((const char*) Ethernet::buffer + off + 157); //mostra somente ok
}

void setup (){
  delay(1000);
  Serial.begin(57600);
  Serial.println("Obtendo IP:");
  if (ether.begin(sizeof Ethernet::buffer, mymac,53) == 0)
    Serial.println( "Falha ao acessar o controlador Ethernet");
  ether.staticSetup(myip, gwip);
  ether.printIp("IP: ", ether.myip);
  ether.printIp("Gatway: ", ether.gwip);
   ether.printIp("Sevidor DNS: ", ether.dnsip);
   if (!ether.dnsLookup(website))     //IP do servidor por DNS
    Serial.println("Falha no DNS");
  ether.parseIp(ether.hisip, "192.168.11.2"); //IP do servidor manual
  ether.printIp("Servidor: ", ether.hisip);
  Serial.println(" ");
}

void loop (){
  ether.packetLoop(ether.packetReceive());
  if (millis() > timer){
    timer = millis() + 4000;
    ether.persistTcpConnection(true);
    sprintf(mensagem, "?inf=oi");  
    ether.browseUrl(PSTR("/arduino.php"), mensagem, website, my_callback);       
  }
} 
