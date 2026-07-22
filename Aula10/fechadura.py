import RPi.GPIO as GPIO
import time
from RPLCD.i2c import CharLCD

# ==========================================================
# CONFIGURACOES
# ==========================================================

# Modos:
# "teclado", "lcd", "buzzer", "sensor" ou "completo"
MODO = "completo"

SENHA_CORRETA = "1234"

# Distancia maxima para considerar a porta/tranca fechada
DISTANCIA_FECHADA = 10.0  # cm


# ==========================================================
# PINOS
# ==========================================================

# Teclado matricial 4x4
LINHAS = [5, 6, 13, 19]
COLUNAS = [12, 16, 20, 21]

TECLAS = [
    ["1", "2", "3", "A"],
    ["4", "5", "6", "B"],
    ["7", "8", "9", "C"],
    ["*", "0", "#", "D"]
]

# Buzzer
BUZZER = 18

# Sensor ultrassonico HC-SR04
TRIG = 23
ECHO = 24

# LCD I2C
ENDERECO_LCD = 0x27


# ==========================================================
# CONFIGURACAO GPIO
# ==========================================================

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

# Teclado
for linha in LINHAS:
    GPIO.setup(linha, GPIO.OUT)
    GPIO.output(linha, GPIO.HIGH)

for coluna in COLUNAS:
    GPIO.setup(coluna, GPIO.IN, pull_up_down=GPIO.PUD_UP)

# Buzzer
GPIO.setup(BUZZER, GPIO.OUT)
GPIO.output(BUZZER, GPIO.LOW)

# Ultrassonico
GPIO.setup(TRIG, GPIO.OUT)
GPIO.setup(ECHO, GPIO.IN)
GPIO.output(TRIG, GPIO.LOW)

time.sleep(0.5)


# ==========================================================
# LCD
# ==========================================================

lcd = None


def iniciar_lcd():
    global lcd

    lcd = CharLCD(
        i2c_expander="PCF8574",
        address=ENDERECO_LCD,
        port=1,
        cols=16,
        rows=2,
        charmap="A02"
    )


def escrever_lcd(linha1, linha2=""):
    lcd.clear()

    lcd.write_string(linha1[:16])

    lcd.cursor_pos = (1, 0)
    lcd.write_string(linha2[:16])


# ==========================================================
# TECLADO
# ==========================================================

def ler_tecla():

    for i, linha in enumerate(LINHAS):

        GPIO.output(linha, GPIO.LOW)

        for j, coluna in enumerate(COLUNAS):

            if GPIO.input(coluna) == GPIO.LOW:

                tecla = TECLAS[i][j]

                # Espera a tecla ser solta
                while GPIO.input(coluna) == GPIO.LOW:
                    time.sleep(0.02)

                GPIO.output(linha, GPIO.HIGH)

                # Debounce simples
                time.sleep(0.05)

                return tecla

        GPIO.output(linha, GPIO.HIGH)

    return None


# ==========================================================
# BUZZER
# ==========================================================

def bip_curto():

    GPIO.output(BUZZER, GPIO.HIGH)
    time.sleep(0.15)
    GPIO.output(BUZZER, GPIO.LOW)


def bip_longo():

    GPIO.output(BUZZER, GPIO.HIGH)
    time.sleep(0.7)
    GPIO.output(BUZZER, GPIO.LOW)


# ==========================================================
# SENSOR ULTRASSONICO
# ==========================================================

def medir_distancia():

    # Pulso de 10 microssegundos no TRIG
    GPIO.output(TRIG, GPIO.LOW)
    time.sleep(0.000002)

    GPIO.output(TRIG, GPIO.HIGH)
    time.sleep(0.00001)

    GPIO.output(TRIG, GPIO.LOW)

    inicio_espera = time.time()

    # Espera o ECHO subir
    while GPIO.input(ECHO) == GPIO.LOW:

        inicio_pulso = time.time()

        # Evita ficar preso caso o sensor nao responda
        if time.time() - inicio_espera > 0.03:
            return None

    inicio_espera = time.time()

    # Espera o ECHO descer
    while GPIO.input(ECHO) == GPIO.HIGH:

        fim_pulso = time.time()

        if time.time() - inicio_espera > 0.03:
            return None

    tempo = fim_pulso - inicio_pulso

    # Velocidade do som = aproximadamente 34300 cm/s
    distancia = (tempo * 34300) / 2

    return distancia


