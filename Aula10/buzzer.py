import RPi.GPIO as GPIO
import time

BUZZER = 4

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(BUZZER, GPIO.OUT)

pwm = GPIO.PWM(BUZZER, 440)

NOTAS = {
    "C4": 262,
    "D4": 294,
    "E4": 330,
    "F4": 349,
    "G4": 392,
    "A4": 440,
    "B4": 494,

    "C5": 523,
    "D5": 587,
    "E5": 659,
    "F5": 698,
    "G5": 784,
    "A5": 880,
    "B5": 988,

    "C6": 1047,
    "PAUSA": 0
}


def tocar_nota(nota, duracao):
    freq = NOTAS[nota]

    if freq == 0:
        pwm.stop()
        time.sleep(duracao)
        return

    pwm.ChangeFrequency(freq)
    pwm.start(50)

    time.sleep(duracao * 0.88)

    pwm.stop()
    time.sleep(duracao * 0.12)


def marcha_espacial():

    musica = [

        # Parte 1 - pesada
        ("G4", 0.45),
        ("G4", 0.45),
        ("G4", 0.45),

        ("D4", 0.30),
        ("A4", 0.15),

        ("G4", 0.45),
        ("D4", 0.30),
        ("A4", 0.15),
        ("G4", 0.70),

        ("PAUSA", 0.20),

        # Parte 2
        ("D5", 0.45),
        ("D5", 0.45),
        ("D5", 0.45),

        ("E5", 0.30),
        ("A4", 0.15),

        ("F4", 0.45),
        ("D4", 0.30),
        ("A4", 0.15),
        ("G4", 0.70),

        ("PAUSA", 0.25),

        # Continuação original
        ("G5", 0.30),
        ("G4", 0.15),
        ("G4", 0.15),

        ("G5", 0.30),
        ("F5", 0.15),
        ("E5", 0.15),
        ("D5", 0.15),
        ("C5", 0.15),

        ("B4", 0.30),
        ("A4", 0.15),
        ("G4", 0.15),

        ("D5", 0.40),
        ("A4", 0.20),

        ("PAUSA", 0.15),

        # Resposta
        ("E5", 0.25),
        ("D5", 0.25),
        ("C5", 0.25),

        ("B4", 0.25),
        ("A4", 0.25),

        ("G4", 0.50),

        ("PAUSA", 0.15),

        # Crescimento
        ("G4", 0.20),
        ("A4", 0.20),
        ("B4", 0.20),
        ("C5", 0.20),

        ("D5", 0.40),

        ("C5", 0.20),
        ("B4", 0.20),
        ("A4", 0.20),

        ("G4", 0.50),

        ("PAUSA", 0.20),

        # Seção grave
        ("D4", 0.35),
        ("D4", 0.35),

        ("G4", 0.25),
        ("F4", 0.25),
        ("E4", 0.25),

        ("D4", 0.50),

        ("PAUSA", 0.20),

        ("A4", 0.25),
        ("G4", 0.25),
        ("F4", 0.25),

        ("E4", 0.25),
        ("D4", 0.50),

        ("PAUSA", 0.25),

        # Final mais dramático
        ("G4", 0.35),
        ("G4", 0.35),
        ("D5", 0.35),

        ("C5", 0.20),
        ("B4", 0.20),
        ("A4", 0.20),

        ("G4", 0.45),

        ("D4", 0.25),
        ("A4", 0.25),

        ("G4", 0.70),

        ("PAUSA", 0.25),

        ("G5", 0.20),
        ("F5", 0.20),
        ("E5", 0.20),
        ("D5", 0.20),

        ("C5", 0.30),
        ("B4", 0.30),
        ("A4", 0.30),

        ("G4", 1.00),
    ]

    for nota, duracao in musica:
        tocar_nota(nota, duracao)


try:
    marcha_espacial()

finally:
    pwm.stop()
    GPIO.cleanup()