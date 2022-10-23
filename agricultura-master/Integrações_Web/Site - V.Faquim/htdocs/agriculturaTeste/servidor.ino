#include <SPI.h>
#include <Ethernet_W5500.h>
String valor;
#define pino_sinal_analogico A0

int valor_analogico;


#if defined(WIZ550io_WITH_MACADDRESS) 
#else
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
#endif  

char server[] = "www.adrianobraga.com.br";    

IPAddress ip(192,168,0,177);


EthernetClient client;

void setup() {
  Serial.begin(9600);
  pinMode(pino_sinal_analogico, INPUT);


#if defined(WIZ550io_WITH_MACADDRESS)
  if (Ethernet.begin() == 0) {
#else
  if (Ethernet.begin(mac) == 0) {
#endif  
    Serial.println("Failed to configure Ethernet using DHCP");
#if defined(WIZ550io_WITH_MACADDRESS)
    Ethernet.begin(ip);
#else
    Ethernet.begin(mac, ip);
#endif  
  }
  delay(1000);
  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());
  
  Serial.println("connecting...");

  if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET /agricultura/agricultura/public/ HTTP/1.1");
    client.println("Host: www.adrianobraga.com.br");
    client.println("Connection: close");
    client.println();
  } 
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");,
  }
}

void loop()
{
  
  if (client.available()) {
    char c = client.read();

  }

    valor = "1000";
    if (!client.connected()) {
        Serial.println();
        Serial.println("Desconectando.");
        client.stop();
        delay(1000);
        client.connect(server, 80);
        Serial.println("Conectado!");
        valor_analogico = analogRead(pino_sinal_analogico);
        valor = valor_analogico;
        client.println("GET /agricultura/agricultura/public/?i=recebe&id=1&valor="+valor+" HTTP/1.1");
        client.println("Host: www.adrianobraga.com.br");
        client.println("Connection: close");
        client.println();

    }


}
