# =====================================================
# Calculadora Binaria - RISC-V (RV32I base, CPUlator)
# Operacoes: soma, subtracao, multiplicacao, fatorial, divisao
# RV32I base nao tem MUL/DIV nativos (dependem da extensao "M"),
# entao multiplicacao e divisao sao feitas manualmente,
# igual ao que foi feito no C do ESP32.
# Codigo gerado com apoio de IA generativa -- validar cada
# resultado no simulador antes de usar como evidencia (RNF03).
# =====================================================

.data
op_a:           .word 7      # Operando A (exemplo, 4 bits: 0-15)
op_b:           .word 5      # Operando B (exemplo, 4 bits: 0-15)
res_soma:       .word 0
res_sub:        .word 0
res_mul:        .word 0
res_fat:        .word 0
res_div:        .word 0
erro_div:       .word 0      # 1 = divisao por zero
overflow_fat:   .word 0      # 1 = fatorial excedeu 4 bits (>15)

.text
.globl _start

_start:
    la   t0, op_a
    lw   a0, 0(t0)            # a0 = A
    la   t0, op_b
    lw   a1, 0(t0)            # a1 = B

    # ---- SOMA ----
    add  t1, a0, a1
    la   t2, res_soma
    sw   t1, 0(t2)

    # ---- SUBTRACAO ----
    sub  t1, a0, a1
    la   t2, res_sub
    sw   t1, 0(t2)

    # ---- MULTIPLICACAO (shift + soma, RV32I base nao tem MUL) ----
    jal  ra, multiplicar
    la   t2, res_mul
    sw   t1, 0(t2)

    # ---- FATORIAL de A ----
    jal  ra, fatorial
    la   t2, res_fat
    sw   t1, 0(t2)

    # ---- DIVISAO (subtracoes sucessivas, A / B) ----
    jal  ra, dividir
    la   t2, res_div
    sw   t1, 0(t2)

fim:
    j    fim

# -----------------------------------------------------
# multiplicar: t1 = a0 * a1, via deslocamento e soma
# usa t3 (multiplicando), t4 (multiplicador), t5 (bit testado)
# -----------------------------------------------------
multiplicar:
    mv   t1, zero              # resultado = 0
    mv   t3, a0                 # multiplicando
    mv   t4, a1                  # multiplicador

mult_loop:
    beq  t4, zero, mult_fim
    andi t5, t4, 1               # testa bit 0 do multiplicador
    beq  t5, zero, mult_skip
    add  t1, t1, t3
mult_skip:
    slli t3, t3, 1
    srli t4, t4, 1
    j    mult_loop
mult_fim:
    ret

# -----------------------------------------------------
# fatorial: t1 = a0!  (assume a0 >= 0)
# reaproveita a rotina "multiplicar" a cada iteracao
# -----------------------------------------------------
fatorial:
    blt  a0, zero, fat_erro

    mv   s1, a0                  # guarda A original
    mv   s2, ra                   # guarda endereco de retorno
    addi t1, zero, 1              # resultado = 1
    addi t6, zero, 2               # contador = 2

fat_loop:
    bgt  t6, s1, fat_fim
    mv   a0, t1                    # prepara chamada: multiplicar(t1, t6)
    mv   a1, t6
    jal  ra, multiplicar
    li   t5, 15
    blt  t5, t1, fat_overflow
    j    fat_sem_overflow
fat_overflow:
    la   t2, overflow_fat
    li   t5, 1
    sw   t5, 0(t2)
fat_sem_overflow:
    addi t6, t6, 1
    j    fat_loop

fat_fim:
    mv   ra, s2
    ret

fat_erro:
    mv   t1, zero
    ret

# -----------------------------------------------------
# dividir: quociente inteiro de a0 / a1 em t1
# via subtracoes sucessivas
# -----------------------------------------------------
dividir:
    beq  a1, zero, div_erro
    mv   t1, zero                 # quociente = 0
    mv   t3, a0                    # resto = A

div_loop:
    blt  t3, a1, div_fim
    sub  t3, t3, a1
    addi t1, t1, 1
    j    div_loop
div_fim:
    ret

div_erro:
    la   t2, erro_div
    li   t5, 1
    sw   t5, 0(t2)
    mv   t1, zero
    ret
