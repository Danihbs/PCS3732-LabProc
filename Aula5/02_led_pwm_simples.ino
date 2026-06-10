#define LED_PIN 4

const int freq = 5000;
const int resolucao = 8;

void setup() {
  ledcAttach(LED_PIN, freq, resolucao);
}

void loop() {
  for (int brilho = 0; brilho <= 255; brilho++) {
    ledcWrite(LED_PIN, brilho);
    delay(10);
  }

  for (int brilho = 255; brilho >= 0; brilho--) {
    ledcWrite(LED_PIN, brilho);
    delay(10);
  }
}
