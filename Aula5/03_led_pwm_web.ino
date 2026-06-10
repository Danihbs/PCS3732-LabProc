#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const int pinoServo = 4;
const int freqServo = 50;
const char* ssid = "ESP32_SERVO_do_Tulio";
const char* password = "12345meia";

Servo meuServo;
WebServer server(80);

int angulo = 90;

void moverServo(int novoAngulo) {
  if (novoAngulo < 0) {
    novoAngulo = 0;
  }

  if (novoAngulo > 180) {
    novoAngulo = 180;
  }

  angulo = novoAngulo;

  meuServo.write(angulo);
}

String paginaHTML() {
  String html = "";

  html += "<!DOCTYPE html><html>";
  html += "<head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>Controle Servo</title></head>";
  html += "<body>";
  html += "<h2>Controle de Servo com PWM</h2>";

  html += "<form action='/set' method='GET'>";
  html += "Angulo: <input type='range' name='angulo' min='0' max='180' value='" + String(angulo) + "'><br><br>";
  html += "<input type='submit' value='Mover'>";
  html += "</form>";

  html += "<p>Angulo atual: " + String(angulo) + " graus</p>";

  html += "</body></html>";

  return html;
}

void handleRoot() {
  server.send(200, "text/html", paginaHTML());
}

void handleSet() {
  if (server.hasArg("angulo")) {
    moverServo(server.arg("angulo").toInt());
  }

  server.send(200, "text/html", paginaHTML());
}

void setup() {
  Serial.begin(115200);

  meuServo.setPeriodHertz(freqServo);
  meuServo.attach(pinoServo, 500, 2400);
  meuServo.write(0);

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