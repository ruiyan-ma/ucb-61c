addi t0 x0 -1
addi t1 x0 2

bgeu t0 t1 label
addi t3 t0 1
addi t4 t1 1

label:
bgeu t0 t0 label2
addi t3 t0 2
addi t4 t1 2

label2:
bgeu t1 t0 end
addi s0 x0 1

end:

