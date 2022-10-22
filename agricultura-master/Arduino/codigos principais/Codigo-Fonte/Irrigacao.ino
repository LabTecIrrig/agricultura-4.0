//09/08/2021
// --- Bibliotecas Auxiliares ---

//Carrega as bibliotecas para o SIM900 (Envio de SMS)
#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
SMSGSM sms;

//Carrega as bibliotecas para o display LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Carrega a biblioteca do cartão de memória
#include <SPI.h>
#include <SD.h>

//Carrega biblioteca do DS3231
#include <DS3231.h>                              
DS3231 rtc;                                             // Criação do objeto do tipo DS3231
RTCDateTime dataehora;                                  // Criação do objeto do tipo RTCDateTime

//Carrega a biblioteca do Sensor de Temperatura
#include <DHT.h>                                        // Carrega biblioteca do sensor DHT
#define DHTPIN 49                                       // Pino DATA do Sensor ligado na porta Digital 8
#define DHTTYPE DHT22                                   // Define o tipo de sensor DHT utilizado
DHT dht(DHTPIN, DHTTYPE);                               // Cria um objeto da classe dht

//Carrega a biblioteca do Rele Serial
#include <SerialRelay.h>                                // Carrega biblioteca do Módulo Rele Serial
// --- Rele das Válvulas Solenoides e da Bomba---
// --- Módulo Rele Serial
const byte NumModules = 2;
SerialRelay relays(4,5,NumModules);                     // (data, clock, number of modules)

//=================================================================================================================
// --- Mapeamento de Hardware ---
#define butUp    42                                     // Botão para selecionar tela acima no digital 43
#define butDown  43                                     // Botão para selecionar tela abaixo no digital 42
//#define butP     40                                     // Botão de ajuste mais no digital 40
//#define butM     41                                     // Botão de ajuste menos no digital 41

//=================================================================================================================
// --- Range de Umidade do Solo ---
#define    L0        80
#define    L1        221
#define    L2        221
#define    L3        221
#define    L4        221

//--- Número de menus ---
#define quantidade_menus 8                              // Quantidade de menus, se acrescentar mais algum é necessário alterar o valor 

//=================================================================================================================

// --- Protótipo das Funções Auxiliares ---
void changeMenu();                                      // Função para modificar o menu atual
void dispMenu();                                        // Função para mostrar o menu atual
void data_hora();                                       // Função do menu1, data e hora
void temperatura();                                     // Função do menu2, temperatura
void leitura_sensores();                                // Função que lê os dados dos sensores de umidade
void compara_sensores();                                // Faz a comparação dos valores obtidos pelos sensores e dispara o comando para ligar os reles
void imprime_media();                                   // Função do menu3, exibe os dados dos sensores de umidade no display
void liga_sim900();                                     // Função que liga o SIM900
void envia_sms();                                       // Função que envia SMS
void percentual_umidade();                              // Função do menu4, verifica o cartão de memória   
void liga_bomba();                                      // Função que verifica as válvulas solenóides e liga/desliga a bomba 
void configurar_cartao_sd();                            // Função que verifica o cartão de memória
void configurar_arquivo_log();                          // Função que cria o arquivo de Log que será armazenado no cartão de memória                  
void cartao_de_memoria();                               // Função de gravação dos arquivos no cartão de memória
void debug();                                           // Função utilizada para testar partes do código
void sensores_t1();                                     // Função utilizada para exibir os valores dos sensores do tratamento 1 no display
void sensores_t2();                                     // Função utilizada para exibir os valores dos sensores do tratamento 2 no display
void sensores_t3();                                     // Função utilizada para exibir os valores dos sensores do tratamento 3 no display
void sensores_t4();                                     // Função utilizada para exibir os valores dos sensores do tratamento 4 no display

// --- Variáveis Globais ---
// --- Menu ---
char menu = 1;                                          // Variável para selecionar o menu
char set1 = 0x00, set2 = 0x00;                          // Controle das lâmpadas
boolean t_butUp, t_butDown, t_butP, t_butM;             // Flags para armazenar o estado dos botões

// --- Sensor de temperatura e umidade
float umi;                                              // Le o valor da umidade do ar
float temp;                                             // Le o valor da temperatura

// --- Sensores de umidade ---
const int power1 = 2;
const int power2 = 3;
int sensor_value[9], i;
float VWC[9];
float voltagem[9];

