byte sensor1 = 5;
byte sensor2 = 6;
bool direct = false; // se LOW da giù a su, se HIGH da su a giù
bool flag = false;
bool debounce = false;
int counter = 0;

void setup() {
  Serial.begin(9600);
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
}

void loop() {
  if(digitalRead(sensor1)==LOW || digitalRead(sensor2)==LOW){
    Serial.println("ho trovato qualcosa");
    delay(20);
    if(digitalRead(sensor1)==LOW){
      debounce = true;
      direct=false;
    }
    else if (digitalRead(sensor2)==LOW){
      debounce = true;
      direct=true;
    }
    
    if(direct==false && debounce){
      //Serial.println(direct);
      flag=true;
      while(flag){
        while(digitalRead(sensor2)==HIGH);
        delay(20);
        if(digitalRead(sensor2)==LOW){
          flag=false;
          if(counter>0)counter--;
          }
        }
      }
    if(direct==true && debounce){
      //Serial.println(direct);
      flag=true;
      while(flag){
        while(digitalRead(sensor1)==HIGH);
        delay(20);
        if(digitalRead(sensor1)==LOW){
          flag=false;
          counter++;
          }
        }
      }
      if(debounce)  delay(1000);
      debounce = false;
      
    }
    Serial.print("# utenti= ");
    Serial.println(counter);
}
