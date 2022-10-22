//16/08/2022
// --- Bibliotecas Auxiliares ---

//Carrega as bibliotecas para o display LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Carrega a biblioteca do cartão de memória
#include <SPI.h>
#include <SD.h>

//Carrega biblioteca do DS3231
#include <DS3231.h>                                     // alteração   DS3231.h                
DS3231 rtc;                                             // Criação do objeto do tipo DS3231
RTCDateTime dataehora;                                  // Criação do objeto do tipo RTCDateTime

//Carrega a biblioteca do Sensor de Temperatura
#include <DHT.h>                                        // Carrega biblioteca do sensor DHT
#define DHTPIN 45                                       // Pino DATA do Sensor ligado na porta Digital 45
#define DHTTYPE DHT22                                   // Define o tipo de sensor DHT utilizado
DHT dht(DHTPIN, DHTTYPE);                               // Cria um objeto da classe dht

//=================================================================================================================
// --- Mapeamento de Hardware ---
#define butUp    40                                     // Botão para selecionar tela acima no digital 43
#define butDown  42                                     // Botão para selecionar tela abaixo no digital 42
//#define butP     43                                     // Botão de ajuste mais no digital 40
//#define butM     41                                     // Botão de ajuste menos no digital 41

//=================================================================================================================
// --- Range de Umidade do Solo ---
#define    L0        80
#define    L1        235
#define    L2        218
#define    L3        210
#define    L4        225

//--- Número de menus ---
#define quantidade_menus 3                              // Quantidade de menus, se acrescentar mais algum é necessário alterar o valor 

//=================================================================================================================
// --- 4 Relay Module das válvulas solenóides ---
#define PIN_RELAY_1  32 // Arduino porta, rele 1 conectado na porta 32 
#define PIN_RELAY_2  33 // Arduino porta, rele 2 conectado na porta 33 
#define PIN_RELAY_3  34 // Arduino porta, rele 3 conectado na porta 34 
#define PIN_RELAY_4  35 // Arduino porta, rele 4 conectado na porta 35 

//=================================================================================================================
// --- Protótipo das Funções Auxiliares ---
void changeMenu();                                      // Função para modificar o menu atual
void dispMenu();                                        // Função para mostrar o menu atual
void data_hora();                                       // Função do menu1, data e hora
void temperatura();                                     // Função do menu2, temperatura
void leitura_sensores();                                // Função que lê os dados dos sensores de umidade
void compara_sensores();                                // Faz a comparação dos valores obtidos pelos sensores e dispara o comando para ligar os reles
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
int sensor_value[5], i;
float VWC[5];
float voltagem[5];

int rele_1 = 0;                                              // Variável do rele 1 da solenoide ligar para ligar o rele da bomba
int rele_2 = 0;                                              // Variável do rele 2 da solenoide ligar para ligar o rele da bomba
int rele_3 = 0;                                              // Variável do rele 3 da solenoide ligar para ligar o rele da bomba
int rele_4 = 0;                                              // Variável do rele 4 da solenoide ligar para ligar o rele da bomba

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
const int porta_bomba = 44;

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
//=================================================================================================================

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
  
// --- Configurações iniciais do RTC DS3231 (DATA E HORA) ---
//rtc.begin();                                            // Inicialização do RTC DS3231
//                                                      // Informacoes iniciais de data e hora
//                                                      // Apos setar as informacoes, comente a linha abaixo
//rtc.setDateTime(__DATE__, __TIME__);                  // Configurando valores iniciais do RTC DS3231

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

// --- 4 Relay Module das válvulas solenóides ---
pinMode(PIN_RELAY_1, OUTPUT);
pinMode(PIN_RELAY_2, OUTPUT);
pinMode(PIN_RELAY_3, OUTPUT);
pinMode(PIN_RELAY_4, OUTPUT);

digitalWrite(PIN_RELAY_1, HIGH);
digitalWrite(PIN_RELAY_2, HIGH);
digitalWrite(PIN_RELAY_3, HIGH);
digitalWrite(PIN_RELAY_4, HIGH);

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
    //Serial.println("Erro na leitura do arquivo não existe um cartão SD ou o módulo está conectado corretamente ?");    
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
      dataFile.println("SENSOR 1; SENSOR 2; SENSOR 3; SENSOR 4; TEMPERATURA (C); UMIDADE DO AR (%); DIA; MES; ANO; HORAS; MINUTOS; SEGUNDOS; STATUS1; STATUS2; STATUS3; STATUS4");
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

// --- Desenvolvimento das opções que aparecem no menu (Display) ---
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
             nivel_sensores();                            // Chama função dos que imprime os valores dos sensores
            
             break;                                       // break
    
    } //end switch menu

} //end dispMenu
//=================================================================================================================
// --- Início das Funções ---
void data_hora()                                        // Data e Hora (menu1)
{
lcd.setCursor(0,0);
lcd.print("***** IFGOIANO *****");
lcd.setCursor(0,4);
lcd.print("*** CAMPUS CERES ***");

  dataehora = rtc.getDateTime();                        // Le as informacoes do CI
  lcd.setCursor(4, 1);                                  // Imprime as informacoes no LCD
  lcd.print("Data e Hora");
  lcd.setCursor(2, 2);
  lcd.print(dataehora.day);                         
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
      
  digitalWrite(power1, LOW);                                    // Desliga a tensão de (5v) para alimentar os sensores
            
      for (i = 1; i < 5; i = i + 1)
      
        {
          voltagem[i] = sensor_value[i] * (5.0/1023.0);
          Serial.print("Sensor ");
          Serial.print(i);
          Serial.print(".:");
          Serial.println(sensor_value[i]);
         // Serial.println(" mV");
        }
           
     }  
     
} //end leitura_sensores()