int soma_tratamento1;                                   // Variável que recebe a soma dos valores dos sensores do tratamento 1 (sensor1, sensor2, sensor3, sensor4)
int soma_tratamento2;                                   // Variável que recebe a soma dos valores dos sensores do tratamento 2 (sensor5, sensor6, sensor7, sensor8)
int soma_tratamento3;                                   // Variável que recebe a soma dos valores dos sensores do tratamento 3 (sensor9, sensor10, sensor11, sensor12)
int soma_tratamento4;                                   // Variável que recebe a soma dos valores dos sensores do tratamento 4 (sensor13, sensor14, sensor15, sensor16)
int media_tratamento1;                                  // Variável da média do tratamento 1
int media_tratamento2;                                  // Variável da média do tratamento 2
int media_tratamento3;                                  // Variável da média do tratamento 3
int media_tratamento4;                                  // Variável da média do tratamento 4

String status;                                          // Variável do STATUS (Irrigando ou não)
String nivel_umidade_t1;                                // Variável do nível de umidade do Tratamento 1
String nivel_umidade_t2;                                // Variável do nível de umidade do Tratamento 2
String nivel_umidade_t3;                                // Variável do nível de umidade do Tratamento 3
String nivel_umidade_t4;                                // Variável do nível de umidade do Tratamento 4

// --- SIM900 ---
int numdata;
boolean started=false;
char smsbuffer[160];
char n[20];
byte emailSend = 0;

// --- Cartão SD ---
const int chipSelect = 53;                              // Constante que indica em qual pino está conectado o Chip Select do módulo de comunicação
File dataFile;                                          // Objeto responsável por escrever/Ler do cartão SD
bool cartaoOk = true;                                   // Variável para verificação de cartão

// --- Status das válvulas solenóides ---
String status_solenoide_t1;
String status_solenoide_t2;
String status_solenoide_t3;
String status_solenoide_t4;

// --- Rele da Bomba ---
const int porta_bomba = 41;

// --- Variáveis para temporizar o leitor de umidade do solo e outros ---                                         
unsigned long tempo_anterior_leitura = 0;                 // Variável utilizada para guardar o tempo anterior (leitura)
unsigned long tempo_anterior_gravacao = 0;                // Variável utilizada para guardar o tempo anterior (gravação)
unsigned long tempo_anterior_sensor_temp = 0;             // Variável utilizada para guardar o tempo anterior (sensor de temperatura)
unsigned long timer1_anterior_irrigacao = millis();       // Variável utilizada para guardar o tempo anterior da válvula 1
unsigned long timer2_anterior_irrigacao = millis();       // Variável utilizada para guardar o tempo anterior da válvula 2
unsigned long timer3_anterior_irrigacao = millis();       // Variável utilizada para guardar o tempo anterior da válvula 3
unsigned long timer4_anterior_irrigacao = millis();       // Variável utilizada para guardar o tempo anterior da válvula 4
unsigned long tempo_anterior_comparacao = millis();       // Variável utilizada para guardar o tempo anterior (comparação)

unsigned long intervalo_leitura = 10000;                  // Intervalo de tempo em MS para cada leitura dos sensores de umidade do solo
unsigned long intervalo_gravacao = 60000;                 // Intervalo de tempo em MS para gravação dos dados lidos pelo sensor de umidade do solo
//unsigned long intervalo_comparacao = 3600000;           // Intervalo de tempo em MS para realizar a comparação entre os valores dos sensores e ativar ou não o sistema de irrigação
unsigned long intervalo_sensor_temp = 2000;               // Intervalo de tempo em MS para cada leitura do sensor de temperatura
unsigned long timer1_irrigacao = 60000;                   // Intervalo de tempo em MS que a bomba permanece ligada - Tratamento 1
unsigned long timer2_irrigacao = 60000;                   // Intervalo de tempo em MS que a bomba permanece ligada - Tratamento 2
unsigned long timer3_irrigacao = 60000;                   // Intervalo de tempo em MS que a bomba permanece ligada - Tratamento 3
unsigned long timer4_irrigacao = 60000;                   // Intervalo de tempo em MS que a bomba permanece ligada - Tratamento 4
unsigned long intervalo_comparacao = 1800000;               // Intervalo de tempo em MS para realizar a comparação entre os valores dos sensores e ativar ou não o sistema de irrigação

// --- Criação dos caracteres customizados ---
//Simbolo Grau (°)
byte grau[8] =
{  
    B00001100, 
    B00010010, 
    B00010010, 
    B00001100, 
    B00000000, 
    B00000000, 
    B00000000, 
    B00000000,
}; 

//Ícone do Termômetro
byte termo[8] = 
{
    B00100,
    B01010,
    B01010,
    B01110,
    B01110,
    B11111,
    B11111,
    B01110
};

