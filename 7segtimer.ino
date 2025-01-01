#include <TM1637Display.h>
#include <EncoderButton.h>
#include <stdint.h>

#define SWITCH 23
#define KNOB_CLK 25
#define KNOB_DT 22
#define KNOB_5V 24
#define BUZZER 48

#define CLK 52 // Pin for Clock Input
#define DIO 53 // Pin for Data Input/Output

#define SHOW_COLON 0xFF // 00:00

#define MIN_TO_MS(x) (long)((long)x * 60 * 1000)

static const uint8_t TXT_OFF[]  = {
  0b00000000,
  0b00111111,
  0b01110001,
  0b01110001
};

typedef enum
{
  HOURS_MINUTES, // HH:MM
  MINUTES_SECONDS // MM:SS
} DisplayMode;
DisplayMode displayMode = HOURS_MINUTES;

TM1637Display display = TM1637Display(CLK, DIO); // Creates display object that represents the TM1637 display
EncoderButton knob(KNOB_CLK, KNOB_DT, SWITCH);

long timeLeft = MIN_TO_MS(3);
long startTimestamp = 0;
bool silenceAlarm = false;
int lastDisplayNum = 0;

// Create one or more callback functions 
void onKnobTurn(EncoderButton& eb) {
  // Serial.print("eb1 incremented by: ");
  // Serial.println(eb.increment());
  // Serial.print("eb1 position is: ");
  // Serial.println(eb.position());
  //setDisplayNum(eb.position());
  
  //setDisplayNum(displayNum + eb.increment()*abs(eb.increment())*abs(eb.increment())*abs(eb.increment()));
  if (timeLeft > 0) {
    silenceAlarm = false;
  }
  long timeToAdjustByMs = MIN_TO_MS(1);
  if (timeLeft > MIN_TO_MS(10)) {
    timeToAdjustByMs = MIN_TO_MS(5);
  } else if (timeLeft < MIN_TO_MS(1)) {
    timeToAdjustByMs = 10 * 1000;
  }
  timeLeft -= timeToAdjustByMs * eb.increment()*abs(eb.increment())*abs(eb.increment())*abs(eb.increment());
  
}

void onKnobClick(EncoderButton& eb) {
  // switch(progState)
  // {
  //   case COUNTING:
  //     //progState = IDLE;
  //     break;
  //   case IDLE:
  //     // timerStartTimestampMs = millis();
  //     progState = COUNTING;
  //     break;
  //   case ALARM:
  //     progState = IDLE;
  //     break;
  // }
  if (timeLeft <= 0) {
    silenceAlarm = true;
  }
}


void setup() {
  pinMode(BUZZER, OUTPUT);
  pinMode(KNOB_5V, OUTPUT);
  digitalWrite(KNOB_5V, HIGH);
  display.clear(); // clears the display
  display.setBrightness(7); // parameter can range from 0 (lowes brightness) to 7 (highest brightness)
  knob.setEncoderHandler(onKnobTurn);
  knob.setClickHandler(onKnobClick);
}


void loop() {
    knob.update(); // has to be called VERY VERY frequently :(
    
    int timeElapsed = millis() - startTimestamp;
    timeLeft -= timeElapsed;
    startTimestamp = millis();
    if (timeLeft <= 0) {
      timeLeft = 0;
    }
    long hours = timeLeft / (long)3600000;
    long minutes = (timeLeft % (long)3600000) / (long)60000; // 0 to 60
    long seconds = (timeLeft % (long)60000) / (long)1000; // 0 to 60
    if (hours < 1) {
      displayMode = MINUTES_SECONDS;
    } else {
      displayMode = HOURS_MINUTES;
    }
    int displayNum = 9999;
    switch (displayMode) {
      case HOURS_MINUTES:
        displayNum = (hours * 100) + minutes;
        break;
      case MINUTES_SECONDS:
        displayNum = (minutes * 100) + seconds;
        break;
    }
    bool doAlarm = false;
    if (timeLeft == 0) {
      doAlarm = true;
    }
    if (doAlarm) {
      if (silenceAlarm) {
        //display.showNumberHexEx(0x00FF);
        display.setSegments(TXT_OFF);
        noTone(BUZZER);
      } else {
        display.showNumberHexEx(0xFFFF);
        //tone(BUZZER, 277);
        tone(BUZZER, (int)(cos(millis()/(float)100) * 150 + 500));
      }
    }
    else {
      if (lastDisplayNum != displayNum) { // only send new data to display when necessary. mostly because knob.update needs to be called VERY VERY frequently :( But that also means i can make it boop boop beep when the timer is about to run out.
        display.showNumberDecEx(displayNum, SHOW_COLON);
        if (timeLeft < 1 * 1000 ) {
          tone(BUZZER, 1200, 500);
        } else if (timeLeft < 10 * 1000 ) {
          tone(BUZZER, 300, 50);
        } else if (timeLeft < 30 * 1000 ) {
          tone(BUZZER, 900, 10);
        } else {
          noTone(BUZZER);
        }
      }
    }
    lastDisplayNum = displayNum;
}
