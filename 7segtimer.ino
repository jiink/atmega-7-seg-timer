// No effort was made to make this code nice. It is very hard to follow along with.

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

/* Animation Data - HGFEDCBA Map */
#define NUM_DANCE_FRAMES 64
const uint8_t danceFrames[NUM_DANCE_FRAMES][4] = {
  { 0x10, 0x00, 0x00, 0x00 },  // Frame 0
  { 0x30, 0x00, 0x00, 0x00 },  // Frame 1
  { 0x20, 0x00, 0x00, 0x00 },  // Frame 2
  { 0x21, 0x00, 0x00, 0x00 },  // Frame 3
  { 0x01, 0x00, 0x00, 0x00 },  // Frame 4
  { 0x03, 0x00, 0x00, 0x00 },  // Frame 5
  { 0x02, 0x00, 0x00, 0x00 },  // Frame 6
  { 0x42, 0x00, 0x00, 0x00 },  // Frame 7
  { 0x40, 0x00, 0x00, 0x00 },  // Frame 8
  { 0x50, 0x00, 0x00, 0x00 },  // Frame 9
  { 0x10, 0x00, 0x00, 0x00 },  // Frame 10
  { 0x18, 0x00, 0x00, 0x00 },  // Frame 11
  { 0x08, 0x00, 0x00, 0x00 },  // Frame 12
  { 0x08, 0x08, 0x00, 0x00 },  // Frame 13
  { 0x00, 0x08, 0x00, 0x00 },  // Frame 14
  { 0x00, 0x0c, 0x00, 0x00 },  // Frame 15
  { 0x00, 0x04, 0x00, 0x00 },  // Frame 16
  { 0x00, 0x06, 0x00, 0x00 },  // Frame 17
  { 0x00, 0x02, 0x00, 0x00 },  // Frame 18
  { 0x00, 0x03, 0x00, 0x00 },  // Frame 19
  { 0x00, 0x01, 0x00, 0x00 },  // Frame 20
  { 0x00, 0x21, 0x00, 0x00 },  // Frame 21
  { 0x00, 0x20, 0x00, 0x00 },  // Frame 22
  { 0x00, 0x60, 0x00, 0x00 },  // Frame 23
  { 0x00, 0x40, 0x00, 0x00 },  // Frame 24
  { 0x00, 0x40, 0x40, 0x00 },  // Frame 25
  { 0x00, 0x00, 0x40, 0x00 },  // Frame 26
  { 0x00, 0x00, 0x42, 0x00 },  // Frame 27
  { 0x00, 0x00, 0x02, 0x00 },  // Frame 28
  { 0x00, 0x00, 0x03, 0x00 },  // Frame 29
  { 0x00, 0x00, 0x01, 0x00 },  // Frame 30
  { 0x00, 0x00, 0x21, 0x00 },  // Frame 31
  { 0x00, 0x00, 0x20, 0x00 },  // Frame 32
  { 0x00, 0x00, 0x30, 0x00 },  // Frame 33
  { 0x00, 0x00, 0x10, 0x00 },  // Frame 34
  { 0x00, 0x00, 0x18, 0x00 },  // Frame 35
  { 0x00, 0x00, 0x08, 0x00 },  // Frame 36
  { 0x00, 0x00, 0x08, 0x08 },  // Frame 37
  { 0x00, 0x00, 0x00, 0x08 },  // Frame 38
  { 0x00, 0x00, 0x00, 0x0c },  // Frame 39
  { 0x00, 0x00, 0x00, 0x04 },  // Frame 40
  { 0x00, 0x00, 0x00, 0x06 },  // Frame 41
  { 0x00, 0x00, 0x00, 0x02 },  // Frame 42
  { 0x00, 0x00, 0x00, 0x03 },  // Frame 43
  { 0x00, 0x00, 0x00, 0x01 },  // Frame 44
  { 0x00, 0x00, 0x00, 0x21 },  // Frame 45
  { 0x00, 0x00, 0x00, 0x20 },  // Frame 46
  { 0x00, 0x00, 0x40, 0x20 },  // Frame 47
  { 0x00, 0x00, 0x40, 0x00 },  // Frame 48
  { 0x00, 0x40, 0x40, 0x00 },  // Frame 49
  { 0x00, 0x40, 0x00, 0x00 },  // Frame 50
  { 0x00, 0x60, 0x00, 0x00 },  // Frame 51
  { 0x00, 0x20, 0x00, 0x00 },  // Frame 52
  { 0x00, 0x21, 0x00, 0x00 },  // Frame 53
  { 0x00, 0x01, 0x00, 0x00 },  // Frame 54
  { 0x00, 0x03, 0x00, 0x00 },  // Frame 55
  { 0x00, 0x02, 0x00, 0x00 },  // Frame 56
  { 0x00, 0x06, 0x00, 0x00 },  // Frame 57
  { 0x00, 0x04, 0x00, 0x00 },  // Frame 58
  { 0x00, 0x0c, 0x00, 0x00 },  // Frame 59
  { 0x00, 0x08, 0x00, 0x00 },  // Frame 60
  { 0x08, 0x08, 0x00, 0x00 },  // Frame 61
  { 0x08, 0x00, 0x00, 0x00 },  // Frame 62
  { 0x18, 0x00, 0x00, 0x00 }   // Frame 63
};
uint8_t currentDanceFrame = 0;
unsigned long danceAnimationTimer = 0;