//Ícone da umidade (gota d'água)
byte umid[8] = 
{
    B00100,
    B00100,
    B01010,
    B01010,
    B10001,
    B10001,
    B10001,
    B01110,
};
//--------------------------------------- //

// --- Hardware do LCD ---

// Inicializa o display no endereco 0x27
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);

//=================================================================================================================
void setup()
{
  Serial.begin(9600);                                   // Inicializa a Serial
  lcd.begin(20,4);                                      // Inicializa LCD 20 x 4
  
  lcd.createChar(0, grau);                              // Cria o caracter customizado com o simbolo do grau °
  lcd.createChar(1,termo);                              // Cria o ícone do termômetro
  lcd.createChar(2,umid);                               // Cria o ícone da umidade
  
  configurar_cartao_sd();                               // Executa a rotina de verificação do cartão sd
  //liga_sim900();                                        // Inicializa o módulo SIM900 (Envio de SMS) 
     
// --- Configurações iniciais do RTC DS3231 (DATA E HORA) ---
//rtc.begin();                                            // Inicialização do RTC DS3231
//                                                      // Informacoes iniciais de data e hora
//                                                      // Apos setar as informacoes, comente a linha abaixo
rtc.setDateTime(__DATE__, __TIME__);                  // Configurando valores iniciais do RTC DS3231
    
// --- Inicializa o sensor de temperatura e umidade dht22 ---
dht.begin();                                            // Inicializa o DHT22

// --- Variáveis dos botões ---
  t_butUp   = false;                                    // limpa flag do botão Up
  t_butDown = false;                                    // limpa flag do botão Down
  t_butP    = false;                                    // limpa flag do botão P
  t_butM    = false;                                    // limpa flag do botão M


// --- Configuração de Pinos para saída ---
// --- Alimentação 5v dos sensores ---
pinMode(power1, OUTPUT); 
pinMode(power2, OUTPUT); 

digitalWrite(power1, HIGH);                             // Liga a tensão de (5v) para alimentar os sensores

// --- Rele da Bomba ---
pinMode(porta_bomba, OUTPUT);
digitalWrite(porta_bomba, HIGH); 

//--- Reles das válvulas solenóides ---
relays.SetRelay(1, SERIAL_RELAY_OFF, 1); 
relays.SetRelay(2, SERIAL_RELAY_OFF, 1); 
relays.SetRelay(3, SERIAL_RELAY_OFF, 1); 
relays.SetRelay(4, SERIAL_RELAY_OFF, 1); 

} //end setup

//=================================================================================================================
// --- Verificação Inicial do Cartão de Memória ---
void configurar_cartao_sd()
{
  if (SD.begin(chipSelect))                               // Verifica comunicação com o cartão de memória
  {
    lcd.setCursor(0, 0);                                  // Posiciona o cursor para exibir a msg de erro
    lcd.print("Cartao SD OK");                            // Informa que conseguiu conectar com o SD
    delay(3000);                                          // Tempo para ver msg no LCD
    configurar_arquivo_log();                             // Executa a função Arquivo LOG
    lcd.clear();
  } else {
    Serial.println("Erro na leitura do arquivo não existe um cartão SD ou o módulo está conectado corretamente ?");    
    cartaoOk = false;
    lcd.setCursor(0, 0);                                  // Posiciona o cursor para exibir a msg de erro
    lcd.print("Erro na leitura");                         // Informa que nao conseguiu conectar com o SD
    lcd.setCursor(0, 1);                                  // Muda para linha 1
    lcd.print("Falta cartao SD");                         // Informa que nao conseguiu conectar com o SD
    delay(3000);
    lcd.clear();
  }
} // end configurar_cartao_sd()

//=================================================================================================================
// --- Criação do cabeçalho no arquivo ---
void configurar_arquivo_log()
{
    if (!SD.exists("datalog.csv")){
      dataFile = SD.open("datalog.csv", FILE_WRITE);
      //logFile.println(", , ,");                         // Just a leading blank line, incase there was previous data
      dataFile.println("SENSOR 1; SENSOR 2; SENSOR 3; SENSOR 4; SENSOR 5; SENSOR 6; SENSOR 7; SENSOR 8; SOMA T; MEDIA T; TEMPERATURA (C); UMIDADE DO AR (%); DIA; MES; ANO; HORAS; MINUTOS; SEGUNDOS; STATUS");
      dataFile.close();
    }
} // end configurar_arquivo_log()

//=================================================================================================================

// --- Loop Infinito ---
void loop()
{
      
   changeMenu();
   dispMenu();
   leitura_sensores();
   compara_sensores();
   liga_bomba();
   cartao_de_memoria();
   //envia_sms();
   debug(); 

} //end loop