//=================================================================================================================
void compara_sensores() 
{
// --- Realiza as comparações dos sensores e liga as válvulas solenóides se for necessário ---  
    if (millis () - tempo_anterior_comparacao >= intervalo_comparacao)                                     // Verifica se passou o intervalo determinado para realizar as comparações
     {
        tempo_anterior_comparacao = millis ();                                                //  reavalia tempo
        if (sensor_value[1] >= L0 && sensor_value[1] <= L1)
          {
            digitalWrite(PIN_RELAY_1, LOW);                       // liga valvula 1                            
            timer1_anterior_irrigacao = millis ();
            status_solenoide_t1 = ("Tratamento 1 Ligado");
            rele_1 = 1;
          }
        if (sensor_value[2] >= L0 && sensor_value[2] <= L2)
          {
            digitalWrite(PIN_RELAY_2, LOW);                       // liga valvula 2   
            timer2_anterior_irrigacao = millis ();
            status_solenoide_t2 = ("Tratamento 2 Ligado");
            rele_2 = 1;
          }
        if (sensor_value[3] >= L0 && sensor_value[3] <= L3)
          {
            digitalWrite(PIN_RELAY_3, LOW);                       // liga valvula 3  
            timer3_anterior_irrigacao = millis ();
            status_solenoide_t3 = ("Tratamento 3 Ligado");
            rele_3 = 1;
          }
        if (sensor_value[4] >= L0 && sensor_value[4] <= L4)
          {
            digitalWrite(PIN_RELAY_4, LOW);                       // liga valvula 4 
            timer4_anterior_irrigacao = millis ();   
            status_solenoide_t4 = ("Tratamento 4 Ligado");
            rele_4 = 1;
          }   
      }
// --- Desliga as válvulas solenóides com base nos intervalos determinados ---        
     if (millis () - timer1_anterior_irrigacao >= timer1_irrigacao)       //  Verifica os intervalos que as válvulas solenóides permanecem ligadas
      {
      digitalWrite(PIN_RELAY_1, HIGH);                            // desliga valvula 1                    
      status_solenoide_t1 = ("Tratamento 1 Desligado");
      rele_1 = 0;
      }
      if (millis () - timer2_anterior_irrigacao >= timer2_irrigacao)
      {
      digitalWrite(PIN_RELAY_2, HIGH);                            // desliga valvula 2
      status_solenoide_t2 = ("Tratamento 2 Desligado");
      rele_2 = 0;     
      }   
      if (millis () - timer3_anterior_irrigacao >= timer3_irrigacao)
      {
      digitalWrite(PIN_RELAY_3, HIGH);                            // desliga valvula 3
      status_solenoide_t3 = ("Tratamento 3 Desligado");     
      rele_3 = 0;
      }
      if (millis () - timer4_anterior_irrigacao >= timer4_irrigacao)
      {
      digitalWrite(PIN_RELAY_4, HIGH);                            // desliga valvula 4  
      status_solenoide_t4 = ("Tratamento 4 Desligado"); 
      rele_4 = 0;   
      }
}// end compara_sensores()

//=================================================================================================================
void nivel_sensores()                                        // Função que exibe informações dos sensores de umidade no display (menu3)
{
  lcd.setCursor(0, 0);
  lcd.print("* NIVEL DE UMIDADE *");
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

  lcd.setCursor(0, 2);
  lcd.print("S3: ");
  lcd.setCursor(4,2);
  lcd.print(sensor_value[3]);
  
  lcd.setCursor(10, 2);
  lcd.print("S4: ");
  lcd.setCursor(14,2);
  lcd.print(sensor_value[4]);
} //end nivel_sensores()



//=================================================================================================================
void liga_bomba()                                               // Função que verifica as válvulas solnóides e liga/desliga a bomba
{
 if  ((rele_1 == 1) || (rele_2 == 1) || (rele_3 == 1) || (rele_4 == 1))
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
    if (isnan(sensor_value[1]) || isnan(sensor_value[2]) || isnan(sensor_value[3]) || isnan(sensor_value[4])) 

    {
      return ;                                                      // Retornamos a função para o seu ponto inicial
    }
    //                                                          
    //dataehora = rtc.getDateTime();                                  // Atribuindo valores instantâneos de data e hora à instância dataehora
//    char* diaSemana = calcularDiaDaSemana(dataehora.dayofweek);   // Armazena na variável
    
    // Se tudo estiver ok, escrevemos os dados a serem armazenados em uma String
    leitura = String(sensor_value[1]) + ";" + String(sensor_value[2]) + ";" + String(sensor_value[3]) + ";" + String(sensor_value[4]) +  ";" + String(dht.readTemperature()) + ";" + String(dht.readHumidity()) + ";" + String(dataehora.day) + ";" + String(dataehora.month) + ";" + String(dataehora.year) + ";" + String(dataehora.hour) + ";" + String(dataehora.minute) + ";" + String(dataehora.second) + ";" + String(rele_1) + ";" + String(rele_2) + ";" + String(rele_3) + ";" + String(rele_4) + ";";                     
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
