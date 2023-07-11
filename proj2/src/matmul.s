.globl matmul

.text
# =======================================================
# FUNCTION: Matrix Multiplication of 2 integer matrices
# 	d = matmul(m0, m1)
# Arguments:
# 	a0 (int*)  is the pointer to the start of m0 
#	a1 (int)   is the # of rows (height) of m0
#	a2 (int)   is the # of columns (width) of m0
#	a3 (int*)  is the pointer to the start of m1
# 	a4 (int)   is the # of rows (height) of m1
#	a5 (int)   is the # of columns (width) of m1
#	a6 (int*)  is the pointer to the the start of d
# Returns:
#	None (void), sets d = matmul(m0, m1)
# Exceptions:
#   Make sure to check in top to bottom order!
#   - If the dimensions of m0 do not make sense,
#     this function terminates the program with exit code 72.
#   - If the dimensions of m1 do not make sense,
#     this function terminates the program with exit code 73.
#   - If the dimensions of m0 and m1 don't match,
#     this function terminates the program with exit code 74.
# =======================================================
matmul:
    li t0, 1
    # Error checks
    blt a1, t0, m0_dimension_error
    blt a2, t0, m0_dimension_error

    blt a4, t0, m1_dimension_error
    blt a5, t0, m1_dimension_error

    bne a2, a4, mismatch_error

    # Prologue
    addi sp, sp, -32
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)
    sw s6, 28(sp)

    # save all parameters
    mv s0, a0  # m0
    mv s1, a3  # m1
    mv s2, a6  # md
    mv s3, a1  # the number of rows (height) of m0
    mv s4, a2  # the number of columns (width) of m0
    mv s5, a4  # the number of rows (height) of m1
    mv s6, a5  # the number of columns (width) of m1

    li t0, 0  # i = 0
outer_loop_start:
    beq t0, s3, outer_loop_end
    li t1, 0  # j = 0

inner_loop_start:
    beq t1, s6, inner_loop_end

    # set a0 = array0
    li t2, 4
    mul t2, t2, s4
    mul t2, t2, t0
    add a0, s0, t2

    # set a1 = array1
    li t2 ,4
    mul t2, t2, t1
    add a1, s1, t2

    # set parameters for dot
    addi a2, s4, 0  # a2 = length
    addi a3, x0, 1  # a3 = stride of array0 = 1
    addi a4, s6, 0  # a4 = stride of array1 = width of m1

    # save t0, t1, t2
    addi sp, sp, -12
    sw t0, 0(sp)
    sw t1, 4(sp)
    sw t2, 8(sp)

    # call dot
    jal dot

    # restore t0, t1, t2
    lw t0, 0(sp)
    lw t1, 4(sp)
    lw t2, 8(sp)
    addi sp, sp, 12

    # store md[i][j]
    sw a0, 0(s2)

    # move md
    addi s2, s2, 4

    # j = j + 1
    addi t1, t1, 1

    j inner_loop_start

inner_loop_end:
    # i = i + 1
    addi t0, t0, 1

    j outer_loop_start

outer_loop_end:
    # Epilogue
    lw ra, 0(sp)
    lw s0, 4(sp)
    lw s1, 8(sp)
    lw s2, 12(sp)
    lw s3, 16(sp)
    lw s4, 20(sp)
    lw s5, 24(sp)
    lw s6, 28(sp)
    addi sp, sp, 32

    ret

m0_dimension_error:
    li a1, 72
    j exit2

m1_dimension_error:
    li a1, 73
    j exit2

mismatch_error:
    li a1, 74
    j exit2

