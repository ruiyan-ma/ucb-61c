.globl dot

.text
# =======================================================
# FUNCTION: Dot product of 2 int vectors
# Arguments:
#   a0 (int*) is the pointer to the start of v0
#   a1 (int*) is the pointer to the start of v1
#   a2 (int)  is the length of the vectors
#   a3 (int)  is the stride of v0
#   a4 (int)  is the stride of v1
# Returns:
#   a0 (int)  is the dot product of v0 and v1
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 75.
# - If the stride of either vector is less than 1,
#   this function terminates the program with error code 76.
# =======================================================
dot:
    li t0, 1
    # check array length
    blt a2, t0, length_error

    # check stride
    blt a3, t0, stride_error
    blt a4, t0, stride_error

    # Prologue
    addi sp, sp, -4
    sw s0, 0(sp)

    # stride in bytes
    slli a3, a3, 2
    slli a4, a4, 2
    
    li t0, 0  # i = 0
    li s0, 0  # result = 0
loop_start:
    beq t0, a2, loop_end

    lw t1, 0(a0)
    lw t2, 0(a1)

    mul t3, t1, t2
    add s0, s0, t3

    # move pointer by 4 * stride
    add a0, a0, a3
    add a1, a1, a4

    addi t0, t0, 1
    jal x0, loop_start

loop_end:
    addi a0, s0, 0

    # Epilogue
    lw s0, 0(sp)
    addi sp, sp, 4

    ret

length_error:
    li a1, 75
    j exit2

stride_error:
    li a1, 76
    j exit2
