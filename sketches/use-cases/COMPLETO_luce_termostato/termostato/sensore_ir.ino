bool direct = false; // se LOW da giù a su, se HIGH da su a giù
bool flag = false;
bool debounce = false;
int counter = 0;

void ir_setup() {

  Serial.begin(9600);

  // setup del led
  pinMode(LED_PIN, OUTPUT);

  // setup dei sensori IR
  pinMode(OBSTACLE_IR_1, INPUT);
  pinMode(OBSTACLE_IR_2, INPUT);
}

void ir_manager(){

  Serial.println(digitalRead(OBSTACLE_IR_1));
  Serial.println(digitalRead(OBSTACLE_IR_2));

  if (digitalRead(OBSTACLE_IR_1) == LOW || digitalRead(OBSTACLE_IR_2) == LOW){

    Serial.println("✅");

    if(digitalRead(OBSTACLE_IR_1) == LOW){
      debounce = true;
      direct=false;
    } else if(digitalRead(OBSTACLE_IR_2) == LOW){
      debounce = true;
      direct=true;
    }
    
    if (direct==false && debounce){
      
      flag=true;

      while (flag){
        while (digitalRead(OBSTACLE_IR_2) == HIGH);
        delay(5);
        if (digitalRead(OBSTACLE_IR_2) == LOW){
          flag=false;
          if (counter > 0){
            counter--;
          }
        }
      }
    }

    if (direct == true && debounce){

      flag=true;

      while (flag){
        while (digitalRead(OBSTACLE_IR_1) == HIGH);
        delay(5);
        if(digitalRead(OBSTACLE_IR_1) == LOW){
          flag=false;
          counter++;
        }
      }
    }

    // if(debounce){
    //   delay(1000);
    // }
    
    debounce = false;
      
  }
  
  if (counter >= 1){
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  Serial.println((String) "\n👥 " + counter);
}