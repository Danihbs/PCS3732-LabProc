@ =====================================================
@ Calculadora Binaria - ARM (ARMv7, sintaxe GNU / CPUlator)
@ Operacoes: soma, subtracao, multiplicacao, fatorial, divisao
@ Codigo gerado com apoio de IA generativa -- validar cada
@ resultado no simulador antes de usar como evidencia (RNF03).
@ =====================================================

.syntax unified

.data
op_a:           .word 7     @ Operando A (exemplo, 4 bits: 0-15)
op_b:           .word 5     @ Operando B (exemplo, 4 bits: 0-15)
res_soma:       .word 0
res_sub:        .word 0
res_mul:        .word 0
res_fat:        .word 0
res_div:        .word 0
erro_div:       .word 0     @ 1 = divisao por zero
overflow_fat:   .word 0     @ 1 = fatorial excedeu 4 bits (>15)

.text
.global _start

_start:
    LDR R0, =op_a
    LDR R0, [R0]            @ R0 = A
    LDR R1, =op_b
    LDR R1, [R1]            @ R1 = B

    @ ---- SOMA ----
    ADD R2, R0, R1
    LDR R3, =res_soma
    STR R2, [R3]

    @ ---- SUBTRACAO ----
    SUB R2, R0, R1
    LDR R3, =res_sub
    STR R2, [R3]

    @ ---- MULTIPLICACAO (instrucao nativa MUL do ARM) ----
    MUL R2, R0, R1
    LDR R3, =res_mul
    STR R2, [R3]

    @ ---- FATORIAL de A ----
    BL fatorial
    LDR R3, =res_fat
    STR R2, [R3]

    @ ---- DIVISAO (subtracoes sucessivas, A / B) ----
    BL dividir
    LDR R3, =res_div
    STR R2, [R3]

fim:
    B fim

@ -----------------------------------------------------
@ fatorial: recebe A em R0, devolve resultado em R2
@ usa a instrucao MUL nativa a cada iteracao
@ -----------------------------------------------------
fatorial:
    CMP R0, #0
    BLT fatorial_erro

    MOV R2, #1               @ resultado = 1
    MOV R4, #2                @ contador = 2

fatorial_loop:
    CMP R4, R0
    BGT fatorial_fim
    MUL R2, R2, R4
    CMP R2, #15
    BLE fatorial_sem_overflow
    LDR R6, =overflow_fat
    MOV R7, #1
    STR R7, [R6]
fatorial_sem_overflow:
    ADD R4, R4, #1
    B fatorial_loop

fatorial_fim:
    BX LR

fatorial_erro:
    MOV R2, #0
    BX LR

@ -----------------------------------------------------
@ dividir: A em R0, B em R1, quociente inteiro em R2
@ via subtracoes sucessivas
@ -----------------------------------------------------
dividir:
    CMP R1, #0
    BEQ dividir_erro

    MOV R2, #0                @ quociente = 0
    MOV R4, R0                 @ resto = A

dividir_loop:
    CMP R4, R1
    BLT dividir_fim
    SUB R4, R4, R1
    ADD R2, R2, #1
    B dividir_loop

dividir_fim:
    BX LR

dividir_erro:
    LDR R6, =erro_div
    MOV R7, #1
    STR R7, [R6]
    MOV R2, #0
    BX LR
