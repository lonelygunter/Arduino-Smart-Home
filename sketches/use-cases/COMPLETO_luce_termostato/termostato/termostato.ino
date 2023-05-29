#include "DHT.h"
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>

#define RELAY_PIN 2 // pin del relay
#define DS_PIN 3 // pin del DS18B20
#define DHT_PIN 4 // pin del DHT11
#define LED_PIN 7 // pin del led

#define DHT_TYPE DHT11 // tipo del DHT11
#define HUM_MIN 30 // valore minimo di umidità
#define HUM_MAX 60 // valore massimo di umidità

#define TEM_MIN 30 // valore minimo di temperatura
#define TEM_MAX 60 // valore massimo di temperatura

#define LCD_COL 16 // # colonne LCD I2C
#define LCD_ROW 2 // # righe LCD I2C

// VARIABILI:
float humidity;
float temperature;
int counter = 0;
LiquidCrystal_I2C lcd(0x27, LCD_COL, LCD_ROW); // display LCD I2C con 16 colonne and 2 righe
DHT dht(DHT_PIN, DHT_TYPE); // sensore DHT11 per l'umidità
OneWire ds(DS_PIN); // sensore 18B20

// METODI:
void print_humidity(float humidity); // metodo per il print dell'umidità sul display LCD
void print_temperature(float temperature); // metodo per il print della temperatura sul display LCD
float get_temperature(void); // metodo per leggere temperatura dal DS18B20


void setup(){

  Serial.begin(9600);

  // setup relay
  pinMode(RELAY_PIN, OUTPUT); // set del pin come output
  digitalWrite(RELAY_PIN, HIGH); // set del relay C-NO

  // setup LCD
  lcd.init(); // inizializzaione display LCD
  lcd.backlight(); // set della backlight

  // setup DHT11
  dht.begin(); // inizializzazione DHT11

  // setup sensori di ostacoli IR
  ir_setup();

  // setup del led
  pinMode(LED_PIN, OUTPUT);
}

void loop(){

  ir_manager();

  if (counter >= 1){
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  lcd.clear(); // clear del display
  
  humidity = dht.readHumidity(); // prelievo umidità con DHT11
  temperature = get_temperature(); // prelievo temperatura con DS18B20
  
  print_humidity(humidity);
  print_temperature(temperature);
  
  Serial.println((String)"💧 Humidity: " + humidity + " %");
  Serial.println((String)"🌡 Temperature: " + temperature + " C");

  delay(2000); // delay di 2 sec
}


// METODI CUSTOM:

// metodo per il print dell'umidità sul display LCD
void print_humidity(float humidity){
  if(humidity < HUM_MIN || humidity > HUM_MAX){    
    lcd.setCursor(1, 0);
    lcd.print("! Hum: ");
    lcd.print(humidity);
    lcd.print(" %");
  } else {
    lcd.setCursor(1, 0); // sposto il cursore su (1, 0)
    lcd.print("Hum: "); // stampo sul display l'umidità corrente
    lcd.print(humidity);
    lcd.print(" %");
  }
}

// metodo per il print della temperatura sul display LCD
void print_temperature(float temperature){
  if(temperature < TEM_MIN || temperature > TEM_MAX){
    lcd.setCursor(1, 1); // sposto il cursore su (1, 1)
    lcd.print("! Tem: "); // stampo sul display la temperatura corrente
    lcd.print(temperature);
    lcd.print((char)223);
    lcd.print(" C");
  } else {
    lcd.setCursor(1, 1);
    lcd.print("Tem: ");
    lcd.print(temperature);
    lcd.print((char)223);
    lcd.print(" C");
  }
}

// metodo per leggere temperatura dal DS18B20
float get_temperature(void){

  byte data[12]; // vettore per storage di "Scratchpad data" dal sensore DS18B20
  byte address[8]; // vettore per storage del ROM code del vettore

  // search del sensore OneWire tramite indirizzo
  if ( !ds.search(address)){ // check della presenza del sensore
    Serial.println(*address,8);
    ds.reset_search();
    return 1;
  }

  // check di cambi accidentali dei dati digitali
  if ( OneWire::crc8(address, 7) != address[7]) { // check del CRC code
    Serial.println("CRC non valido!");
    return 1;
  }

  // filtro della famiglia del sensore
  if ( address[0] != 0x10 && address[0] != 0x28) { // check della famiglia del sensore
    Serial.println("Dispositivo non riconosciuto");
    return 1;
  }

  ds.reset(); // reset del sensore

  ds.select(address); // selezionare l'indirizzo dello slave
  ds.write(0x44,1); // scrivere dati dall'ADC interno nei registri di Scratchpad

  byte present = ds.reset();

  ds.select(address);
  ds.write(0xBE); // lettura dallo Scratchpad

  // lettura dei byte dallo Scratchpad
  for (int i = 0; i < 9; i++){
    data[i] = ds.read();
  }

  // Serial.print("ROM code: ");
  // for(int i=7; i>=0; i--){
  //   Serial.print(address[i],HEX);
  //   Serial.print(" ");
  // }  
  // Serial.println(" ");

  ds.reset_search(); // restart della search;

  // immagazzina 12 bit in due variabili byte
  byte MSB = data[1];
  byte LSB = data[0];
  
  float tempRead = ((MSB << 8) | LSB); // utilizzare il complemento a due
  float TemperatureSum = tempRead / 16; // convertire dati da raw in Celsius

  return TemperatureSum;
}

