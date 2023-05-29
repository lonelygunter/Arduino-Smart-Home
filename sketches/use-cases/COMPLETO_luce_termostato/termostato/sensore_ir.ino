#define OBSTACLE_IR_1 5 // pin del primo sensore di ostacoli IR
#define OBSTACLE_IR_2 6 // pin del decondo sensore di ostacoli IR

bool direct = false; // se LOW da giù a su, se HIGH da su a giù
bool flag = false;
bool debounce = false;

void ir_setup() {

  Serial.begin(9600);

  pinMode(OBSTACLE_IR_1, INPUT);
  pinMode(OBSTACLE_IR_2, INPUT);
}

void ir_manager(){

  if (digitalRead(OBSTACLE_IR_1) == LOW || digitalRead(OBSTACLE_IR_2) == LOW){

    Serial.println("ho trovato qualcosa");
    delay(20);

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
        delay(20);
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
        delay(20);
        if(digitalRead(OBSTACLE_IR_1) == LOW){
          flag=false;
          counter++;
        }
      }
    }

    if(debounce){
      delay(1000);
    }
    
    debounce = false;
      
  }

  Serial.println((String) "👥 " + counter);
}
