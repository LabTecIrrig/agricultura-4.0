//Bibliotecas
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Portas utilizadas pelos sensores
const int LM35 = A3; 


//Declaração de Variáveis
float temp = 0; 

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
 
void loop()
{
  //Leitura dos sensores
  int Sensor1 = analogRead(A1);
  int Sensor2 = analogRead(A2);
  int Sensor3 = analogRead(A3);

  //Leitura da temperatura
  temp = (float(analogRead(LM35))*5/(1023))/0.01;

  //Exibe temperatura no display
  lcd.setBacklight(HIGH);
  lcd.setCursor(3,0);
  lcd.print(":: Sensores ::");
  
  //lcd.setCursor(14,0);
  //lcd.print(temp);

  //lcd.setCursor(18,0);  
  //lcd.write(5);

  //lcd.setCursor(19,0);
 // lcd.print("C");

  //Exibe os valores do sensores no display
  lcd.setCursor(0,1);
  lcd.print("Sensor 1.: ");
  lcd.setCursor(11,1);
  lcd.print(Sensor1);
  
  lcd.setCursor(0,2);
  lcd.print("Sensor 2.: ");
  lcd.setCursor(11,2);
  lcd.print(Sensor2);
  
  lcd.setCursor(0,3);
  lcd.print("Sensor 3.: ");
  lcd.setCursor(11,3);
  lcd.print(Sensor3);
  

  delay(1000);
  lcd.clear();
}
