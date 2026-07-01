import time
import statistics


def ler_inteiro(mensagem):
    while True:
        try:
            return int(input(mensagem))
        except ValueError:
            print("Entrada inválida.")


def limite_com_sinal(bits):
    minimo = -(2 ** (bits - 1))
    maximo = (2 ** (bits - 1)) - 1
    return minimo, maximo


def tem_overflow(resultado, bits):
    minimo, maximo = limite_com_sinal(bits)
    return resultado < minimo or resultado > maximo


def decimal_para_complemento_2(valor, bits):
    mascara = (1 << bits) - 1
    return format(valor & mascara, f"0{bits}b")


def ler_bits():
    while True:
        bits = ler_inteiro("Bits: ")
        if bits > 0:
            return bits
        print("Valor inválido.")


def ler_numero(nome, bits):
    minimo, maximo = limite_com_sinal(bits)

    while True:
        valor = ler_inteiro(f"{nome} [{minimo} a {maximo}]: ")
        if minimo <= valor <= maximo:
            return valor
        print("Valor fora do intervalo em complemento de 2.")


def soma(a, b):
    return a + b


def subtracao(a, b):
    return a - b


def multiplicacao(a, b):
    return a * b


def divisao(a, b):
    if b == 0:
        return None
    return a // b


def fatorial(n):
    if n < 0:
        return None

    resultado = 1

    for i in range(2, n + 1):
        resultado *= i

    return resultado


def operacao_precisa_de_b(op):
    return op in ["1", "2", "3", "5"]


def nome_operacao(op):
    if op == "1":
        return "Soma"
    if op == "2":
        return "Subtração"
    if op == "3":
        return "Multiplicação"
    if op == "4":
        return "Fatorial"
    if op == "5":
        return "Divisão"
    return "Inválida"


def executar_operacao(op, a, b=None):
    if op == "1":
        return soma(a, b)
    if op == "2":
        return subtracao(a, b)
    if op == "3":
        return multiplicacao(a, b)
    if op == "4":
        return fatorial(a)
    if op == "5":
        return divisao(a, b)
    return None


def mostrar_resultado(resultado, bits, op):
    if resultado is None:
        if op == "4":
            print("Erro: fatorial de número negativo não é permitido.")
        else:
            print("Erro: divisão por zero.")
        return

    minimo, maximo = limite_com_sinal(bits)
    binario = decimal_para_complemento_2(resultado, bits)

    print("Decimal:", resultado)
    print(f"Binário em complemento de 2 ({bits} bits):", binario)

    if tem_overflow(resultado, bits):
        print(f"Aviso: overflow para {bits} bits em complemento de 2.")
        print(f"Intervalo representável: {minimo} até {maximo}.")


def menu_operacoes():
    print()
    print("1 - Soma")
    print("2 - Subtração")
    print("3 - Multiplicação")
    print("4 - Fatorial")
    print("5 - Divisão")
    print("0 - Voltar")

    return input("Operação: ")


def modo_calculadora(bits_fixos=None):
    if bits_fixos is None:
        bits = ler_bits()
    else:
        bits = bits_fixos

    minimo, maximo = limite_com_sinal(bits)

    print()
    print(f"Modo {bits} bits em complemento de 2")
    print(f"Entradas permitidas: {minimo} até {maximo}")

    while True:
        op = menu_operacoes()

        if op == "0":
            break

        if op not in ["1", "2", "3", "4", "5"]:
            print("Operação inválida.")
            continue

        a = ler_numero("A", bits)
        b = None

        if operacao_precisa_de_b(op):
            b = ler_numero("B", bits)

        resultado = executar_operacao(op, a, b)

        print()
        print(nome_operacao(op))
        mostrar_resultado(resultado, bits, op)


def benchmark_operacao(op, bits, repeticoes):
    minimo, maximo = limite_com_sinal(bits)

    a = maximo
    b = maximo - 1

    if b == 0:
        b = 1

    tempos = []

    for _ in range(repeticoes):
        inicio = time.perf_counter()
        executar_operacao(op, a, b)
        fim = time.perf_counter()

        tempos.append(fim - inicio)

    media = statistics.mean(tempos)

    if repeticoes > 1:
        desvio = statistics.stdev(tempos)
    else:
        desvio = 0.0

    return media, desvio


def modo_benchmark():
    bits_inicial = ler_inteiro("Bits iniciais: ")
    bits_final = ler_inteiro("Bits finais: ")
    passo = ler_inteiro("Passo: ")
    repeticoes = ler_inteiro("Repetições: ")

    if bits_inicial <= 0:
        print("Valor inválido.")
        return

    if bits_final < bits_inicial:
        print("Valor inválido.")
        return

    if passo <= 0:
        print("Valor inválido.")
        return

    if repeticoes <= 0:
        print("Valor inválido.")
        return

    print()
    print("bits;operacao;media;desvio_padrao")

    bits = bits_inicial

    while bits <= bits_final:
        for op in ["1", "2", "3", "4", "5"]:
            media, desvio = benchmark_operacao(op, bits, repeticoes)
            print(f"{bits};{nome_operacao(op)};{media:.10f};{desvio:.10f}")

        bits += passo


def menu_principal():
    while True:
        print()
        print("Calculadora Binária - Raspberry Pi 3")
        print("1 - Teste 4 bits")
        print("2 - Calculadora N bits")
        print("3 - Benchmark")
        print("0 - Sair")

        escolha = input("Escolha: ")

        if escolha == "1":
            modo_calculadora(4)
        elif escolha == "2":
            modo_calculadora()
        elif escolha == "3":
            modo_benchmark()
        elif escolha == "0":
            break
        else:
            print("Opção inválida.")


if __name__ == "__main__":
    menu_principal()