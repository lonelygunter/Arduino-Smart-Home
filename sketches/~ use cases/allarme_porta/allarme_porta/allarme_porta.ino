#define BUTTON_PIN 4 // pin del pulsante
#define REED_PIN 2 // pin dell'ampollina reed
#define BUZZER_PIN 3 // pin del buzzer

int buttonState; // stato del pulsante
int lastButtonState; // stato precedente del pulsante
int reedState = LOW; // stato dell'ampollina reed (LOW=porta chiusa, HIGH=porta aperta)
int alarmState = LOW; // stato dell'allarme (LOW=spento, HIGH=attivo)
int continuedAlarm = LOW; // stato dell'allarme continuato

void setup() {

  Serial.begin(9600);

  // setup modalità dei pin
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(REED_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {

  buttonState = digitalRead(BUTTON_PIN); // lettura stato del button
  reedState = digitalRead(REED_PIN); // lettura stato dell'ampollina reed

  // DEBUG per identificare se la porta è chiusa
  // if (reedState == LOW){
  //   Serial.println("====================");
  //   Serial.print((String)"alarm          " + alarmState);
  //   Serial.print((String)"reed           " + reedState);
  //   Serial.print((String)"button         " + buttonState);
  //   Serial.print((String)"last button    " + lastButtonState);
  //   Serial.println("====================");
  //   delay(100);
  // }

  if (buttonState == LOW && lastButtonState == HIGH) { // check della pressione del button
    alarmState = !alarmState; // cambio di stato dell'allarme
    
    if (alarmState == LOW){
      continuedAlarm = LOW;
      Serial.println("⚠️ ALLARME DISATTIVATO");
    } else {
      Serial.println("⚠️ ALLARME ATTIVATO");
    }
  }

  delay(1000);

  // check if the button has been pressed
  if (alarmState == HIGH && reedState == HIGH) { // check dell'apertura della porta con allarme inserito
    Serial.println("🚪   PORTA APERTA");
    
    continuedAlarm = HIGH; // attivazione dell'allarme continuato
    tone(BUZZER_PIN, 400, 500); // attivazione dell'allarme
    Serial.println("🚨 ALLARME");
  }
  
  if (reedState == LOW){
    Serial.println("🚪🗝 PORTA CHIUSA");
  } else {
    Serial.println("🚪   PORTA APERTA");
  }
  
  if (continuedAlarm == HIGH){
    tone(BUZZER_PIN, 400, 500); // allarme continua a suonare dopo essere stato attivato una prima volta
    Serial.println("🚨 ALLARME");
  }

  lastButtonState = buttonState; // sett del vecchio stato del pulsante
  
  delay(1000);
}