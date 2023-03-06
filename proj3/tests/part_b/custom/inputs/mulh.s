addi t0 x0 -1
addi t1 x0 1024
addi t2 x0 17

mulh s0 t0 t0
mulh s1 t1 t0
mulh s2 t2 t1

mulh a0 s2 s1
mulh sp s2 s1
mulh ra s2 s1
