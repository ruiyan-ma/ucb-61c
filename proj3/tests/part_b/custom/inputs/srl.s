addi t0 x0 1
addi t1 x0 -1
addi t2 x0 2

srl s0 t2 t0
srl s1 t1 t0
srl s2 t2 t1

srl a0 s1 s2
srl ra s2 s1
