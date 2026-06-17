#include <Arduino.h>

// Pinos
const int LED_AMARELO = 4;
const int LED_VERMELHO = 5;
const int BOTAO_SOS = 2;
const int LDR_PIN = 0;

// Variáveis compartilhadas com ISR
volatile bool sosAtivado = false;

// Controle do LED amarelo
unsigned long ultimoPisca = 0;
bool estadoAmarelo = false;

// Controle da emergência
bool emergenciaEmAndamento = false;
unsigned long inicioEmergencia = 0;

// ISR (Interruption Service Routine)
void IRAM_ATTR trataSOS()
{
    sosAtivado = true;
}

void setup()
{
    Serial.begin(115200);

    pinMode(LED_AMARELO, OUTPUT);
    pinMode(LED_VERMELHO, OUTPUT);

    pinMode(BOTAO_SOS, INPUT_PULLUP);

    attachInterrupt(
        digitalPinToInterrupt(BOTAO_SOS),
        trataSOS,
        FALLING);

    digitalWrite(LED_AMARELO, LOW);
    digitalWrite(LED_VERMELHO, LOW);

    Serial.println("Sistema iniciado.");
}

void loop()
{
    unsigned long agora = millis();

    // ==================================================
    // 1. Tratamento da interrupção SOS
    // ==================================================
    if (sosAtivado && !emergenciaEmAndamento)
    {
        sosAtivado = false;

        emergenciaEmAndamento = true;
        inicioEmergencia = agora;

        digitalWrite(LED_AMARELO, LOW);
        digitalWrite(LED_VERMELHO, HIGH);

        Serial.println("SOS acionado!");
    }

    // ==================================================
    // 2. Modo Emergência
    // ==================================================
    if (emergenciaEmAndamento)
    {
        if (agora - inicioEmergencia >= 3000)
        {
            digitalWrite(LED_VERMELHO, LOW);

            emergenciaEmAndamento = false;

            Serial.println("Fim da emergencia.");
        }

        // Enquanto estiver em emergência,
        // o loop principal fica interrompido.
        return;
    }

    // ==================================================
    // 3. Loop principal normal
    // ==================================================

    // Pisca LED amarelo a cada 2 s
    if (agora - ultimoPisca >= 2000)
    {
        ultimoPisca = agora;

        estadoAmarelo = !estadoAmarelo;
        digitalWrite(LED_AMARELO, estadoAmarelo);
    }

    // Leitura do LDR
    static unsigned long ultimaLeitura = 0;

    if (agora - ultimaLeitura >= 1000)
    {
        ultimaLeitura = agora;

        int valorLDR = analogRead(LDR_PIN);

        Serial.print("LDR = ");
        Serial.println(valorLDR);

        // Aqui futuramente você pode atualizar o webserver
    }
}