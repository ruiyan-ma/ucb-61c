.globl relu

.text
# ==============================================================================
# FUNCTION: Performs an inplace element-wise ReLU on an array of ints
# Arguments:
# 	a0 (int*) is the pointer to the array
#	a1 (int)  is the # of elements in the array
# Returns:
#	None
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 78.
# ==============================================================================
relu:
    # check array length
    li t0, 1
    blt a1, t0, error

    li t0, 0  # counter
loop_start:
    beq t0, a1, loop_end
    lw t1, 0(a0)

    # jump to continue if positive
    bge t1, x0, loop_continue

    # set negative value to 0
    sw x0, 0(a0)

loop_continue:
    addi a0, a0, 4
    addi t0, t0, 1
    jal x0, loop_start

loop_end:
    ret

error:
    li a1, 78
    j exit2
