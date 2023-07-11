.globl argmax

.text
# =================================================================
# FUNCTION: Given a int vector, return the index of the largest
#	element. If there are multiple, return the one
#	with the smallest index.
# Arguments:
# 	a0 (int*) is the pointer to the start of the vector
#	a1 (int)  is the # of elements in the vector
# Returns:
#	a0 (int)  is the first index of the largest element
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 77.
# =================================================================
argmax:
    # check array length
    li t0, 1
    blt a1, t0, error

    # Prologue
    addi sp, sp, -8
    sw s0, 0(sp)
    sw s1, 4(sp)

    li t0, 0  # i = 0
    li s0, 0  # current max index
    lw s1, 0(a0)  # current max value
loop_start:
    beq t0, a1, loop_end
    lw t1, 0(a0)  # arr[i]

    # if s1 >= t1, jump to continue
    bge s1, t1, loop_continue

    # set s0 = t0
    addi s0, t0, 0
    # set s1 = t1
    addi s1, t1, 0

loop_continue:
    addi a0, a0, 4
    addi t0, t0, 1
    jal x0, loop_start

loop_end:
    addi a0, s0, 0
    # Epilogue
    lw s0, 0(sp)
    lw s1, 4(sp)
    addi sp, sp, 8
    ret

error:
    li a1, 77
    jal x0, exit2