//=================================================================================================================
// --- Desenvolvimento das Funções Auxiliares ---
void changeMenu()                                         // Modifica o menu atual
{
  if (!digitalRead(butUp))   t_butUp   = true;            // Botão Up pressionado? Seta flag
  if (!digitalRead(butDown)) t_butDown = true;            // Botão Down pressionado? Seta flag
  if (digitalRead(butUp) && t_butUp)                      // Botão Up solto e flag setada?
  {                                                       // Sim...
    t_butUp = false;                                      // Limpa flag
    lcd.clear();                                          // Limpa display
    menu++;                                               // Incrementa menu
    if (menu > quantidade_menus) menu = 1;                // Se menu maior que 6, volta a ser 1
  }                                                       // end butUp
  if (digitalRead(butDown) && t_butDown)                  // Botão Down solto e flag setada?
  {                                                       // Sim...
    t_butDown = false;                                    // Limpa flag
    lcd.clear();                                          // Limpa display
    menu--;                                               // Decrementa menu
    if (menu < 1) menu = quantidade_menus;                // Se menu menor que 1, volta a ser 6
  }                                                       // end butDown

} //end changeMenu
//=================================================================================================================
void dispMenu()                                           // Mostra o menu atual
{
    switch(menu)                                          // Controle da variável menu
    {
       case 1:                                            // Caso 1
             data_hora();                                 // Chama a função de relógio
             
             break;                                       // break
       case 2:                                            // Caso 2
             temperatura();                               // Chama a função do termômetro
             
             break;                                       // break
       case 3:                                            // Caso 3
             imprime_media();                             // Chama função dos que imprime a media dos sensores
            
             break;                                       // break
       case 4:                                            // Caso 4
             percentual_umidade();                        // Chama função que verifica o cartao de memória
             
             break;                                       // break

       case 5:                                            // Caso 5
             sensores_t1();                               // Chama a função que exibe valores do sensores do tratamento 1
             
             break;                                       // break
       case 6:                                            // Caso 6
             sensores_t2();                               // Chama a função que exibe valores do sensores do tratamento 2
             
             break;                                       // break
       case 7:                                            // Caso 7
             sensores_t3();                               // Chama a função que exibe valores do sensores do tratamento 3
            
             break;                                       // break
       case 8:                                            // Caso 8
             sensores_t4();                               // Chama a função que exibe valores do sensores do tratamento 4
             
             break;                                       // break
    
    } //end switch menu

} //end dispMenu
//=================================================================================================================
void data_hora()                                        // Data e Hora (menu1)
{
lcd.setCursor(0,0);
lcd.print("***** IFGOIANO *****");
lcd.setCursor(0,4);
lcd.print("*** CAMPUS CERES ***");

dataehora = rtc.getDateTime();                          // Le as informacoes do CI
  lcd.setCursor(4, 1);                                  // Imprime as informacoes no LCD
  lcd.print("Data e Hora");
  lcd.setCursor(2, 2);
  lcd.print(dataehora.day);                          
  //imprime_dia_da_semana(myRTC.dayofweek);             // Chama a rotina que imprime o dia da semana
  lcd.print("/");
  lcd.print(dataehora.month);
  lcd.print("/");
  lcd.print(dataehora.year - 2000);
  lcd.print(" ");
  if (dataehora.hour < 10)                              // Adiciona um 0 caso o valor da hora seja <10
  {
    lcd.print("0");
  }
  lcd.print(dataehora.hour);
  lcd.print(":");
  if (dataehora.minute < 10)                            // Adiciona um 0 caso o valor dos minutos seja <10
  {
    lcd.print("0");
  }
  lcd.print(dataehora.minute);
  lcd.print(":");
  if (dataehora.second < 10)                            // Adiciona um 0 caso o valor dos segundos seja <10
  {
    lcd.print("0");
  }
  lcd.print(dataehora.second);
} // end data_hora()

