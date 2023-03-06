addi t0 x0 -1
addi t1 x0 1024
addi t2 x0 17

mulhu s0 t0 t0
mulhu s1 t1 t0
mulhu s2 t2 t1

mulhu a0 s2 s1
mulhu sp s2 s1
mulhu ra s2 s1
