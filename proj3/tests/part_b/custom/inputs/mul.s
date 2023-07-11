addi t0 x0 1
addi t1 x0 2
addi t2 x0 -2

mul s0 t1 t0
mul s1 t2 t1
mul s2 s1 s1

mul a0 s2 s1
mul sp t2 s1
mul ra s2 s1