//=================================================================================================================
void temperatura()                                                         // Temperatura (menu2)
{
 unsigned long tempo_atual = millis();                                     // Realizamos a leitura atual do tempo em que o nosso Arduino está ligado
  if (tempo_atual - tempo_anterior_sensor_temp > intervalo_sensor_temp)    // Pequena lógica para realizar leituras temporizadas sem parar o microcontrolador
   {
    tempo_anterior_sensor_temp = tempo_atual;                              // Guardamos o tempo anterior como o ultimo intervalo de tempo lido   
      float umi = dht.readHumidity();                                      // Le o valor da umidade
      float temp = dht.readTemperature();                                  // Le o valor da temperatura      
// testa se retorno é valido, caso contrário algo está errado.
  if (isnan(temp) || isnan(umi)) 
   {
    lcd.setCursor(0, 0);
    lcd.print("Erro na Leitura");
   }
    else
    {
//--- Exibe as informações no LCD ---
    lcd.setCursor(0,0);
    lcd.print("* DADOS CLIMATICOS *");
    lcd.setCursor(0,4);
    lcd.print("*** CAMPUS CERES ***");

    lcd.setCursor(0, 1);
    lcd.print("IN: ");
    lcd.setCursor(4, 1);
    lcd.write((byte)1);
    lcd.print(" ");
    lcd.setCursor(6, 1);
    lcd.print(temp, 1);
    lcd.setCursor(10, 1);
    lcd.write((byte)0);                                                   // Mostra o simbolo do grau formado pelo array
    lcd.setCursor(13, 1);                                                 // Mostra as informações lidas pelo sensor
    lcd.write((byte)2);
    lcd.print(" ");
    lcd.setCursor(15, 1);
    lcd.print(umi, 1);
    lcd.setCursor(19, 1);
    lcd.print("%");
     }
   }
} //end temperatura()
//=================================================================================================================
void leitura_sensores()                                         // Lê os dados dos sensores de umidade do solo
{
  digitalWrite(power1, HIGH);                                   // Liga a tensão de (5v) para alimentar os sensores

// --- Rotina para que as leituras sejam a cada 10 segundos (Orientação do Fabricante do Sensor)
   
  unsigned long tempo_atual = millis();                         // Realizamos a leitura atual do tempo em que o nosso Arduino está ligado
  if (tempo_atual - tempo_anterior_leitura > intervalo_leitura) // Pequena lógica para realizar leituras temporizadas sem parar o microcontrolador
    {
      tempo_anterior_leitura = tempo_atual;      
    
      // --- Realiza a leitura dos sensores ---  
        sensor_value[1] = analogRead(A0);
        sensor_value[2] = analogRead(A1);
        sensor_value[3] = analogRead(A2);
        sensor_value[4] = analogRead(A3);
        sensor_value[5] = analogRead(A4);
        sensor_value[6] = analogRead(A5);
        sensor_value[7] = analogRead(A6);
        sensor_value[8] = analogRead(A7);

      
  digitalWrite(power1, LOW);                                    // Desliga a tensão de (5v) para alimentar os sensores
            
      for (i = 1; i < 9; i = i + 1)
      
        {
          voltagem[i] = sensor_value[i] * (5.0/1023.0);
          Serial.print("Sensor ");
          Serial.print(i);
          Serial.print(".:");
          Serial.println(sensor_value[i]);
         // Serial.println(" mV");
        }
           
     }

//--- Realiza as operações matemáticas ---//    
    
    soma_tratamento1 = sensor_value[1] + sensor_value[2];               // Calcula a soma do tratamento 1
    soma_tratamento2 = sensor_value[3] + sensor_value[4];               // Calcula a soma do tratamento 2
    soma_tratamento3 = sensor_value[5] + sensor_value[6];               // Calcula a soma do tratamento 3
    soma_tratamento4 = sensor_value[7] + sensor_value[8];               // Calcula a soma do tratamento 4
   
    media_tratamento1 = soma_tratamento1 / 2;                           // Calcula a média do tratamento 1
    media_tratamento2 = soma_tratamento2 / 2;                           // Calcula a média do tratamento 2
    media_tratamento3 = soma_tratamento3 / 2;                           // Calcula a média do tratamento 3
    media_tratamento4 = soma_tratamento4 / 2;                           // Calcula a média do tratamento 4
    

} //end leitura_sensores()

