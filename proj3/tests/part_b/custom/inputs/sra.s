addi t0 x0 1
addi t1 x0 -1
addi t2 x0 2

sra s0 t2 t0
sra s1 t1 t0
sra s2 t2 t1

sra a0 s1 s2
sra ra s2 s1
