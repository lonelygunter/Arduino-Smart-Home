#define SWITCHPIN 6 // pin di controllo del tilt ball switch

// VARIABILI:
int switchState = LOW; // stato del sensore

void setup() {

  Serial.begin(9600);

  pinMode(SWITCHPIN, INPUT_PULLUP); // set del pin come input con pull-up resistor
}

void loop() {
  switchState = digitalRead(SWITCHPIN); // lettura dello stato del sensore

  if (switchState == LOW){ // check dello stato della maniglia
    Serial.println("🟢  MANIGLIA A RIPOSO"); // segnala che la maniglia è a riposo
  } else {
    Serial.println("🔴  MANIGLIA ABBASSATA"); // segnala che qualcuno sta abbassando la maniglia
  }
  
  delay(100);
}