//=================================================================================================================
void compara_sensores() 
{
// --- Realiza as comparações dos sensores e liga as válvulas solenóides se for necessário ---  
    if (millis () - tempo_anterior_comparacao >= intervalo_comparacao)                                     // Verifica se passou o intervalo determinado para realizar as comparações
     {
        tempo_anterior_comparacao = millis ();                                                //  reavalia tempo
        if (media_tratamento4 >= L0 && media_tratamento4 <= L4)
          {
            relays.SetRelay(4, SERIAL_RELAY_ON, 1);                       // liga valvula 4                            
            timer1_anterior_irrigacao = millis ();
            nivel_umidade_t1 = ("Baixa");
            status = ("Irrigando");
            status_solenoide_t1 = ("Tratamento 1 Ligado");
          }
        if (media_tratamento2 >= L0 && media_tratamento2 <= L2)
          {
            relays.SetRelay(2, SERIAL_RELAY_ON, 1);                       // liga valvula 2   
            timer2_anterior_irrigacao = millis ();
            nivel_umidade_t2 = ("Baixa");
            status = ("Irrigando");
            status_solenoide_t2 = ("Tratamento 2 Ligado");
          }
        if (media_tratamento3 >= L0 && media_tratamento3 <= L3)
          {
            relays.SetRelay(3, SERIAL_RELAY_ON, 1);                       // liga valvula 3  
            timer3_anterior_irrigacao = millis ();
            nivel_umidade_t3 = ("Baixa");
            status = ("Irrigando");
            status_solenoide_t3 = ("Tratamento 3 Ligado");
          }
        if (media_tratamento1 >= L0 && media_tratamento1 <= L1)
          {
            relays.SetRelay(1, SERIAL_RELAY_ON, 1);                       // liga valvula 1 
            timer4_anterior_irrigacao = millis ();   
            nivel_umidade_t4 = ("Baixa");
            status = ("Irrigando");
            status_solenoide_t4 = ("Tratamento 4 Ligado");
          }   
      }
// --- Desliga as válvulas solenóides com base nos intervalos determinados ---        
     if (millis () - timer1_anterior_irrigacao >= timer1_irrigacao)       //  Verifica os intervalos que as válvulas solenóides permanecem ligadas
      {
      relays.SetRelay(4, SERIAL_RELAY_OFF, 1);                            // desliga valvula 4                    
      nivel_umidade_t1 = ("Media");
      status = ("Nao Irrigando");
      status_solenoide_t1 = ("Tratamento 1 Desligado");
      }
      if (millis () - timer2_anterior_irrigacao >= timer2_irrigacao)
      {
      relays.SetRelay(2, SERIAL_RELAY_OFF, 1);                            // desliga valvula 2
      nivel_umidade_t2 = ("Media");
      status = ("Nao Irrigando"); 
      status_solenoide_t2 = ("Tratamento 2 Desligado");     
      }   
      if (millis () - timer3_anterior_irrigacao >= timer3_irrigacao)
      {
      relays.SetRelay(3, SERIAL_RELAY_OFF, 1);                            // desliga valvula 3
      nivel_umidade_t3 = ("Media");
      status = ("Nao Irrigando"); 
      status_solenoide_t3 = ("Tratamento 3 Desligado");     
      }
      if (millis () - timer4_anterior_irrigacao >= timer4_irrigacao)
      {
      relays.SetRelay(1, SERIAL_RELAY_OFF, 1);                            // desliga valvula 1
      nivel_umidade_t4 = ("Media");
      status = ("Nao Irrigando");  
      status_solenoide_t4 = ("Tratamento 4 Desligado");    
      }
}// end compara_sensores()

//=================================================================================================================
void imprime_media()                                        // Função que exibe informações dos sensores de umidade no display (menu2)
{
  lcd.setCursor(0, 0);
  lcd.print("* NIVEL DE UMIDADE *");
  lcd.setCursor(0,4);
  lcd.print("*** CAMPUS CERES ***");
  
  lcd.setCursor(0, 1);
  lcd.print("S1: ");
  lcd.setCursor(4,1);
  lcd.print(media_tratamento1);

  lcd.setCursor(10, 1);
  lcd.print("S2: ");
  lcd.setCursor(14,1);
  lcd.print(media_tratamento2);

  lcd.setCursor(0, 2);
  lcd.print("S3: ");
  lcd.setCursor(4,2);
  lcd.print(media_tratamento3);
  
  lcd.setCursor(10, 2);
  lcd.print("S4: ");
  lcd.setCursor(14,2);
  lcd.print(media_tratamento4);
} //end imprime_media()

//=================================================================================================================
void percentual_umidade()                                       //Função exibe o percentual de umidade em mV (Menu4)
{
  lcd.setCursor(0, 0);
  lcd.print("* NIVEL DE UMIDADE *");
  lcd.setCursor(0,4);
  lcd.print("*** CAMPUS CERES ***");
  
  lcd.setCursor(0, 1);
  lcd.print("S1: ");
  lcd.setCursor(4,1);
  lcd.print(nivel_umidade_t1);

  lcd.setCursor(10, 1);
  lcd.print("S2: ");
  lcd.setCursor(14,1);
  lcd.print(nivel_umidade_t2);

  lcd.setCursor(0, 2);
  lcd.print("S3: ");
  lcd.setCursor(4,2);
  lcd.print(nivel_umidade_t3);
  
  lcd.setCursor(10, 2);
  lcd.print("S4: ");
  lcd.setCursor(14,2);
  lcd.print(nivel_umidade_t4);
} //end percentual_umidade()

