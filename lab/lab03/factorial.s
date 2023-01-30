.globl factorial

.data
n: .word 8

.text
main:
    la t0, n
    lw a0, 0(t0)
    jal ra, factorial

    addi a1, a0, 0
    addi a0, x0, 1
    ecall # Print Result

    addi a1, x0, '\n'
    addi a0, x0, 11
    ecall # Print newline

    addi a0, x0, 10
    ecall # Exit

factorial:
    addi t0, a0, 0  # i = n
loop:
    addi t0, t0, -1  # i -= 1
    beq t0, x0, finish  # if i == 0, done
    mul a0, a0, t0  # a0 *= i
    jal x0, loop  # jump to loop
finish:
    jr ra  # return
