#include <Arduino.h>

constexpr uint8_t RED_PIN = 6;
constexpr uint8_t YELLOW_PIN = 15;
constexpr uint8_t GREEN_PIN = 16;
constexpr uint8_t BUTTON_PIN = 4;

struct
{ 
  uint32_t GREEN = 10000;         
  uint32_t GREEN_BLINKING = 3000;
  uint32_t YELLOW = 2000;
  uint32_t RED = 10000;
  uint32_t RED_YELLOW = 2000;
} ModeDuration;

enum State{
  GREEN,
  GREEN_BLINKING,
  YELLOW,
  RED,
  RED_YELLOW
};

constexpr uint32_t BLINK_INTERVAL = 500; 
uint32_t currentStateBegin = 0;
uint32_t lastBlinkTime = 0;
bool blinkState = false;

volatile bool isYellowBlinkingMode = false; 
volatile uint32_t lastDebounceTime = 0;
constexpr uint32_t DEBOUNCE_DELAY = 200; 

State state = GREEN;

void IRAM_ATTR onTimer(){

}

void IRAM_ATTR handleButtonPress() {
  uint32_t current_time = millis();
  
  if (current_time - lastDebounceTime > DEBOUNCE_DELAY) {
    lastDebounceTime = current_time;
    isYellowBlinkingMode = !isYellowBlinkingMode; 
  }
}

void setup() {
  Serial.begin(115200);
  delay(50);
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, FALLING);
  
  currentStateBegin = millis();
}

void loop() {
  uint32_t current_time = millis();

  if (isYellowBlinkingMode)
  {
    if (current_time - lastBlinkTime >= BLINK_INTERVAL)
    {
      lastBlinkTime = current_time;
      blinkState = !blinkState;
      digitalWrite(YELLOW_PIN, blinkState ? HIGH : LOW);
    }
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(RED_PIN, LOW);
    
    currentStateBegin = current_time;
    return; 
  }

  State previousState = state;

  if (state == GREEN && (current_time - currentStateBegin > ModeDuration.GREEN))
  {
    state = GREEN_BLINKING;
  }
  else if (state == GREEN_BLINKING && (current_time - currentStateBegin > ModeDuration.GREEN_BLINKING))
  {
    state = YELLOW;
  }
  else if (state == YELLOW && (current_time - currentStateBegin > ModeDuration.YELLOW))
  {
    state = RED;
  }
  else if (state == RED && (current_time - currentStateBegin > ModeDuration.RED))
  {
    state = RED_YELLOW;
  }
  else if (state == RED_YELLOW && (current_time - currentStateBegin > ModeDuration.RED_YELLOW))
  {
    state = GREEN;
  }

  if (state != previousState)
  {
    currentStateBegin = current_time;
    lastBlinkTime = current_time;
    blinkState = false;
  }

  switch (state)
  {
    case GREEN:
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(RED_PIN, LOW);
      break;

    case GREEN_BLINKING:
      if (current_time - lastBlinkTime >= BLINK_INTERVAL)
      {
        lastBlinkTime = current_time;
        blinkState = !blinkState;
        digitalWrite(GREEN_PIN, blinkState ? HIGH : LOW);
      }
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(RED_PIN, LOW);
      break;

    case YELLOW:
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(YELLOW_PIN, HIGH);
      digitalWrite(RED_PIN, LOW);
      break;

    case RED:
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(RED_PIN, HIGH);
      break;

    case RED_YELLOW:
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(YELLOW_PIN, HIGH);
      digitalWrite(RED_PIN, HIGH);
      break;
  }
}