//=================================================================================================================
void sensores_t1()                                        // Função que exibe informações dos sensores de umidade do tratamento 1 no display (menu5)
{
  lcd.setCursor(0, 0);
  lcd.print("*** SENSORES T1 ***");
  lcd.setCursor(0,4);
  lcd.print("*** CAMPUS CERES ***");
  
  lcd.setCursor(0, 1);
  lcd.print("S1: ");
  lcd.setCursor(4,1);
  lcd.print(sensor_value[1]);

  lcd.setCursor(10, 1);
  lcd.print("S2: ");
  lcd.setCursor(14,1);
  lcd.print(sensor_value[2]);

// Valores exibidos no display em mV
  /*lcd.setCursor(0, 2);
  lcd.print("S1: ");
  lcd.setCursor(4,2);
  lcd.print(voltagem[3]);
  
  lcd.setCursor(10, 2);
  lcd.print("S2: ");
  lcd.setCursor(14,2);
  lcd.print(voltagem[4]); 
  */
} //end sensores_t1()

//=================================================================================================================
void sensores_t2()                                        // Função que exibe informações dos sensores de umidade do tratamento 2 no display (menu6)
{
  lcd.setCursor(0, 0);
  lcd.print("*** SENSORES T2 ***");
  lcd.setCursor(0,4);
  lcd.print("*** CAMPUS CERES ***");
  
  lcd.setCursor(0, 1);
  lcd.print("S1: ");
  lcd.setCursor(4,1);
  lcd.print(sensor_value[3]);

  lcd.setCursor(10, 1);
  lcd.print("S2: ");
  lcd.setCursor(14,1);
  lcd.print(sensor_value[4]);

// Valores exibidos no display em mV
  /*lcd.setCursor(0, 2);
  lcd.print("S1: ");
  lcd.setCursor(4,2);
  lcd.print(voltagem[3]);
  
  lcd.setCursor(10, 2);
  lcd.print("S2: ");
  lcd.setCursor(14,2);
  lcd.print(voltagem[4]); 
  */
} //end sensores_t2()

//=================================================================================================================
void sensores_t3()                                        // Função que exibe informações dos sensores de umidade do tratamento 3 no display (menu7)
{
  lcd.setCursor(0, 0);
  lcd.print("*** SENSORES T3 ***");
  lcd.setCursor(0,4);
  lcd.print("*** CAMPUS CERES ***");
  
  lcd.setCursor(0, 1);
  lcd.print("S1: ");
  lcd.setCursor(4,1);
  lcd.print(sensor_value[5]);

  lcd.setCursor(10, 1);
  lcd.print("S2: ");
  lcd.setCursor(14,1);
  lcd.print(sensor_value[6]);

// Valores exibidos no display em mV
  /*lcd.setCursor(0, 2);
  lcd.print("S1: ");
  lcd.setCursor(4,2);
  lcd.print(voltagem[5]);
  
  lcd.setCursor(10, 2);
  lcd.print("S2: ");
  lcd.setCursor(14,2);
  lcd.print(voltagem[6]); 
  */
} //sensores_t3()

//=================================================================================================================
void sensores_t4()                                        // Função que exibe informações dos sensores de umidade do tratamento 4 no display (menu8)
{
  lcd.setCursor(0, 0);
  lcd.print("*** SENSORES T4 ***");
  lcd.setCursor(0,4);
  lcd.print("*** CAMPUS CERES ***");
  
  lcd.setCursor(0, 1);
  lcd.print("S1: ");
  lcd.setCursor(4,1);
  lcd.print(sensor_value[7]);

  lcd.setCursor(10, 1);
  lcd.print("S2: ");
  lcd.setCursor(14,1);
  lcd.print(sensor_value[8]);

// Valores exibidos no display em mV
  /*lcd.setCursor(0, 2);
  lcd.print("S1: ");
  lcd.setCursor(4,2);
  lcd.print(voltagem[7]);
  
  lcd.setCursor(10, 2);
  lcd.print("S2: ");
  lcd.setCursor(14,2);
  lcd.print(voltagem[8]); 
  */
} //end sensores_t4()

//=================================================================================================================
void liga_bomba()                                               // Função que verifica as válvulas solnóides e liga/desliga a bomba
{
 if  ((relays.GetState(1,1) == SERIAL_RELAY_ON) || (relays.GetState(2,1) == SERIAL_RELAY_ON) || (relays.GetState(3,1) == SERIAL_RELAY_ON) || (relays.GetState(4,1) == SERIAL_RELAY_ON))
  {  
     digitalWrite(porta_bomba, LOW);
        }
        else
         {
          digitalWrite(porta_bomba, HIGH);      
         }
}
// end liga_bomba()

