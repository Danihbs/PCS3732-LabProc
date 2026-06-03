/*
  Calculadora ESP32 WebServer - versao para ESP32 fisica
  Operacoes: soma, subtracao, multiplicacao, fatorial e divisao.

  Requisitos atendidos:
  - ESP32 cria rede Wi-Fi propria em modo Access Point.
  - Interface web acessada por HTTP em http://192.168.4.1.
  - JavaScript apenas valida entrada e envia requisicao HTTP.
  - Operacoes aritmeticas implementadas no firmware C/C++ da ESP32.
  - LEDs mostram os 4 bits menos significativos do resultado.
  - Suporte a 4, 8, 12 e 16 bits para os operandos.

  Ligacao dos LEDs:
  GPIO -> resistor 220 ohms -> anodo do LED
  catodo do LED -> GND
*/

#include <WiFi.h>
#include <WebServer.h>
#include <stdint.h>

const char* ssid = "Calculadora_ESP32";
const char* password = "12345678";

WebServer server(80);

// Para ESP32 DevKit comum. Altere se sua montagem usar outros pinos.
const int ledPins[4] = {26, 27, 25, 33}; // bit0, bit1, bit2, bit3
const int LED_ON = HIGH;
const int LED_OFF = LOW;

const char paginaHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Calculadora ESP32</title>
  <style>
    body { font-family: Arial, Helvetica, sans-serif; background:#f3f3f3; margin:0; padding:24px; color:#222; }
    .card { max-width:680px; margin:0 auto; background:white; padding:22px; border-radius:12px; box-shadow:0 2px 12px rgba(0,0,0,.16); }
    h1 { text-align:center; margin:0 0 8px; font-size:24px; }
    .subtitulo { text-align:center; color:#555; margin-bottom:20px; font-size:14px; }
    label { display:block; margin-top:14px; margin-bottom:6px; font-weight:bold; }
    input, select { width:100%; box-sizing:border-box; padding:11px; font-size:18px; border:1px solid #bbb; border-radius:8px; }
    input { text-align:center; letter-spacing:2px; }
    .grid { display:grid; grid-template-columns:1fr 1fr; gap:12px; }
    .botoes { display:grid; grid-template-columns:1fr 1fr; gap:12px; margin-top:18px; }
    button { padding:13px; font-size:15px; border:0; border-radius:8px; cursor:pointer; color:white; background:#2c6e49; font-weight:bold; }
    button.sub { background:#3a506b; }
    button.mul { background:#6a4c93; }
    button.fact { background:#bc6c25; }
    button.div { background:#8d0801; }
    .resultado { margin-top:20px; padding:14px; border-radius:8px; background:#eee; white-space:pre-line; line-height:1.45; font-size:15px; overflow:auto; }
    .ok { border-left:6px solid #2c6e49; }
    .overflow { border-left:6px solid #c1121f; background:#fff0f0; }
    .erro { border-left:6px solid #f77f00; background:#fff8ea; }
    .nota { margin-top:18px; font-size:13px; color:#555; line-height:1.4; }
    code { background:#eee; padding:2px 4px; border-radius:4px; }
  </style>
</head>
<body>
  <div class="card">
    <h1>Calculadora ESP32</h1>
    <div class="subtitulo">Soma, subtração, multiplicação, fatorial e divisão — cálculo feito em C/C++ na ESP32</div>

    <label for="bits">Tamanho dos operandos:</label>
    <select id="bits" onchange="ajustarTamanho()">
      <option value="4">4 bits (-8 a 7)</option>
      <option value="8">8 bits (-128 a 127)</option>
      <option value="12">12 bits (-2048 a 2047)</option>
      <option value="16">16 bits (-32768 a 32767)</option>
    </select>

    <div class="grid">
      <div>
        <label for="a">Operando A:</label>
        <input id="a" value="0011" inputmode="numeric">
      </div>
      <div>
        <label for="b">Operando B:</label>
        <input id="b" value="0010" inputmode="numeric">
      </div>
    </div>

    <div class="botoes">
      <button onclick="calcular('add')">SOMA</button>
      <button class="sub" onclick="calcular('sub')">SUBTRAÇÃO</button>
      <button class="mul" onclick="calcular('mul')">MULTIPLICAÇÃO</button>
      <button class="fact" onclick="calcular('fact')">FATORIAL DE A</button>
      <button class="div" onclick="calcular('div')">DIVISÃO</button>
    </div>

    <div id="resultado" class="resultado">Aguardando operação...</div>

    <div class="nota">
      O JavaScript somente valida a entrada e envia requisições HTTP para <code>/calc</code>.
      As operações, iterações, verificação de overflow e acionamento dos LEDs são feitos no firmware C/C++ da ESP32.
      Os LEDs físicos exibem apenas os 4 bits menos significativos do resultado.
    </div>
  </div>

<script>
  function bitsSelecionados() { return parseInt(document.getElementById('bits').value); }
  function entradaValida(valor, bits) { return new RegExp('^[01]{' + bits + '}$').test(valor); }
  function normalizar(campo) {
    const bits = bitsSelecionados();
    campo.value = campo.value.replace(/[^01]/g, '').slice(0, bits);
  }
  function ajustarTamanho() {
    const bits = bitsSelecionados();
    document.getElementById('a').maxLength = bits;
    document.getElementById('b').maxLength = bits;
    normalizar(document.getElementById('a'));
    normalizar(document.getElementById('b'));
  }
  document.getElementById('a').addEventListener('input', function(){ normalizar(this); });
  document.getElementById('b').addEventListener('input', function(){ normalizar(this); });
  ajustarTamanho();

  function calcular(op) {
    const bits = bitsSelecionados();
    const a = document.getElementById('a').value.trim();
    const b = document.getElementById('b').value.trim();
    const r = document.getElementById('resultado');
    if (!entradaValida(a, bits) || (op !== 'fact' && !entradaValida(b, bits))) {
      r.className = 'resultado erro';
      r.innerText = 'Erro: informe exatamente ' + bits + ' bits em A' + (op !== 'fact' ? ' e B.' : '.');
      return;
    }
    r.className = 'resultado';
    r.innerText = 'Enviando requisição HTTP para a ESP32...';
    fetch(`/calc?a=${a}&b=${b}&op=${op}&bits=${bits}`)
      .then(res => res.json())
      .then(data => {
        if (!data.ok) { r.className = 'resultado erro'; r.innerText = data.message; return; }
        r.className = data.overflow ? 'resultado overflow' : 'resultado ok';
        r.innerText =
          `Operação: ${data.operation}\n` +
          `Bits: ${data.bits}\n` +
          `A: ${data.aBin} (${data.aDec})\n` +
          (op !== 'fact' ? `B: ${data.bBin} (${data.bDec})\n` : '') +
          `Resultado decimal: ${data.resultDec}\n` +
          `Resultado binário/truncado: ${data.resultBin}\n` +
          `LEDs: ${data.ledBin}\n` +
          `Iterações: ${data.iterations}\n` +
          `Tempo: ${data.timeUs} us\n` +
          `Status: ${data.status}`;
      })
      .catch(() => { r.className = 'resultado erro'; r.innerText = 'Erro de comunicação com a ESP32.'; });
  }

</script>
</body>
</html>
)rawliteral";

bool isSupportedBits(int bits) { return bits == 4 || bits == 8 || bits == 12 || bits == 16; }

bool isBinaryN(const String &value, int bits) {
  if (value.length() != bits) return false;
  for (int i = 0; i < bits; i++) if (value[i] != '0' && value[i] != '1') return false;
  return true;
}

int32_t maxPositiveForBits(int bits) { return (1L << (bits - 1)) - 1; }
int32_t minNegativeForBits(int bits) { return -(1L << (bits - 1)); }
bool fitsSignedRange(int64_t v, int bits) { return v >= minNegativeForBits(bits) && v <= maxPositiveForBits(bits); }

int32_t binToSignedInt(const String &bin, int bits) {
  uint32_t unsignedValue = 0;
  for (int i = 0; i < bits; i++) unsignedValue = (unsignedValue << 1) | (bin[i] - '0');
  uint32_t signBit = 1UL << (bits - 1);
  if (unsignedValue & signBit) return (int32_t)unsignedValue - (1L << bits);
  return (int32_t)unsignedValue;
}

uint64_t maskForBits(int bits) { return (1ULL << bits) - 1ULL; }
uint64_t signedToTwosComplement(int64_t value, int bits) { return ((uint64_t)value) & maskForBits(bits); }

String toBinaryUnsigned(uint64_t value, int bits) {
  String out = "";
  for (int i = bits - 1; i >= 0; i--) out += ((value >> i) & 1ULL) ? '1' : '0';
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

void writeLeds4Bits(uint64_t value) {
  uint8_t bits = value & 0x0F;
  for (int i = 0; i < 4; i++) digitalWrite(ledPins[i], ((bits >> i) & 1) ? LED_ON : LED_OFF);
}

void clearLeds() {
  for (int i = 0; i < 4; i++) digitalWrite(ledPins[i], LED_OFF);
}

void testLedsIndependentes() {
  Serial.println("Teste independente dos 4 LEDs...");
  clearLeds();
  for (int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], LED_ON);
    delay(300);
    digitalWrite(ledPins[i], LED_OFF);
    delay(150);
  }
  Serial.println("Teste dos LEDs concluido.");
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
  int64_t result = quotient;
  return negative ? -result : result;
}

bool executeOperation(String op, int bits, int32_t a, int32_t b, int64_t &signedResult, uint64_t &unsignedResult, bool &isUnsigned, bool &overflow, uint32_t &iterations, String &errorMessage) {
  signedResult = 0; unsignedResult = 0; isUnsigned = false; overflow = false; iterations = 0; errorMessage = "";
  if (op == "add") signedResult = (int64_t)a + b;
  else if (op == "sub") signedResult = (int64_t)a - b;
  else if (op == "mul") signedResult = multiplyIterative(a, b, iterations);
  else if (op == "div") {
    bool divisionByZero = false;
    signedResult = divideIterative(a, b, iterations, divisionByZero);
    if (divisionByZero) { errorMessage = "Erro: divisao por zero."; return false; }
  }
  else if (op == "fact") {
    if (a < 0) { errorMessage = "Erro: fatorial aceita apenas A >= 0."; return false; }
    bool overflow64 = false;
    unsignedResult = factorialIterative((uint32_t)a, iterations, overflow64);
    isUnsigned = true;
    overflow = overflow64 || unsignedResult > (uint64_t)maxPositiveForBits(bits);
    return true;
  } else {
    errorMessage = "Erro: operacao invalida.";
    return false;
  }
  overflow = !fitsSignedRange(signedResult, bits);
  return true;
}

uint32_t measureSingleOperationUs(String op, int bits, int32_t a, int32_t b) {
  int64_t sr; uint64_t ur; bool isU, ov; uint32_t it; String err;
  uint32_t t0 = micros();
  executeOperation(op, bits, a, b, sr, ur, isU, ov, it, err);
  uint32_t t1 = micros();
  return t1 - t0;
}

String buildErrorJson(String msg) {
  return String("{\"ok\":false,\"message\":\"") + msg + "\"}";
}

String buildJson(String aBin, String bBin, String opName, int bits, int32_t aDec, int32_t bDec, String resultDec, String resultBin, String ledBin, bool overflow, uint32_t iterations, uint32_t timeUs) {
  String json = "{";
  json += "\"ok\":true,";
  json += "\"operation\":\"" + opName + "\",";
  json += "\"bits\":" + String(bits) + ",";
  json += "\"aBin\":\"" + aBin + "\",\"bBin\":\"" + bBin + "\",";
  json += "\"aDec\":" + String(aDec) + ",\"bDec\":" + String(bDec) + ",";
  json += "\"resultDec\":\"" + resultDec + "\",";
  json += "\"resultBin\":\"" + resultBin + "\",\"ledBin\":\"" + ledBin + "\",";
  json += "\"overflow\":" + String(overflow ? "true" : "false") + ",";
  json += "\"iterations\":" + String(iterations) + ",";
  json += "\"timeUs\":" + String(timeUs) + ",";
  json += "\"status\":\"" + String(overflow ? "OVERFLOW/TRUNCAMENTO" : "OK") + "\"";
  json += "}";
  return json;
}

void handleRoot() { server.send(200, "text/html", paginaHTML); }

void handleCalc() {
  String aBin = server.arg("a");
  String bBin = server.arg("b");
  String op = server.arg("op");
  int bits = server.arg("bits").toInt();
  aBin.trim(); bBin.trim(); op.trim();

  if (!isSupportedBits(bits)) { server.send(400, "application/json", buildErrorJson("Erro: bits deve ser 4, 8, 12 ou 16.")); return; }
  if (!isBinaryN(aBin, bits) || (op != "fact" && !isBinaryN(bBin, bits))) { server.send(400, "application/json", buildErrorJson("Erro: A e B devem ter a quantidade correta de bits.")); return; }
  if (op != "add" && op != "sub" && op != "mul" && op != "fact" && op != "div") { server.send(400, "application/json", buildErrorJson("Erro: operacao invalida.")); return; }

  int32_t aDec = binToSignedInt(aBin, bits);
  int32_t bDec = (op == "fact") ? 0 : binToSignedInt(bBin, bits);
  int64_t signedResult; uint64_t unsignedResult; bool isUnsigned, overflow; uint32_t iterations; String error;

  uint32_t t0 = micros();
  bool ok = executeOperation(op, bits, aDec, bDec, signedResult, unsignedResult, isUnsigned, overflow, iterations, error);
  uint32_t t1 = micros();
  if (!ok) { clearLeds(); server.send(400, "application/json", buildErrorJson(error)); return; }

  uint64_t displayValue = isUnsigned ? (unsignedResult & maskForBits(bits)) : signedToTwosComplement(signedResult, bits);
  writeLeds4Bits(displayValue);

  String opName = op == "add" ? "SOMA" : op == "sub" ? "SUBTRACAO" : op == "mul" ? "MULTIPLICACAO" : op == "div" ? "DIVISAO" : "FATORIAL DE A";
  String resultDec = isUnsigned ? u64ToString(unsignedResult) : i64ToString(signedResult);
  String resultBin = toBinaryUnsigned(displayValue, bits);
  String ledBin = toBinaryUnsigned(displayValue & 0x0F, 4);

  Serial.println(opName + " A=" + String(aDec) + " B=" + String(bDec) + " Resultado=" + resultDec + " tempo=" + String(t1 - t0) + "us");
  server.send(200, "application/json", buildJson(aBin, bBin, opName, bits, aDec, bDec, resultDec, resultBin, ledBin, overflow, iterations, t1 - t0));
}

void handleNotFound() { server.send(404, "text/plain", "Rota nao encontrada. Acesse / ou /calc."); }

void runRegressionTests() {
  int64_t sr; uint64_t ur; bool isU, ov; uint32_t it; String err;
  Serial.println("Testes iniciais:");
  executeOperation("add", 4, 2, 4, sr, ur, isU, ov, it, err); Serial.println(String("2+4=") + i64ToString(sr));
  executeOperation("sub", 4, 5, 2, sr, ur, isU, ov, it, err); Serial.println(String("5-2=") + i64ToString(sr));
  executeOperation("mul", 4, 3, 2, sr, ur, isU, ov, it, err); Serial.println(String("3*2=") + i64ToString(sr));
  executeOperation("fact", 8, 5, 0, sr, ur, isU, ov, it, err); Serial.println(String("5!=") + u64ToString(ur));
  executeOperation("div", 4, 6, 2, sr, ur, isU, ov, it, err); Serial.println(String("6/2=") + i64ToString(sr));
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Iniciando Calculadora ESP32 Web - soma/sub/mul/fatorial/divisao");
  for (int i = 0; i < 4; i++) pinMode(ledPins[i], OUTPUT);
  clearLeds();
  testLedsIndependentes();
  runRegressionTests();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.println("Rede Wi-Fi criada");
  Serial.print("SSID: "); Serial.println(ssid);
  Serial.print("Senha: "); Serial.println(password);
  Serial.print("Acesse: http://"); Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/calc", handleCalc);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Servidor Web iniciado.");
}

void loop() {
  server.handleClient();
}
