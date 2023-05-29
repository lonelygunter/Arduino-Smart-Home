#define MIC_PIN 2 // pin del sensore con microfono
#define SWITCHPIN 3 // pin di controllo del tilt ball switch
#define REED_PIN 4 // pin dell'ampollina reed
#define BUZZER_PIN 5 // pin del buzzer
#define BUTTON_PIN 6 // pin del pulsante

// VARIABILI:
int buttonState; // stato del pulsante
int lastButtonState; // stato precedente del pulsante
int reedState = LOW; // stato dell'ampollina reed (LOW=porta chiusa, HIGH=porta aperta)
int alarmState = LOW; // stato dell'allarme (LOW=spento, HIGH=attivo)
int continuedAlarm = LOW; // stato dell'allarme continuato
int sounds = LOW; // indica se è stato rivelato un suono
int switchState = LOW; // stato del sensore

// METODI:
void sound_check(void); // metodo per identificare la cattura di un suono nell'ambiente
void check_alarm_state(void); // metodo per verificare lo stato dell'allarme
void activate_alarm_if(void); // metdo per verificare se l'allarme debba scattare
void verbose(void); // metodo per stampare sul serial monitor lo stato dei sensori
void continued_alarm(void); // metodo per far continare l'allarme anche se i sensori sono tornati a riposo


void setup() {

  Serial.begin(9600);

  // setup modalità dei pin
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(REED_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MIC_PIN, INPUT);
  pinMode(SWITCHPIN, INPUT_PULLUP);
  attachInterrupt(0, sound_check, RISING); // set di interrupt esterni
}

void loop() {

  buttonState = digitalRead(BUTTON_PIN); // lettura stato del button
  reedState = digitalRead(REED_PIN); // lettura stato dell'ampollina reed
  switchState = digitalRead(SWITCHPIN); // lettura dello stato del sensore

  check_alarm_state(); // verificare lo stato dell'allarme

  delay(1000);

  activate_alarm_if(); // verificare se l'allarme debba scattare

  verbose(); // stampare sul serial monitor lo stato dei sensori
  
  continued_alarm(); // far continare l'allarme anche se i sensori sono tornati a riposo

  lastButtonState = buttonState; // set del vecchio stato del pulsante

  delay(1000);
}


// METODI CUSTOM:

// metodo per identificare la cattura di un suono nell'ambiente
void sound_check(void){ sounds = HIGH; }

// metodo per verificare lo stato dell'allarme
void check_alarm_state(void){

  if (buttonState == LOW && lastButtonState == HIGH) { // check della pressione del button
    alarmState = !alarmState; // cambio di stato dell'allarme
    
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

// metodo per far continare l'allarme anche se i sensori sono tornati a riposo
void continued_alarm(void){

  if (continuedAlarm == HIGH){
    tone(BUZZER_PIN, 400, 500); // allarme continua a suonare dopo essere stato attivato una prima volta
    Serial.println("🚨 ALLARME");
  }
}