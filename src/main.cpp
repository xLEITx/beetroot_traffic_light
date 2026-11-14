#include <Arduino.h>

constexpr uint8_t RED_LED = 16;
constexpr uint8_t YELLOW_LED = 17;
constexpr uint8_t GREEN_LED = 18;
constexpr uint8_t BUTTON_PIN = 2;


void setup() {
  Serial.begin(115200);
  delay(50);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

}

void loop() {

}
