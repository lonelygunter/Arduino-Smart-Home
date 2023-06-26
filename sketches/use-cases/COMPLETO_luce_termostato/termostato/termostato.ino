#include "DHT.h"
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define RELAY_PIN 6 // pin del relay
#define DS_PIN 4 // pin del DS18B20
#define DHT_PIN 5 // pin del DHT11

#define DHT_TYPE DHT11 // tipo del DHT11
#define HUM_MIN 30 // valore minimo di umidità
#define HUM_MAX 60 // valore massimo di umidità

#define TEM_MIN 30 // valore minimo di temperatura
#define TEM_MAX 60 // valore massimo di temperatura

#define LCD_COL 16 // # colonne LCD I2C
#define LCD_ROW 2 // # righe LCD I2C

#define OBSTACLE_IR_1 2 // pin del primo sensore di ostacoli IR
#define OBSTACLE_IR_2 3 // pin del decondo sensore di ostacoli IR
#define LED_PIN 7 // pin del led


// VARIABILI:
float humidity;
float temperature;
LiquidCrystal_I2C lcd(0x27, LCD_COL, LCD_ROW); // display LCD I2C con 16 colonne and 2 righe
DHT dht(DHT_PIN, DHT_TYPE); // sensore DHT11 per l'umidità
OneWire oneWire(DS_PIN); // protocollo 1-Wire
DallasTemperature sensors(&oneWire); // sensore DS18B20
bool sensorActivated = false; // boolean per capire se il senore è stato triggerato

// METODI:
void irInterrupt(void); // metodo per sapere se l'interrupt è stato triggerato
void makeMeasures(void); // metodo per prendere dei dati
void print_humidity(float humidity); // metodo per il print dell'umidità sul display LCD
void print_temperature(float temperature); // metodo per il print della temperatura sul display LCD


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

  // setup DS18B20
  sensors.begin(); // inizializzazione DS18B20

  // setup interrupt
  attachInterrupt(digitalPinToInterrupt(OBSTACLE_IR_1), irInterrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(OBSTACLE_IR_2), irInterrupt, RISING);
}

void loop(){

  if (sensorActivated) { // verificare se l'interrupt è stato triggerato
    ir_manager();
    sensorActivated = false; // reset flag
  }

  lcd.clear(); // clear del display
  
  makeMeasures(); // prendere le misure tramite i sensori

  print_humidity(humidity);
  print_temperature(temperature);

  Serial.println((String)"\n💧 Humidity: " + humidity + " %");
  Serial.println((String)"🌡 Temperature: " + temperature + " C");

}


// METODI CUSTOM:

// metodo per sapere se l'interrupt è stato triggerato
void irInterrupt(void){
  sensorActivated = true; // set flag
}

// metodo per prendere dei dati
void makeMeasures(void){
  humidity = dht.readHumidity(); // prelievo umidità con DHT11
  sensors.requestTemperatures(); // prelievo temperatura con DS18B20
  temperature = sensors.getTempCByIndex(0); // soricizzazione della temperatura con DS18B20
}

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
