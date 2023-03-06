addi t0 x0 1
addi t1 x0 2

beq t0 t0 label
addi t3 t0 1
addi t4 t1 1

label:
beq t0 t1 end
addi t3 t0 2
addi t4 t1 2

end:
