import RPi.GPIO as GPIO
import time

BUZZER = 4

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(BUZZER, GPIO.OUT)

pwm = GPIO.PWM(BUZZER, 440)

NOTAS = {
    "A4": 440.00,
    "Bb4": 466.16,
    "B4": 493.88,
    "C5": 523.25,
    "C#5": 554.37,
    "D5": 587.33,
    "E5": 659.25,
    "F5": 698.46,
    "F#5": 739.99,
    "G5": 783.99,
    "A5": 880.00,
    "C6": 1046.50,
    "D6": 1174.66,
    "E6": 1318.51,
}

def nota(nome, duracao, pausa=0.015):
    freq = NOTAS[nome]

    pwm.ChangeFrequency(freq)
    pwm.start(35)

    time.sleep(duracao)

    pwm.stop()
    time.sleep(pausa)


def bend(freq_inicio, freq_fim, duracao, passos=25):
    pwm.start(35)

    intervalo = duracao / passos

    for i in range(passos):
        freq = freq_inicio + (freq_fim - freq_inicio) * i / (passos - 1)
        pwm.ChangeFrequency(freq)
        time.sleep(intervalo)

    pwm.stop()
    time.sleep(0.02)


def solo():

    # Frase rápida em A pentatônica
    nota("A4", 0.10)
    nota("C5", 0.10)
    nota("D5", 0.10)
    nota("E5", 0.12)

    nota("D5", 0.08)
    nota("C5", 0.08)
    nota("A4", 0.12)

    # Subida
    nota("C5", 0.08)
    nota("D5", 0.08)
    nota("E5", 0.08)
    nota("G5", 0.08)
    nota("A5", 0.18)

    # Bend A -> B
    bend(880, 988, 0.25)

    nota("A5", 0.10)
    nota("G5", 0.10)
    nota("E5", 0.10)

    # frase rápida
    for _ in range(2):
        nota("D5", 0.065)
        nota("E5", 0.065)
        nota("G5", 0.065)
        nota("A5", 0.065)

    # região aguda
    nota("C6", 0.10)
    nota("D6", 0.10)

    bend(1174.66, 1318.51, 0.30)

    nota("D6", 0.09)
    nota("C6", 0.09)
    nota("A5", 0.09)
    nota("G5", 0.09)

    # lick descendente
    nota("E5", 0.07)
    nota("D5", 0.07)
    nota("C5", 0.07)
    nota("A4", 0.14)

    # repetição acelerada
    for _ in range(4):
        nota("E5", 0.055)
        nota("G5", 0.055)
        nota("A5", 0.055)
        nota("G5", 0.055)

    # bend final
    nota("A5", 0.12)
    bend(880, 1046, 0.35)

    nota("A5", 0.10)
    nota("G5", 0.10)
    nota("E5", 0.15)

    nota("D5", 0.10)
    nota("C5", 0.10)
    nota("A4", 0.45)


try:
    solo()

except KeyboardInterrupt:
    pass

finally:
    pwm.stop()
    GPIO.cleanup()