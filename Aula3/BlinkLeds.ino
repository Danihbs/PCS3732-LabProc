const int led0 = 0;
const int led1 = 1;
const int led2 = 2;
const int led3 = 3;

void setup() {
    pinMode(led0, OUTPUT);
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
    pinMode(led3, OUTPUT);
}

void loop() {
    digitalWrite(led0, HIGH);
    delay(500);
    digitalWrite(led0, LOW);
    delay(500);
    digitalWrite(led1, HIGH);
    delay(500);
    digitalWrite(led1, LOW);
    delay(500);
    digitalWrite(led2, HIGH);
    delay(500);
    digitalWrite(led2, LOW);
    delay(500);
    digitalWrite(led3, HIGH);
    delay(500);
    digitalWrite(led3, LOW);
    delay(500);
}