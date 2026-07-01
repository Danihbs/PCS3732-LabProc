import time
import RPi.GPIO as GPIO
from RPLCD.i2c import CharLCD


LINHAS = [5, 6, 13, 19]
COLUNAS = [12, 16, 20, 21]

TECLAS = [
    ["1", "2", "3", "+"],
    ["4", "5", "6", "-"],
    ["7", "8", "9", "*"],
    ["C", "0", "=", "/"]
]

lcd = CharLCD(
    i2c_expander="PCF8574",
    address=0x27,
    port=1,
    cols=16,
    rows=2
)


def configurar_gpio():
    GPIO.setmode(GPIO.BCM)

    for linha in LINHAS:
        GPIO.setup(linha, GPIO.OUT)
        GPIO.output(linha, GPIO.LOW)

    for coluna in COLUNAS:
        GPIO.setup(coluna, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)


def limpar_lcd():
    lcd.clear()


def escrever_lcd(linha1="", linha2=""):
    lcd.clear()
    lcd.write_string(str(linha1)[:16])

    lcd.cursor_pos = (1, 0)
    lcd.write_string(str(linha2)[:16])


def ler_tecla():
    while True:
        for i in range(len(LINHAS)):
            GPIO.output(LINHAS[i], GPIO.HIGH)

            for j in range(len(COLUNAS)):
                if GPIO.input(COLUNAS[j]) == GPIO.HIGH:
                    tecla = TECLAS[i][j]

                    while GPIO.input(COLUNAS[j]) == GPIO.HIGH:
                        time.sleep(0.05)

                    GPIO.output(LINHAS[i], GPIO.LOW)
                    return tecla

            GPIO.output(LINHAS[i], GPIO.LOW)

        time.sleep(0.05)


def calcular(a, b, op):
    if op == "+":
        return a + b

    if op == "-":
        return a - b

    if op == "*":
        return a * b

    if op == "/":
        if b == 0:
            return None
        return a // b

    return None


def executar_calculadora():
    numero_atual = ""
    primeiro_numero = None
    operacao = None

    escrever_lcd("Calculadora", "Digite valor")

    while True:
        tecla = ler_tecla()

        if tecla == "C":
            numero_atual = ""
            primeiro_numero = None
            operacao = None
            escrever_lcd("Calculadora", "Digite valor")

        elif tecla in ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"]:
            numero_atual += tecla

            if operacao is None:
                escrever_lcd(numero_atual, "")
            else:
                escrever_lcd(str(primeiro_numero) + operacao, numero_atual)

        elif tecla in ["+", "-", "*", "/"]:
            if numero_atual != "":
                primeiro_numero = int(numero_atual)
                operacao = tecla
                numero_atual = ""
                escrever_lcd(str(primeiro_numero) + operacao, "")

        elif tecla == "=":
            if primeiro_numero is not None and operacao is not None and numero_atual != "":
                segundo_numero = int(numero_atual)
                resultado = calcular(primeiro_numero, segundo_numero, operacao)

                if resultado is None:
                    escrever_lcd("Erro", "Div por zero")
                else:
                    escrever_lcd("Resultado", resultado)

                numero_atual = ""
                primeiro_numero = None
                operacao = None


try:
    configurar_gpio()
    executar_calculadora()

except KeyboardInterrupt:
    pass

finally:
    limpar_lcd()
    GPIO.cleanup()