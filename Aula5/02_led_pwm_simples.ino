#include <ESP32Servo.h>

Servo meuServo;

const int pinoServo = 4;

void setup() {
  meuServo.setPeriodHertz(50);
  meuServo.attach(pinoServo, 500, 2400);
}

void loop() {
  meuServo.write(90);
  delay(1000);
  meuServo.write(0);
  delay(1000);
}