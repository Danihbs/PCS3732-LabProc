#include <WiFi.h>
#include <WebServer.h>

#define LED_PIN 4

const char* ssid = "ESP32_PWM_LED";
const char* password = "12345678";

WebServer server(80);

int brilho = 128;
int frequencia = 5000;
const int resolucao = 8;

void atualizarPWM() {
  ledcDetach(LED_PIN);
  ledcAttach(LED_PIN, frequencia, resolucao);
  ledcWrite(LED_PIN, brilho);
}

String paginaHTML() {
  String html = "";

  html += "<!DOCTYPE html><html>";
  html += "<head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>Controle LED PWM</title></head>";
  html += "<body>";
  html += "<h2>Controle de LED com PWM</h2>";

  html += "<form action='/set' method='GET'>";
  html += "Brilho: <input type='range' name='brilho' min='0' max='255' value='" + String(brilho) + "'><br><br>";
  html += "Frequencia: <input type='number' name='freq' value='" + String(frequencia) + "'><br><br>";
  html += "<input type='submit' value='Atualizar'>";
  html += "</form>";

  html += "<p>Brilho atual: " + String(brilho) + "</p>";
  html += "<p>Frequencia atual: " + String(frequencia) + " Hz</p>";

  html += "</body></html>";

  return html;
}

void handleRoot() {
  server.send(200, "text/html", paginaHTML());
}

void handleSet() {
  if (server.hasArg("brilho")) {
    brilho = server.arg("brilho").toInt();
  }

  if (server.hasArg("freq")) {
    frequencia = server.arg("freq").toInt();
  }

  if (frequencia < 100) {
    frequencia = 100;
  }

  if (frequencia > 20000) {
    frequencia = 20000;
  }

  atualizarPWM();

  server.send(200, "text/html", paginaHTML());
}

void setup() {
  Serial.begin(115200);

  ledcAttach(LED_PIN, frequencia, resolucao);
  ledcWrite(LED_PIN, brilho);

  WiFi.softAP(ssid, password);

  Serial.println("Rede criada:");
  Serial.println(ssid);
  Serial.println("IP do ESP32:");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/set", handleSet);

  server.begin();
}

void loop() {
  server.handleClient();
}
