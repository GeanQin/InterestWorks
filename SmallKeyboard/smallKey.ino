#include <Keypad.h>
#include <Keyboard.h>

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
{KEY_INSERT,KEY_HOME,KEY_PAGE_UP},
{KEY_DELETE,KEY_END,KEY_PAGE_DOWN},
{' ',KEY_UP_ARROW,' '},
{KEY_LEFT_ARROW,KEY_DOWN_ARROW,KEY_RIGHT_ARROW},
};
byte rowPins[ROWS] = {3,4,5,6}; //connect to the row pinouts of the kpd
byte colPins[COLS] = {7,8,9}; //connect to the column pinouts of the kpd

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

unsigned long loopCount;
unsigned long startTime;
String msg;
char buffer[5];
int numdata=0;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  while(Serial1.read()>= 0){}//clear serial port
  loopCount = 0;
  startTime = millis();
  msg = "";
}


void loop() {
  if(Serial1.available()>0){
    delay(100);
    numdata = Serial1.readBytes(buffer,5);
    Serial.print("Serial.readBytes:");
    Serial.println(buffer);
    if(buffer[1]==';'){
      Keyboard.press(buffer[0]);
      Keyboard.release(buffer[0]);
    }else{
      if(buffer[0]=='y'&&buffer[1]=='s'&&buffer[2]=='x'&&buffer[3]=='w'){
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('t');
        Keyboard.release('t');
        Keyboard.press('t');
        Keyboard.release('t');
        Keyboard.release(KEY_LEFT_GUI);
        Keyboard.press(KEY_RETURN);
        Keyboard.release(KEY_RETURN);
        delay(1000);
        Keyboard.print("tv.cctv.com/live/cctv13");
        delay(1000);
        Keyboard.press(KEY_RETURN);
        Keyboard.release(KEY_RETURN);
        Keyboard.press(KEY_RETURN);
        Keyboard.release(KEY_RETURN);
      }
    }
  }
  // clear serial buffer
  while(Serial1.read() >= 0){}
  for(int i=0; i<5; i++){
    buffer[i]='\0';
  }
  
  loopCount++;
  if ( (millis()-startTime)>5000 ) {
    Serial.print("Average loops per second = ");
    Serial.println(loopCount/5);
    startTime = millis();
    loopCount = 0;
  }

  // Fills kpd.key[ ] array with up-to 10 active keys.
  // Returns true if there are ANY active keys.
  if (kpd.getKeys())
  {
    for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
    {
      if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
      {
        switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
          case PRESSED:
            msg = " PRESSED.";
            Keyboard.press(kpd.key[i].kchar);
            break;
          case HOLD:
            msg = " HOLD.";
            break;
          case RELEASED:
            Keyboard.release(kpd.key[i].kchar);
            msg = " RELEASED.";
            break;
          case IDLE:
            msg = " IDLE.";
        }
        Serial.print("Key ");
        Serial.print(kpd.key[i].kchar);
        
        Serial.println(msg);
      }
    }
  }
}  // End loop
