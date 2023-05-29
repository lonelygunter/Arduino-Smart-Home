#define RELAY_PIN 2 // pin del relay

void setup() {

  pinMode(RELAY_PIN, OUTPUT); // set del pin come output

  digitalWrite(RELAY_PIN, HIGH); // set del relay C-NO

}

void loop() {

}