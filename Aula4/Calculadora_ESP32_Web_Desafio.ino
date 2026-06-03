#include <WiFi.h>
#include <WebServer.h>
#include <stdint.h>

const char* ssid = "Calculadora_ESP32";
const char* password = "12345678";

WebServer server(80);

const int ledPins[4] = {1, 2, 3, 4};

const char paginaHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Calculadora ESP32</title>
  <style>
    body { font-family: Arial, sans-serif; background: #f4f4f4; margin: 0; padding: 24px; color: #222; }
    .card { max-width: 620px; margin: 0 auto; background: #fff; padding: 24px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,.12); }
    h1 { margin: 0 0 18px; text-align: center; font-size: 24px; }
    label { display: block; margin-top: 14px; margin-bottom: 6px; font-weight: bold; }
    input, select { width: 100%; box-sizing: border-box; padding: 10px; font-size: 17px; border: 1px solid #bbb; border-radius: 6px; }
    input { text-align: center; }
    .grid { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; }
    .botoes { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-top: 18px; }
    button { padding: 12px; font-size: 15px; border: 0; border-radius: 6px; cursor: pointer; color: white; background: #2f5f98; font-weight: bold; }
    .resultado { margin-top: 18px; padding: 14px; border-radius: 6px; background: #eeeeee; white-space: pre-line; line-height: 1.45; font-size: 15px; }
    .ok { border-left: 5px solid #2e7d32; }
    .erro { border-left: 5px solid #c62828; background: #fff0f0; }
  </style>
</head>
<body>
  <div class="card">
    <h1>Calculadora ESP32</h1>

    <label for="bits">Bits</label>
    <select id="bits">
      <option value="4">4 bits (-8 a 7)</option>
      <option value="8">8 bits (-128 a 127)</option>
      <option value="12">12 bits (-2048 a 2047)</option>
      <option value="16">16 bits (-32768 a 32767)</option>
    </select>

    <div class="grid">
      <div>
        <label for="a">A</label>
        <input id="a" type="number" value="3">
      </div>
      <div>
        <label for="b">B</label>
        <input id="b" type="number" value="2">
      </div>
    </div>

    <div class="botoes">
      <button onclick="calcular('add')">Soma</button>
      <button onclick="calcular('sub')">Subtração</button>
      <button onclick="calcular('mul')">Multiplicação</button>
      <button onclick="calcular('fact')">Fatorial de A</button>
      <button onclick="calcular('div')">Divisão</button>
    </div>

    <div id="resultado" class="resultado">Aguardando operação.</div>
  </div>

<script>
function bitsSelecionados() {
  return parseInt(document.getElementById("bits").value);
}

function faixa(bits) {
  return {
    min: -(2 ** (bits - 1)),
    max: (2 ** (bits - 1)) - 1
  };
}

function calcular(op) {
  const bits = bitsSelecionados();
  const a = parseInt(document.getElementById("a").value);
  const b = parseInt(document.getElementById("b").value);
  const r = document.getElementById("resultado");
  const f = faixa(bits);

  if (Number.isNaN(a) || (op !== "fact" && Number.isNaN(b))) {
    r.className = "resultado erro";
    r.innerText = "Entrada inválida.";
    return;
  }

  if (a < f.min || a > f.max || (op !== "fact" && (b < f.min || b > f.max))) {
    r.className = "resultado erro";
    r.innerText = `Valores fora da faixa para ${bits} bits: ${f.min} a ${f.max}.`;
    return;
  }

  fetch(`/calc?a=${a}&b=${b}&op=${op}&bits=${bits}`)
    .then(res => res.json())
    .then(data => {
      if (!data.ok) {
        r.className = "resultado erro";
        r.innerText = data.message;
        return;
      }

      r.className = "resultado ok";
      r.innerText =
        `Operação: ${data.operation}\n` +
        `A: ${data.aDec}\n` +
        (op !== "fact" ? `B: ${data.bDec}\n` : "") +
        `Resultado: ${data.resultDec}\n` +
        `Binário: ${data.resultBin}\n` +
        `LEDs: ${data.ledBin}\n` +
        `Tempo: ${data.timeUs} us\n` +
        `Status: ${data.status}`;
    })
    .catch(() => {
      r.className = "resultado erro";
      r.innerText = "Erro de comunicação.";
    });
}
</script>
</body>
</html>
)rawliteral";

bool isSupportedBits(int bits) {
  return bits == 4 || bits == 8 || bits == 12 || bits == 16;
}

int32_t maxPositiveForBits(int bits) {
  return (1L << (bits - 1)) - 1;
}

int32_t minNegativeForBits(int bits) {
  return -(1L << (bits - 1));
}

bool fitsSignedRange(int64_t value, int bits) {
  return value >= minNegativeForBits(bits) && value <= maxPositiveForBits(bits);
}

bool inputInRange(int32_t value, int bits) {
  return value >= minNegativeForBits(bits) && value <= maxPositiveForBits(bits);
}

uint64_t maskForBits(int bits) {
  return (1ULL << bits) - 1ULL;
}

uint64_t signedToTwosComplement(int64_t value, int bits) {
  return ((uint64_t)value) & maskForBits(bits);
}

String toBinary(uint64_t value, int bits) {
  String out = "";

  for (int i = bits - 1; i >= 0; i--) {
    out += ((value >> i) & 1ULL) ? '1' : '0';
  }

  return out;
}

String i64ToString(int64_t value) {
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%lld", (long long)value);
  return String(buffer);
}

String u64ToString(uint64_t value) {
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%llu", (unsigned long long)value);
  return String(buffer);
}

uint32_t magnitude32(int32_t value) {
  if (value >= 0) return (uint32_t)value;
  return (uint32_t)(-(int64_t)value);
}

void writeLeds(uint64_t value) {
  uint8_t lowerBits = value & 0x0F;

  for (int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], ((lowerBits >> i) & 1) ? HIGH : LOW);
  }
}

void clearLeds() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

int64_t multiplyIterative(int32_t a, int32_t b, uint32_t &iterations) {
  bool negative = (a < 0) ^ (b < 0);
  uint32_t multiplicand = magnitude32(a);
  uint32_t multiplier = magnitude32(b);
  int64_t result = 0;

  iterations = 0;

  for (uint32_t i = 0; i < multiplier; i++) {
    result += multiplicand;
    iterations++;
  }

  return negative ? -result : result;
}

uint64_t factorialIterative(uint32_t n, uint32_t &iterations, bool &overflow64) {
  uint64_t result = 1;

  iterations = 0;
  overflow64 = false;

  for (uint32_t i = 2; i <= n; i++) {
    if (result > UINT64_MAX / i) {
      overflow64 = true;
      return result;
    }

    result *= i;
    iterations++;
  }

  return result;
}

int64_t divideIterative(int32_t a, int32_t b, uint32_t &iterations, bool &divisionByZero) {
  iterations = 0;
  divisionByZero = false;

  if (b == 0) {
    divisionByZero = true;
    return 0;
  }

  bool negative = (a < 0) ^ (b < 0);
  uint32_t dividend = magnitude32(a);
  uint32_t divisor = magnitude32(b);
  uint32_t quotient = 0;

  while (dividend >= divisor) {
    dividend -= divisor;
    quotient++;
    iterations++;
  }

  return negative ? -(int64_t)quotient : (int64_t)quotient;
}

bool executeOperation(
  String op,
  int bits,
  int32_t a,
  int32_t b,
  int64_t &signedResult,
  uint64_t &unsignedResult,
  bool &isUnsigned,
  bool &overflow,
  uint32_t &iterations,
  String &errorMessage
) {
  signedResult = 0;
  unsignedResult = 0;
  isUnsigned = false;
  overflow = false;
  iterations = 0;
  errorMessage = "";

  if (op == "add") {
    signedResult = (int64_t)a + b;
  } else if (op == "sub") {
    signedResult = (int64_t)a - b;
  } else if (op == "mul") {
    signedResult = multiplyIterative(a, b, iterations);
  } else if (op == "fact") {
    if (a < 0) {
      errorMessage = "Erro: fatorial aceita apenas A >= 0.";
      return false;
    }

    bool overflow64 = false;
    unsignedResult = factorialIterative((uint32_t)a, iterations, overflow64);
    isUnsigned = true;
    overflow = overflow64 || unsignedResult > (uint64_t)maxPositiveForBits(bits);
    return true;
  } else if (op == "div") {
    bool divisionByZero = false;
    signedResult = divideIterative(a, b, iterations, divisionByZero);

    if (divisionByZero) {
      errorMessage = "Erro: divisao por zero.";
      return false;
    }
  } else {
    errorMessage = "Erro: operacao invalida.";
    return false;
  }

  overflow = !fitsSignedRange(signedResult, bits);
  return true;
}

String jsonError(String message) {
  return String("{\"ok\":false,\"message\":\"") + message + "\"}";
}

String jsonResult(
  String opName,
  int bits,
  int32_t aDec,
  int32_t bDec,
  String resultDec,
  String resultBin,
  String ledBin,
  bool overflow,
  uint32_t timeUs
) {
  String json = "{";
  json += "\"ok\":true,";
  json += "\"operation\":\"" + opName + "\",";
  json += "\"bits\":" + String(bits) + ",";
  json += "\"aDec\":" + String(aDec) + ",";
  json += "\"bDec\":" + String(bDec) + ",";
  json += "\"resultDec\":\"" + resultDec + "\",";
  json += "\"resultBin\":\"" + resultBin + "\",";
  json += "\"ledBin\":\"" + ledBin + "\",";
  json += "\"overflow\":" + String(overflow ? "true" : "false") + ",";
  json += "\"timeUs\":" + String(timeUs) + ",";
  json += "\"status\":\"" + String(overflow ? "OVERFLOW" : "OK") + "\"";
  json += "}";

  return json;
}

void handleRoot() {
  server.send(200, "text/html", paginaHTML);
}

void handleCalc() {
  String op = server.arg("op");
  int bits = server.arg("bits").toInt();
  int32_t aDec = server.arg("a").toInt();
  int32_t bDec = server.arg("b").toInt();

  op.trim();

  if (!isSupportedBits(bits)) {
    server.send(400, "application/json", jsonError("Quantidade de bits invalida."));
    return;
  }

  if (!inputInRange(aDec, bits) || (op != "fact" && !inputInRange(bDec, bits))) {
    server.send(400, "application/json", jsonError("Valor fora da faixa para a quantidade de bits selecionada."));
    return;
  }

  int64_t signedResult = 0;
  uint64_t unsignedResult = 0;
  bool isUnsigned = false;
  bool overflow = false;
  uint32_t iterations = 0;
  String errorMessage = "";

  uint32_t t0 = micros();
  bool ok = executeOperation(op, bits, aDec, bDec, signedResult, unsignedResult, isUnsigned, overflow, iterations, errorMessage);
  uint32_t t1 = micros();

  if (!ok) {
    clearLeds();
    server.send(400, "application/json", jsonError(errorMessage));
    return;
  }

  uint64_t displayValue = isUnsigned ? (unsignedResult & maskForBits(bits)) : signedToTwosComplement(signedResult, bits);

  writeLeds(displayValue);

  String opName =
    op == "add" ? "SOMA" :
    op == "sub" ? "SUBTRACAO" :
    op == "mul" ? "MULTIPLICACAO" :
    op == "fact" ? "FATORIAL" :
    "DIVISAO";

  String resultDec = isUnsigned ? u64ToString(unsignedResult) : i64ToString(signedResult);
  String resultBin = toBinary(displayValue, bits);
  String ledBin = toBinary(displayValue & 0x0F, 4);

  server.send(
    200,
    "application/json",
    jsonResult(opName, bits, aDec, bDec, resultDec, resultBin, ledBin, overflow, t1 - t0)
  );
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  clearLeds();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.print("Acesse: http://");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/calc", handleCalc);
  server.begin();
}

void loop() {
  server.handleClient();
}
