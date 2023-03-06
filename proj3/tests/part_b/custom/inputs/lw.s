addi a0 x0 -7
sw a0 40(x0)
lw s0 40(x0)

addi a0 a0 1656
sw a0 60(x0)
lw s0 60(x0)

addi t0 x0 1234
sb t0 80(x0)
lb t1 80(x0)
