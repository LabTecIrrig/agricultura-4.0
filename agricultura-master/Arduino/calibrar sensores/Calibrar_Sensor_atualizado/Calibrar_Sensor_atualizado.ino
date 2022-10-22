//Bibliotecas
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Portas utilizadas pelos sensores
const int LM35 = A3; 


//Declaração de Variáveis
float temp = 0; 
const int power1 = 2;
const int power2 = 3;
float VWC[5];
float voltagem[5];

int sensor_value[5], i;
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


// Caractere customizado - Símbolo do Grau (°)
byte custom[8] = {
  0b00111,          
  0b00101,
  0b00111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

// Inicializa o display no endereco 0x27
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);
 
void setup()
{
 //Inicializa o Display
 lcd.begin (20,4);

 // Cria nosso caractere definindo-o como o byte 5
 lcd.createChar(5, custom); 

}

void leitura_sensores()                                         // Lê os dados dos sensores de umidade do solo
{
  digitalWrite(power1, HIGH);                                   // Liga a tensão de (5v) para alimentar os sensores

// --- Rotina para que as leituras sejam a cada 10 segundos (Orientação do Fabricante do Sensor)
   
  unsigned long tempo_atual = millis();                         // Realizamos a leitura atual do tempo em que o nosso Arduino está ligado
  if (tempo_atual - tempo_anterior_leitura > intervalo_leitura) // Pequena lógica para realizar leituras temporizadas sem parar o microcontrolador
    {
      tempo_anterior_leitura = tempo_atual;      
    
      
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


void loop()
{
 
  //Leitura dos sensores
  int Sensor1 = analogRead(A1);
  int Sensor2 = analogRead(A2);
  int Sensor3 = analogRead(A3);
  int Sensor4 = analogRead(A4);

  //Leitura da temperatura
  temp = (float(analogRead(LM35))*5/(1023))/0.01;

  //Exibe temperatura no display
  lcd.setBacklight(HIGH);
  lcd.setCursor(3,0);
  
  //lcd.setCursor(14,0);
  //lcd.print(temp);

  //lcd.setCursor(18,0);  
  //lcd.write(5);

  //lcd.setCursor(19,0);
 // lcd.print("C");

  //Exibe os valores do sensores no display
  lcd.setCursor(0,0);
  lcd.print("Sensor 1.: ");
  lcd.setCursor(11,0);
  lcd.print(Sensor1);
  
  lcd.setCursor(0,1);
  lcd.print("Sensor 2.: ");
  lcd.setCursor(11,1);
  lcd.print(Sensor2);
  
  lcd.setCursor(0,2);
  lcd.print("Sensor 3.: ");
  lcd.setCursor(11,2);
  lcd.print(Sensor3);

  lcd.setCursor(0,3);
  lcd.print("Sensor 4.: ");
  lcd.setCursor(11,3);
  lcd.print(Sensor4);
  

  delay(1000);
  lcd.clear();
 
}
