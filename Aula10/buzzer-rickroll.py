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

    "PAUSA": 0
}


def tocar(nota, duracao):
    freq = NOTAS[nota]

    if freq == 0:
        pwm.stop()
        time.sleep(duracao)
        return

    pwm.ChangeFrequency(freq)
    pwm.start(50)

    time.sleep(duracao * 0.85)

    pwm.stop()
    time.sleep(duracao * 0.15)


def rickroll():
    # Trecho curto reconhecível, seguido de continuação original
    musica = [
        ("D5", 0.20),
        ("E5", 0.20),
        ("G5", 0.20),
        ("E5", 0.20),

        ("B4", 0.30),
        ("B4", 0.20),
        ("A4", 0.30),

        ("PAUSA", 0.15),

        ("D5", 0.20),
        ("E5", 0.20),
        ("G5", 0.20),
        ("E5", 0.20),

        ("A4", 0.30),
        ("A4", 0.20),
        ("G4", 0.35),

        # continuação original no mesmo clima
        ("B4", 0.20),
        ("D5", 0.20),
        ("E5", 0.20),
        ("G5", 0.30),

        ("E5", 0.20),
        ("D5", 0.20),
        ("B4", 0.30),

        ("A4", 0.20),
        ("B4", 0.20),
        ("D5", 0.20),
        ("E5", 0.30),

        ("D5", 0.20),
        ("B4", 0.20),
        ("A4", 0.35),
    ]

    for nota, duracao in musica:
        tocar(nota, duracao)


try:
    rickroll()

finally:
    pwm.stop()
    GPIO.cleanup()