//=================================================================================================================
void liga_sim900()
{
// --- Inicializa o SIM900 e envia SMS informando ---
  Serial.println("Testando GSM shield...");
     //Inicia a configuracao do Shield
     if (gsm.begin(2400)) 
     {
       Serial.println("nstatus=READY");
       started=true;
     } 
     else Serial.println("nstatus=IDLE");

     if(started) 
     {
       //Envia um SMS para o numero selecionado
       //Formato sms.SendSMS(<numero>,<mensagem>)
       if (sms.SendSMS("+5562985947974", "Sistema Inicializado"))
       Serial.println("nSMS sent OK");
     }
}
// end liga_sim900()

//=================================================================================================================
void envia_sms()
{
  if (digitalRead(porta_bomba) == LOW)
  {  
     if (emailSend == 0)
     {    
      if(started) 
       {
         //Envia um SMS para o numero selecionado
         //Formato sms.SendSMS(<numero>,<mensagem>)
         if (sms.SendSMS("+5562985947974", "Irrigacao Ligada"))
         Serial.println("nSMS sent OK");
         emailSend = 1;
       }
     }
   }        
      else
      {    
         if (emailSend == 1)
         {   
          if(started) 
           {
             //Envia um SMS para o numero selecionado
             //Formato sms.SendSMS(<numero>,<mensagem>)
             if (sms.SendSMS("+5562985947974", "Irrigacao Desligada"))
             Serial.println("nSMS sent OK");
             emailSend = 0;
           }
          }
       }
}
// end envia_sms()

//=================================================================================================================
void cartao_de_memoria()                                            // Função grava as informções dos sensores no cartão de memória
{                                                               
  unsigned long tempo_atual = millis();                             // Realizamos a leitura atual do tempo em que o nosso Arduino Nano está ligado
  if (tempo_atual - tempo_anterior_gravacao > intervalo_gravacao)   // Pequena lógica para realizar leituras temporizadas sem parar o microcontrolador
  {
    tempo_anterior_gravacao = tempo_atual;                          // Guardamos o tempo anterior como o ultimo intervalo de tempo lido
    if (cartaoOk)                                                   // Grava os dados dos sensores no cartão de memória
    {
      dataFile = SD.open("datalog.csv", FILE_WRITE);
    }
    String leitura = "";                                            // Limpando Variáveis    // Limpo campo contendo string que será armazenada em arquivo CSV
    //                                                              // Caso os valores lidos do sensor não sejam válidos executamos as seguintes linhas
    if (isnan(sensor_value[1]) || isnan(sensor_value[2]) || isnan(sensor_value[3]) || isnan(sensor_value[4]) || isnan(sensor_value[5]) || isnan(sensor_value[6]) || isnan(sensor_value[7]) || isnan(sensor_value[8])) 

    {
      return ;                                                      // Retornamos a função para o seu ponto inicial
    }
    //                                                          
    //dataehora = rtc.getDateTime();                                  // Atribuindo valores instantâneos de data e hora à instância dataehora
//    char* diaSemana = calcularDiaDaSemana(dataehora.dayofweek);   // Armazena na variável
    
    // Se tudo estiver ok, escrevemos os dados a serem armazenados em uma String
    leitura = String(sensor_value[1]) + ";" + String(sensor_value[2]) + ";" + String(sensor_value[3]) + ";" + String(sensor_value[4]) + ";" + String(sensor_value[5]) + ";" + String(sensor_value[6]) + ";" + String(sensor_value[7]) + ";" + String(sensor_value[8]) + ";" + String(soma_tratamento1) + ";" + String(media_tratamento1) +  ";" + String(dht.readTemperature()) + ";" + String(dht.readHumidity()) + ";" + String(dataehora.day) + ";" + String(dataehora.month) + ";" + String(dataehora.year) + ";" + String(dataehora.hour) + ";" + String(dataehora.minute) + ";" + String(dataehora.second) + ";" + String(status) + ";";
    if (dataFile)                                                   // Se o arquivo estiver realmente aberto para leitura executamos as seguintes linhas de código
    {
      // Serial.println(leitura);                                   // Mostramos no monitor a linha que será escrita
      dataFile.println(leitura);                                    // Escrevemos no arquivos e pulamos uma linha
      dataFile.close();                                             // Fechamos o arquivo
    }
  }
} //end cartao_de_memória()

//=================================================================================================================
void debug()
{

}
//pode-se implementar quantos menus quiser
//............
