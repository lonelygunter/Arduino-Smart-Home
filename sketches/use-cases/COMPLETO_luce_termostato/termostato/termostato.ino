#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>

#define RELAY_PIN 6 // pin del relay
#define DS_PIN 4 // pin del DS18B20
#define DHT_PIN 5 // pin del DHT11

#define DHT_TYPE DHT11 // tipo del DHT11
#define HUM_MIN 30 // valore minimo di umidità
#define HUM_MAX 60 // valore massimo di umidità

#define TEM_MIN 30 // valore minimo di temperatura
#define TEM_MAX 60 // valore massimo di temperatura

#define OBSTACLE_IR_1 2 // pin del primo sensore di ostacoli IR
#define OBSTACLE_IR_2 3 // pin del decondo sensore di ostacoli IR
#define LED_PIN 7 // pin del led


// VARIABILI:
float humidity;
float temperature;
DHT dht(DHT_PIN, DHT_TYPE); // sensore DHT11 per l'umidità
OneWire oneWire(DS_PIN); // protocollo 1-Wire
DallasTemperature sensors(&oneWire); // sensore DS18B20
bool sensorActivated = false; // boolean per capire se il senore è stato triggerato

// METODI:
void irInterrupt(void); // metodo per sapere se l'interrupt è stato triggerato
void makeMeasures(void); // metodo per prendere dei dati
void sendTemperatures(float temperature); // metodo per l'invio della temperatura sull bus I2C


void setup(){

  Wire.begin(); // inizializza nel bus i2c con address 8

  Serial.begin(9600);

  // setup relay
  pinMode(RELAY_PIN, OUTPUT); // set del pin come output
  digitalWrite(RELAY_PIN, HIGH); // set del relay C-NO

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
  
  makeMeasures(); // prendere le misure tramite i sensori

  delay(250);
  sendTemperatures();

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

// metodo per l'invio della temperatura sull bus I2C
void sendTemperatures(void){
  // pack the two floats into a byte array
  byte byteArray[sizeof(float) * 2];
  memcpy(&byteArray[0], &humidity, sizeof(float));
  memcpy(&byteArray[sizeof(float)], &temperature, sizeof(float));

  Wire.beginTransmission(8); // transmit to slave device with address 8
  Wire.write(byteArray, sizeof(byteArray)); // send the byte array
  Wire.endTransmission(); // stop transmitting

  Serial.print("Sent numbers: ");
  Serial.print(humidity);
  Serial.print(", ");
  Serial.println(temperature);

  delay(1000); // wait for a second before sending the next numbers
}
