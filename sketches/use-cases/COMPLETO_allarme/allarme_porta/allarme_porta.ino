#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define MIC_PIN 2 // pin del sensore con microfono
#define SWITCHPIN 6 // pin di controllo del tilt ball switch
#define REED_PIN 4 // pin dell'ampollina reed
#define BUZZER_PIN 5 // pin del buzzer

#define LCD_MAX_X 16 // massimo valore dell'asse delle X del display
#define LCD_MAX_Y 2 // massimo valore dell'asse delle Y del display
#define JS_X A0 // pin analogico dell'asse X del joystick
#define JS_Y A1 // pin analogico dell'asse Y del joystick
#define JS_BUTTON 3 // pin digitale del pulsante del joystick
#define JS_RANGE_INF 300 // range inferiore dei valori del joystick
#define JS_RANGE_SUP 700 // range superiore dei valori del joystick
#define PSW_SIZE 4 // max caratteri per la password


// VARIABILI:
LiquidCrystal_I2C lcd(0x27, LCD_MAX_X, LCD_MAX_Y); // display LCD I2C con 16 colonne and 2 righe
int reedState = LOW; // stato dell'ampollina reed (LOW=porta chiusa, HIGH=porta aperta)
int alarmState = LOW; // stato dell'allarme (LOW=spento, HIGH=attivo)
int continuedAlarm = LOW; // stato dell'allarme continuato
int sounds = LOW; // indica se è stato rivelato un suono
int switchState = LOW; // stato del sensore
bool buttonPressed = false;
float humidity = 0;
float temperature = 0;

// METODI:
void visualizeTemp(void); // metodo per far visualizzare i valori di temperature ed umidità sullo screen lcd
void receiveTemp(int bytes); // metodo per leggere la temperatura inviata dallo slave
void sound_check(void); // metodo per identificare la cattura di un suono nell'ambiente
void check_alarm_state(void); // metodo per verificare lo stato dell'allarme
void activate_alarm_if(void); // metdo per verificare se l'allarme debba scattare
void verbose(void); // metodo per stampare sul serial monitor lo stato dei sensori
void continued_alarm(void); // metodo per far continare l'allarme anche se i sensori sono tornati a riposo

void setup() {

  Serial.begin(9600);

  // setup modalità dei pin
  pinMode(REED_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MIC_PIN, INPUT);
  pinMode(SWITCHPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MIC_PIN), sound_check, RISING); // set di interrupt esterni
  psw_setup(); // setup dei pin delle funzioni di password
  attachInterrupt(digitalPinToInterrupt(JS_BUTTON), buttonInterrupt, RISING);
}

void loop() {

  while (buttonPressed) {
    Serial.println("PASSWORD...");
    psw_manager();
  }

  reedState = digitalRead(REED_PIN); // lettura stato dell'ampollina reed
  switchState = digitalRead(SWITCHPIN); // lettura dello stato del sensore

  check_alarm_state(); // verificare lo stato dell'allarme

  delay(1000);

  activate_alarm_if(); // verificare se l'allarme debba scattare

  verbose(); // stampare sul serial monitor lo stato dei sensori

  visualizeTemp();
  
  continued_alarm(); // far continare l'allarme anche se i sensori sono tornati a riposo
  
}


// METODI CUSTOM:

// metodo per far visualizzare i valori di temperature ed umidità sullo screen lcd
void visualizeTemp(void){
  lcd.setCursor(0, 0);
  lcd.print("hum: " + String(humidity));
  lcd.setCursor(0, 1);
  lcd.print("temp: " + String(temperature));
} 

// metodo per leggere la temperatura inviata dallo slave
void receiveTemp(int bytes) {
  if (bytes == sizeof(float) * 2) {
    byte receivedBytes[sizeof(float) * 2];

    int i = 0;
    while (Wire.available()) {
      receivedBytes[i] = Wire.read(); // read the byte array from the master
      i++;
    }

    // unpack the byte array back into the two floats
    memcpy(&humidity, &receivedBytes[0], sizeof(float));
    memcpy(&temperature, &receivedBytes[sizeof(float)], sizeof(float));
  } else {
    Serial.println("Invalid data size received"); // handle the case when data of an unexpected size is received
  }
}

// metodo per identificare la cattura di un suono nell'ambiente
void sound_check(void){ sounds = HIGH; }

// metodo per far continare l'allarme anche se i sensori sono tornati a riposo
void continued_alarm(void){

  if (continuedAlarm == HIGH){
    tone(BUZZER_PIN, 400, 500); // allarme continua a suonare dopo essere stato attivato una prima volta
    Serial.println("🚨 ALLARME");
  }
}

// metodo per verificare lo stato dell'allarme
void check_alarm_state(void){
    
  if (alarmState == LOW){
    // reset delle variabili
    continuedAlarm = LOW;
    sounds = LOW;
    switchState = LOW;
    Serial.println("⚠️ ALLARME DISATTIVATO");
  } else {
    Serial.println("⚠️ ALLARME ATTIVATO");
  }
}

// metdo per verificare se l'allarme debba scattare
void activate_alarm_if(void){

  if (alarmState == HIGH && (reedState == HIGH || sounds == HIGH || switchState == HIGH)) { // check dell'attivazione dell'allarme inserito
    
    continuedAlarm = HIGH; // attivazione dell'allarme continuato
    tone(BUZZER_PIN, 400, 500); // attivazione dell'allarme
    Serial.println("🚨 ALLARME");
  }
}

// metodo per stampare sul serial monitor lo stato dei sensori
void verbose(void) {

  Serial.println(" ");

  if (reedState == LOW){
    Serial.println("🚪🗝 PORTA CHIUSA");
  } else {
    Serial.println("🚪   PORTA APERTA");
  }

  if (sounds == LOW){
    Serial.println("🔇   NESSUN RUMORE");
  } else {
    Serial.println("🔊   RUMORE");
  }

  if (switchState == LOW){
    Serial.println("📫   MANIGLIA A RIPOSO");
  } else {
    Serial.println("📪   MANIGLIA ABBASSATA");
  }

  Serial.println(" ");
}

void buttonInterrupt() {
  buttonPressed = true; // set flag
}