// https://jasonacox.github.io/TM1637TinyDisplay/examples/7-segment-animator.html
#define NUM_ALARM_FRAMES 3
static const uint8_t alarmFrames[NUM_ALARM_FRAMES][4] = {
  { 0x00, 0x00, 0x00, 0x00 },  // Frame 0
  { 0x31, 0x01, 0x01, 0x07 },  // Frame 1
  { 0x38, 0x08, 0x08, 0x0e },  // Frame 2
};
uint8_t currentAlarmFrame = 0;
unsigned long animationTimer = 0;

typedef enum
{
  HOURS_MINUTES, // HH:MM
  MINUTES_SECONDS // MM:SS
} DisplayMode;
DisplayMode displayMode = HOURS_MINUTES;

TM1637Display display = TM1637Display(CLK, DIO); // Creates display object that represents the TM1637 display
EncoderButton knob(KNOB_CLK, KNOB_DT, SWITCH);

long timeLeft = 0;
long startTimestamp = 0;
bool silenceAlarm = true;
int lastDisplayNum = 0;
int toneOffset = 0;
unsigned long toneOffsetTimer = 0;
const unsigned long maxAlarmLength = MIN_TO_MS(2);
unsigned long alarmStartedTimestamp = 0; // shut off alarm automatically after maxAlarmLength ms
bool doAlarm = false;

// Create one or more callback functions 
void onKnobTurn(EncoderButton& eb) {
  // Serial.print("eb1 incremented by: ");
  // Serial.println(eb.increment());
  // Serial.print("eb1 position is: ");
  // Serial.println(eb.position());
  //setDisplayNum(eb.position());
  
  //setDisplayNum(displayNum + eb.increment()*abs(eb.increment())*abs(eb.increment())*abs(eb.increment()));
  if (timeLeft > 5000) { // prevent alarming if knob was accidentally turned just a notch or two
    silenceAlarm = false;
  }
  long timeToAdjustByMs = MIN_TO_MS(1);
  if (timeLeft > MIN_TO_MS(10)) {
    timeToAdjustByMs = MIN_TO_MS(5);
  } else if (timeLeft < 5 * 1000) {
    timeToAdjustByMs = 1000;
  } else if (timeLeft < MIN_TO_MS(1)) {
    timeToAdjustByMs = 10 * 1000;
  }
  timeLeft -= timeToAdjustByMs * eb.increment()*abs(eb.increment())*abs(eb.increment())*abs(eb.increment());
  
}

void onKnobClick(EncoderButton& eb) {
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
    if (timeLeft == 0) {
      bool wasAlreadyDoingAlarm = (doAlarm == true);
      doAlarm = true;
      if (!wasAlreadyDoingAlarm)
      {
        toneOffset = 0;
        alarmStartedTimestamp = millis();
      }
    } else { doAlarm = false; }
    if (doAlarm) {
      if (silenceAlarm) {
        noTone(BUZZER);
        if (millis() - danceAnimationTimer > 25)
        {
          danceAnimationTimer = millis();
          display.setSegments(danceFrames[++currentDanceFrame % NUM_DANCE_FRAMES]);
        }
      } else {
        if ((millis() / 50) % 2 == 0) {
          tone(BUZZER, toneOffset + 80); 
        } else {
          tone(BUZZER, toneOffset + 80 + 96); 
        }
        if (millis() - toneOffsetTimer >= 5000) {
          toneOffsetTimer = millis();
          toneOffset += 400;
          if (toneOffset > 1600) { 
            toneOffset = 0;
          }
        }
        if (millis() - animationTimer > 50)
        {
          animationTimer = millis();
          // Also animate the display
          currentAlarmFrame++;
          display.setSegments(alarmFrames[currentAlarmFrame % NUM_ALARM_FRAMES]);
        }
        if (millis() - alarmStartedTimestamp > maxAlarmLength)
        {
          silenceAlarm = true;
        }
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