def porta_fechada():

    distancia = medir_distancia()

    if distancia is None:
        return False

    return distancia <= DISTANCIA_FECHADA


# ==========================================================
# TESTES ISOLADOS
# ==========================================================

def teste_teclado():

    print("Teste do teclado.")
    print("Pressione Ctrl+C para sair.")

    while True:

        tecla = ler_tecla()

        if tecla is not None:
            print("Tecla pressionada:", tecla)


def teste_lcd():

    iniciar_lcd()

    escrever_lcd(
        "Teste LCD",
        "Funcionando!"
    )

    print("Mensagem enviada ao LCD.")

    time.sleep(5)


def teste_buzzer():

    print("Bip curto")
    bip_curto()

    time.sleep(1)

    print("Bip longo")
    bip_longo()


def teste_sensor():

    print("Teste do sensor ultrassonico.")
    print("Pressione Ctrl+C para sair.")

    while True:

        distancia = medir_distancia()

        if distancia is None:
            print("Erro na leitura do sensor")

        else:

            print("Distancia: %.2f cm" % distancia)

            if distancia <= DISTANCIA_FECHADA:
                print("Estado: FECHADA")
            else:
                print("Estado: ABERTA")

        print("--------------------")

        time.sleep(0.5)


# ==========================================================
# PROGRAMA COMPLETO
# ==========================================================

def programa_completo():

    iniciar_lcd()

    senha_digitada = ""

    escrever_lcd(
        "Fechadura",
        "Digite a senha"
    )

    while True:

        tecla = ler_tecla()

        if tecla is None:
            time.sleep(0.02)
            continue

        print("Tecla:", tecla)

        # Digitos da senha
        if tecla.isdigit():

            if len(senha_digitada) < 6:
                senha_digitada += tecla

            escrever_lcd(
                "Senha:",
                "*" * len(senha_digitada)
            )

        # Apaga ultimo digito
        elif tecla == "*":

            senha_digitada = senha_digitada[:-1]

            escrever_lcd(
                "Senha:",
                "*" * len(senha_digitada)
            )

        # Limpa senha
        elif tecla == "A":

            senha_digitada = ""

            escrever_lcd(
                "Senha limpa",
                ""
            )

            time.sleep(0.5)

            escrever_lcd(
                "Fechadura",
                "Digite a senha"
            )

        # Confirma senha
        elif tecla == "#":

            if senha_digitada == SENHA_CORRETA:

                print("Senha correta")

                escrever_lcd(
                    "Acesso liberado",
                    ""
                )

                bip_curto()

            else:

                print("Senha incorreta")

                escrever_lcd(
                    "Acesso negado",
                    ""
                )

                bip_longo()

            senha_digitada = ""

            time.sleep(1)

            # Depois de validar a senha,
            # verifica o estado fisico da porta/tranca
            distancia = medir_distancia()

            if distancia is None:

                escrever_lcd(
                    "Erro no sensor",
                    ""
                )

            elif distancia <= DISTANCIA_FECHADA:

                print("Porta/tranca fechada")
                print("Distancia: %.2f cm" % distancia)

                escrever_lcd(
                    "Estado:",
                    "FECHADA"
                )

            else:

                print("Porta/tranca aberta")
                print("Distancia: %.2f cm" % distancia)

                escrever_lcd(
                    "Estado:",
                    "ABERTA"
                )

            time.sleep(2)

            escrever_lcd(
                "Fechadura",
                "Digite a senha"
            )


# ==========================================================
# EXECUCAO
# ==========================================================

try:

    if MODO == "teclado":
        teste_teclado()

    elif MODO == "lcd":
        teste_lcd()

    elif MODO == "buzzer":
        teste_buzzer()

    elif MODO == "sensor":
        teste_sensor()

    elif MODO == "completo":
        programa_completo()

    else:
        print("Modo invalido")


except KeyboardInterrupt:

    print("\nPrograma encerrado")


finally:

    if lcd is not None:
        lcd.clear()

    GPIO.cleanup()
