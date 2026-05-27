#include <WiFi.h>
#include <WebServer.h>

// ======================
// CONFIGURAÇÃO DO WI-FI
// ======================

const char* ssid = "Calculadora_ESP32_do_Caio";
const char* password = "12345678";   // mínimo de 8 caracteres

WebServer server(80);

// ======================
// CONFIGURAÇÃO DOS LEDS
// ======================

const int ledPins[4] = {4, 5, 6, 7}; // bit0, bit1, bit2, bit3

const int LED_ON = HIGH;
const int LED_OFF = LOW;

// Último resultado calculado, para mostrar na página.
String ultimoResultado = "Nenhum cálculo realizado ainda.";

// ======================
// HTML + CSS + JAVASCRIPT
// ======================

const char paginaHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Calculadora Binária ESP32</title>
  <style>
    body {
      font-family: Arial, Helvetica, sans-serif;
      background: #f3f3f3;
      margin: 0;
      padding: 24px;
      color: #222;
    }

    .card {
      max-width: 520px;
      margin: 0 auto;
      background: #fff;
      padding: 22px;
      border-radius: 12px;
      box-shadow: 0 2px 12px rgba(0,0,0,0.16);
    }

    h1 {
      font-size: 24px;
      margin: 0 0 8px;
      text-align: center;
    }

    .subtitulo {
      text-align: center;
      font-size: 14px;
      margin-bottom: 22px;
      color: #555;
    }

    label {
      display: block;
      margin-top: 14px;
      margin-bottom: 6px;
      font-weight: bold;
    }

    input {
      width: 100%;
      box-sizing: border-box;
      padding: 12px;
      font-size: 22px;
      text-align: center;
      letter-spacing: 4px;
      border: 1px solid #bbb;
      border-radius: 8px;
    }

    .botoes {
      display: flex;
      gap: 12px;
      margin-top: 18px;
    }

    button {
      flex: 1;
      padding: 13px;
      font-size: 16px;
      border: 0;
      border-radius: 8px;
      cursor: pointer;
      color: white;
      background: #2c6e49;
      font-weight: bold;
    }

    button.sub {
      background: #3a506b;
    }

    .resultado {
      margin-top: 20px;
      padding: 14px;
      border-radius: 8px;
      background: #eeeeee;
      white-space: pre-line;
      line-height: 1.45;
      font-size: 15px;
    }

    .ok {
      border-left: 6px solid #2c6e49;
    }

    .overflow {
      border-left: 6px solid #c1121f;
      background: #fff0f0;
    }

    .erro {
      border-left: 6px solid #f77f00;
      background: #fff8ea;
    }

    .nota {
      margin-top: 18px;
      font-size: 13px;
      color: #555;
      line-height: 1.4;
    }

    code {
      background: #eee;
      padding: 2px 4px;
      border-radius: 4px;
    }
  </style>
</head>

<body>
  <div class="card">
    <h1>Calculadora Binária de 4 Bits</h1>
    <div class="subtitulo">Soma e subtração em complemento de dois — cálculo feito na ESP32</div>

    <label for="a">Operando A:</label>
    <input id="a" maxlength="4" value="0011" inputmode="numeric">

    <label for="b">Operando B:</label>
    <input id="b" maxlength="4" value="0010" inputmode="numeric">

    <div class="botoes">
      <button onclick="calcular('add')">SOMA</button>
      <button class="sub" onclick="calcular('sub')">SUBTRAÇÃO</button>
    </div>

    <div id="resultado" class="resultado">Aguardando operação...</div>

    <div class="nota">
      O JavaScript apenas valida a entrada e envia a requisição HTTP para a rota
      <code>/calc</code>. A soma, a subtração, a conversão em complemento de dois,
      a verificação de overflow e o acionamento dos LEDs são feitos no firmware C/C++ da ESP32.
    </div>
  </div>

  <script>
    function entradaValida(valor) {
      return /^[01]{4}$/.test(valor);
    }

    function normalizarEntrada(campo) {
      campo.value = campo.value.replace(/[^01]/g, "").slice(0, 4);
    }

    document.getElementById("a").addEventListener("input", function() {
      normalizarEntrada(this);
    });

    document.getElementById("b").addEventListener("input", function() {
      normalizarEntrada(this);
    });

    function calcular(op) {
      const a = document.getElementById("a").value.trim();
      const b = document.getElementById("b").value.trim();
      const resultado = document.getElementById("resultado");

      if (!entradaValida(a) || !entradaValida(b)) {
        resultado.className = "resultado erro";
        resultado.innerText = "Erro: informe exatamente 4 bits em A e B.\nExemplo: A = 0011, B = 0010.";
        return;
      }

      resultado.className = "resultado";
      resultado.innerText = "Enviando requisição para a ESP32...";

      const url = `/calc?a=${encodeURIComponent(a)}&b=${encodeURIComponent(b)}&op=${encodeURIComponent(op)}`;

      fetch(url)
        .then(res => res.json())
        .then(data => {
          if (!data.ok) {
            resultado.className = "resultado erro";
            resultado.innerText = data.message;
            return;
          }

          resultado.className = data.overflow ? "resultado overflow" : "resultado ok";

          resultado.innerText =
            `Operação: ${data.operation}\n` +
            `A: ${data.aBin} (${data.aDec})\n` +
            `B: ${data.bBin} (${data.bDec})\n` +
            `Resultado nos LEDs: ${data.resultBin} (${data.resultDec})\n` +
            `Status: ${data.status}`;
        })
        .catch(() => {
          resultado.className = "resultado erro";
          resultado.innerText = "Erro de comunicação com a ESP32.";
        });
    }
  </script>
