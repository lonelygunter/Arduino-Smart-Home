#include <LiquidCrystal_I2C.h>

#define LCD_MAX_X 16 // massimo valore dell'asse delle X del display
#define LCD_MAX_Y 2 // massimo valore dell'asse delle Y del display
#define JS_X A0 // pin analogico dell'asse X del joystick
#define JS_Y A1 // pin analogico dell'asse Y del joystick
#define JS_BUTTON 7 // pin digitale del pulsante del joystick
#define JS_RANGE_INF 300 // range inferiore dei valori del joystick
#define JS_RANGE_SUP 700 // range superiore dei valori del joystick
#define PSW_SIZE 4 // max caratteri per la password

// VARIABILI:
LiquidCrystal_I2C lcd(0x27, LCD_MAX_X, LCD_MAX_Y); // display LCD I2C con 16 colonne and 2 righe

// inizializzazione posizione del cursore
int cursor_X = 6; // valore asse X iniziale del cursore
int cursor_Y = 1; // valore asse X iniziale del cursore
int current_X = cursor_X; // posizione corrente del cursore
int current_index = 0; // posizione corrente dell'indice dell'array psw_chars
int enter_position = cursor_X+PSW_SIZE; // posizione del tasto enter
int last_action = LOW; // indica l'ultima azione eseguita (LOW=inserito lettera, HIGH=spostato cursore)

int insert = LOW; // per capire se è stato catturato un input

char psw_chars[16] = "0123456789abcdef"; // whitelist dei caratteri utilizzabili nella password
int psw_chars_size = sizeof(psw_chars)-1; // setup array size - 1 sennò fa buffer overflow

String right_psw = "1234"; // setting password giusta
String inserted_psw = "0000"; // inizializzazione password che verrà inserita dall'utente

byte enter[8] = { // array del tasto enter
  0b00000,
  0b00100,
  0b00010,
  0b11111,
  0b00010,
  0b00100,
  0b00000,
};

// METODI
void(* restartFunc) (void) = 0; // metodo per resettare il run del codice
void setupFrontEnd(void); // metodo per il setup del front end
void writePsw(void); // metodo per salvare nella stringa inserted_psw la password che si sta inserendo
void checkPsw(void); // metodo che verifica che la password inserita sia giusta
void inputOnClick(int x, int y); // metodo per indicare che ho preso l'input e da qui in poi può prenderne altri 
void continuosArray(void); // metodo per iterare sempre i valori sull'array
void frameRange(void); // metodo per non far oltrepassare un custom frame nel display


void psw_setup(){

  pinMode(JS_BUTTON, INPUT_PULLUP); // setup pin mode del pulsante

  // setup display
  lcd.init(); // inizializzaione display LCD
  lcd.backlight(); // set della backlight
  lcd.setCursor(cursor_X, cursor_Y); // inizializzazione posizione cursore sullo schermo LCD
  lcd.createChar(0, enter); // inizializzazione del carattere custom

  setupFrontEnd();
}


void psw_manager(){

  int x = analogRead(JS_X); // lettura dell'asse X del joystick
  int y = analogRead(JS_Y); // lettura dell'asse Y del joystick
  int button = !digitalRead(JS_BUTTON); // lettura del pulsante del joystick


  // DEBUG
  // Serial.println(x);
  // Serial.println(y);
  // Serial.println(button);
  // Serial.println(cursor_X);
  // Serial.println(current_X);
  // Serial.println(enter_position);
  // Serial.println(right_psw);
  // Serial.println(inserted_psw);
  // Serial.println(current_X+PSW_SIZE);
  
  if (y > JS_RANGE_SUP && insert == LOW){ // per passare alla cella successiva del display

    insert = HIGH; // indico che è stato inserito un input

    // aggiorno la posizione di +1 a destra    
    current_X += 1;
    lcd.setCursor(current_X, cursor_Y);

    last_action = HIGH; // indico che l'ultima azione è stata uno spostamento
    // Serial.println("destra");

  } else if (x < JS_RANGE_INF && insert == LOW && current_X != enter_position){ // per selezionare il numero precedente

    insert = HIGH; // indico che è stato inserito un input

    // faccio visualizzare il numero precedente
    current_index -= 1;
    lcd.write(psw_chars[current_index]);
    lcd.setCursor(current_X, cursor_Y); // per non andare avanti con il cursore
    
    last_action = LOW; // indico che l'ultima azione NON è stata uno spostamento
    // Serial.println("giu");

  } else if (x > JS_RANGE_SUP && insert == LOW && current_X != enter_position){ // per selezionare un numero successivo

    insert = HIGH; // indico che è stato inserito un input

    // faccio visualizzare il numero successivo
    current_index += 1;
    lcd.write(psw_chars[current_index]);
    lcd.setCursor(current_X, cursor_Y); // per non andare avanti con il cursore

    last_action = LOW; // indico che l'ultima azione NON è stata uno spostamento
    // Serial.println("su");
    
  }
  
  writePsw(); // per salvare nella stringa inserted_psw la password che si sta inserendo  
  
  if (button == 1 && insert == LOW && current_X == enter_position){ // per dire di aver inserito la password

    insert = HIGH; // indico che è stato inserito un input

    lcd.setCursor(cursor_X, cursor_Y); // reset della posizione del cursore

    checkPsw(); // verifica che la password inserita sia giusta

    lcd.noCursor(); // non faccio visualizzare il cursore
    // Serial.println("pulsante");
  }

  inputOnClick(x, y); // per indicare che ho preso l'input e da qui in poi può prenderne altri   
  
  continuosArray(); // per iterare sempre i valori sull'array

  frameRange(); // per non far oltrepassare un custom frame nel display
}


// METODI CUSTOM:

// metodo per il setup del front end
void setupFrontEnd(void){

  // setup della schermata per inserire la password
  lcd.setCursor(4, 0);
  lcd.print("Password");
  lcd.setCursor(6, 1);
  lcd.print("0000");
  lcd.setCursor(cursor_X+PSW_SIZE, 1);
  lcd.write(byte(0));

  // setup per iniziare a scrivere
  lcd.setCursor(current_X, cursor_Y);
  
  delay(1000); // delay di 2 sec
  
  lcd.cursor();
}

// metodo per salvare nella stringa inserted_psw la password che si sta inserendo
void writePsw(void){
  if (last_action == HIGH) {
    inserted_psw[current_X-cursor_X-1] = psw_chars[current_index];
  }
}

// metodo che verifica che la password inserita sia giusta
void checkPsw(void){
  if (inserted_psw == right_psw){
    lcd.print("RIGHT"); // password giusta
  } else {
    lcd.print("WRONG"); // password sbagliata
    delay(1000);
    restartFunc(); // restart del codice
  }
}

// metodo per indicare che ho preso l'input e da qui in poi può prenderne altri
void inputOnClick(int x, int y){
  if (x > JS_RANGE_INF && x < JS_RANGE_SUP && y > JS_RANGE_INF && y < JS_RANGE_SUP){
    insert = LOW;
  }
}

// metodo per iterare sempre i valori sull'array
void continuosArray(){
  if (current_index > psw_chars_size){ // check della fine dell'array
    current_index = 0;
  } else if (current_index < 0){ // check dell'inizio dell'array
    current_index = psw_chars_size;
  }
}

// metodo per non far oltrepassare un custom frame nel display
void frameRange(void){
  if (current_X >= cursor_X+PSW_SIZE){
    lcd.setCursor(cursor_X+PSW_SIZE, cursor_Y);
    current_X = enter_position;
  }
}