#include <Arduino.h>

constexpr uint8_t RED_LED = 16;
constexpr uint8_t YELLOW_LED = 17;
constexpr uint8_t GREEN_LED = 18;
constexpr uint8_t BUTTON_PIN = 2;

hw_timer_t * timer = NULL;
volatile uint32_t timer_counter = 0;

enum class TrafficState: uint8_t {
  GREEN,
  GREEN_BLINKING,
  YELLOW,
  RED,
  RED_YELLOW,
  YELLOW_BLINKING
};

struct TIMERS_S
{
  uint8_t GREEN = 10;
  uint8_t GREEN_BLINKING = 3;
  uint8_t YELLOW = 3;
  uint8_t RED = 10;
  uint8_t RED_YELLOW = 2;
};

constexpr uint32_t GENERAL_BLINKING_MS = 500;

TrafficState currentState = TrafficState::GREEN;
TrafficState stateBeforeInterrupt = TrafficState::GREEN;
uint32_t currentStateBegin = 0;
uint32_t lastBlinkMs = 0;

volatile bool isButtonPressed = false;
volatile uint32_t lastButtonPressTime = 0;

void IRAM_ATTR onButton(){
 isButtonPressed = true;
}

void IRAM_ATTR onTimer(){
  timer_counter++;
}

void handleTrafficLight(TrafficState state){
  uint32_t currentMillis = millis();
  switch (state)
  {
  case TrafficState::GREEN:
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    break;
  
  case TrafficState::GREEN_BLINKING:
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    if(currentMillis-lastBlinkMs >= GENERAL_BLINKING_MS){
      digitalWrite(GREEN_LED, !digitalRead(GREEN_LED));
      lastBlinkMs = millis();
    }
    break;
  case TrafficState::RED:
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    break;

  case TrafficState::RED_YELLOW:
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    break;

  case TrafficState::YELLOW:
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    break;
  
  case TrafficState ::YELLOW_BLINKING:
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    if(currentMillis-lastBlinkMs >= GENERAL_BLINKING_MS){
      digitalWrite(YELLOW_LED, !digitalRead(YELLOW_LED));
      lastBlinkMs = millis();
    }
    break;
  
  default:
    break;
  }

}

void setup() {
  Serial.begin(115200);
  delay(50);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);

  attachInterrupt(BUTTON_PIN, onButton, FALLING);

}

void loop() {
  if (isButtonPressed)
  {
    uint32_t currentMillis = millis();
    if (currentMillis-lastButtonPressTime > 50)
    {
      lastButtonPressTime = currentMillis;
      if (currentState == TrafficState::YELLOW_BLINKING)
      {
        currentState = stateBeforeInterrupt;
        
      }else{
        stateBeforeInterrupt = currentState;
        currentState = TrafficState::YELLOW_BLINKING;
      }
    }
    
    isButtonPressed = false;    
  }
  handleTrafficLight(currentState);

}