</body>
</html>
)rawliteral";

// ======================
// FUNÇÕES AUXILIARES
// ======================

bool isBinary4(String value) {
  if (value.length() != 4) {
    return false;
  }

  for (int i = 0; i < 4; i++) {
    if (value[i] != '0' && value[i] != '1') {
      return false;
    }
  }

  return true;
}

// Converte uma string de 4 bits em inteiro com complemento de dois.

int bin4ToSignedInt(String bin) {
  int unsignedValue = 0;

  for (int i = 0; i < 4; i++) {
    unsignedValue = (unsignedValue << 1) | (bin[i] - '0');
  }

  if (unsignedValue >= 8) {
    return unsignedValue - 16;
  }

  return unsignedValue;
}

// Converte os 4 bits menos significativos para string binária.

String toBinary4(int value) {
  uint8_t bits = value & 0x0F;
  String out = "";

  for (int i = 3; i >= 0; i--) {
    out += ((bits >> i) & 1) ? '1' : '0';
  }

  return out;
}

// Aciona os 4 LEDs conforme os 4 bits menos significativos.
// ledPins[0] mostra o bit 0, ou seja, o bit menos significativo.
void writeLeds4Bits(int value) {
  uint8_t bits = value & 0x0F;

  for (int i = 0; i < 4; i++) {
    bool bitLigado = (bits >> i) & 1;
    digitalWrite(ledPins[i], bitLigado ? LED_ON : LED_OFF);
  }
}

// Apaga todos os LEDs.
void clearLeds() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], LED_OFF);
  }
}

// Monta resposta JSON sem biblioteca externa.
String buildJsonResponse(
  String aBin,
  String bBin,
  String operation,
  int aDec,
  int bDec,
  int resultDec,
  String resultBin,
  bool overflow
) {
  String json = "{";
  json += "\"ok\":true,";
  json += "\"aBin\":\"" + aBin + "\",";
  json += "\"bBin\":\"" + bBin + "\",";
  json += "\"operation\":\"" + operation + "\",";
  json += "\"aDec\":" + String(aDec) + ",";
  json += "\"bDec\":" + String(bDec) + ",";
  json += "\"resultDec\":" + String(resultDec) + ",";
  json += "\"resultBin\":\"" + resultBin + "\",";
  json += "\"overflow\":" + String(overflow ? "true" : "false") + ",";
  json += "\"status\":\"" + String(overflow ? "OVERFLOW" : "OK") + "\"";
  json += "}";
  return json;
}

String buildErrorJson(String message) {
  String json = "{";
  json += "\"ok\":false,";
  json += "\"message\":\"" + message + "\"";
  json += "}";
  return json;
}

// ======================
// ROTAS DO WEBSERVER
// ======================

void handleRoot() {
  server.send(200, "text/html", paginaHTML);
}

void handleCalc() {
  String aBin = server.arg("a");
  String bBin = server.arg("b");
  String op = server.arg("op");

  aBin.trim();
  bBin.trim();
  op.trim();

  if (!isBinary4(aBin) || !isBinary4(bBin)) {
    server.send(400, "application/json", buildErrorJson("Erro: A e B devem conter exatamente 4 bits, usando apenas 0 e 1."));
    return;
  }

  if (op != "add" && op != "sub") {
    server.send(400, "application/json", buildErrorJson("Erro: operação inválida. Use add para soma ou sub para subtração."));
    return;
  }

  int aDec = bin4ToSignedInt(aBin);
  int bDec = bin4ToSignedInt(bBin);

  int resultDec;
  String operationName;

  if (op == "add") {
    resultDec = aDec + bDec;
    operationName = "SOMA";
  } else {
    resultDec = aDec - bDec;
    operationName = "SUBTRAÇÃO";
  }

  // Faixa representável em 4 bits com complemento de dois: -8 a +7.
  bool overflow = (resultDec < -8 || resultDec > 7);

  // Mesmo em overflow, os LEDs mostram os 4 bits resultantes do truncamento.
  // O status OVERFLOW avisa que o valor matemático não cabe em 4 bits.
  String resultBin = toBinary4(resultDec);
  writeLeds4Bits(resultDec);

  ultimoResultado =
    operationName + ": A=" + aBin + " (" + String(aDec) + "), B=" + bBin + " (" + String(bDec) + "), Resultado=" +
    resultBin + " (" + String(resultDec) + "), Status=" + String(overflow ? "OVERFLOW" : "OK");

  Serial.println(ultimoResultado);

  String json = buildJsonResponse(aBin, bBin, operationName, aDec, bDec, resultDec, resultBin, overflow);
  server.send(200, "application/json", json);
}

void handleNotFound() {
  server.send(404, "text/plain", "Rota não encontrada. Acesse / ou use /calc?a=0011&b=0010&op=add");
}

// ======================
// SETUP E LOOP
// ======================

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("Iniciando Calculadora Binaria de 4 bits...");

  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  clearLeds();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.println("Rede Wi-Fi criada:");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Senha: ");
  Serial.println(password);
  Serial.print("Acesse no navegador: http://");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/calc", handleCalc);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Servidor Web iniciado.");
}

void loop() {
  server.handleClient();
}