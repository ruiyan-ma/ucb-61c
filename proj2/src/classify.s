.globl classify

.text
classify:
    # =====================================
    # COMMAND LINE ARGUMENTS
    # =====================================
    # Args:
    #   a0 (int)    argc
    #   a1 (char**) argv
    #   a2 (int)    print_classification, if this is zero, 
    #               you should print the classification. Otherwise,
    #               this function should not print ANYTHING.
    # Returns:
    #   a0 (int)    Classification
    # Exceptions:
    # - If there are an incorrect number of command line args,
    #   this function terminates the program with exit code 89.
    # - If malloc fails, this function terminats the program with exit code 88.
    #
    # Usage:
    #   main.s <M0_PATH> <M1_PATH> <INPUT_PATH> <OUTPUT_PATH>

    # prologue
    addi sp, sp, -36
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)
    sw s6, 28(sp)
    sw s7, 32(sp)
    
    # save arguments
    mv s0, a1  # argv
    mv s1, a2  # print flag

    # check arguments
    jal check_arguments


	# =====================================
    # LOAD MATRICES
    # =====================================

    # load pretrained m0
    lw a1, 4(s0)
    jal read_matrix_info
    mv s2, a0  # (rows, cols), m0

    # load pretrained m1
    lw a1, 8(s0)
    jal read_matrix_info
    mv s3, a0  # (rows, cols), m1

    # load input matrix
    lw a1, 12(s0)
    jal read_matrix_info
    mv s4, a0  # (rows, cols), input


    # =====================================
    # RUN LAYERS
    # =====================================
    # 1. LINEAR LAYER:    m0 * input
    mv a0, s2
    mv a1, s4
    jal mult_matrix
    mv s5, a0  # (rows, cols), m0 * input

    # 2. NONLINEAR LAYER: ReLU(m0 * input)
    lw t0, 0(s5)
    lw t1, 0(t0)  # rows
    lw t2, 4(t0)  # cols
    lw a0, 4(s5)  # m0 * input
    mul a1, t1, t2  # rows * cols
    jal relu

    # 3. LINEAR LAYER:    m1 * ReLU(m0 * input)
    mv a0, s3  # m1
    mv a1, s5  # ReLU(m0 * input)
    jal mult_matrix
    mv s6, a0  # (rows, cols), m1 * ReLU(m0 * input)


    # =====================================
    # WRITE OUTPUT
    # =====================================
    # write output matrix
	lw a0, 16(s0)  # output filename
    lw a1, 4(s6)  # matrix
    lw t0, 0(s6)
    lw a2, 0(t0)  # rows
    lw a3, 4(t0)  # cols
    jal write_matrix


    # =====================================
    # CALCULATE CLASSIFICATION/LABEL
    # =====================================
    # call argmax
    lw a0, 4(s6)
    lw t0, 0(s6)
    lw t1, 0(t0)  # rows
    lw t2, 4(t0)  # cols
    mul a1, t1, t2
    jal argmax
    mv s7, a0
   
    bne s1, x0, not_print
    # print classification
    mv a1, s7
    jal print_int

    # print newline afterwards for clarity
	li a1 '\n'
    jal print_char

not_print:
    # free memory
    mv a1, s2
    jal free_matrix
    mv a1, s3
    jal free_matrix
    mv a1, s4
    jal free_matrix
    mv a1, s5
    jal free_matrix
    mv a1, s6
    jal free_matrix

    # set return value
    mv a0, s7

    # epilogue
    lw ra, 0(sp)
    lw s0, 4(sp)
    lw s1, 8(sp)
    lw s2, 12(sp)
    lw s3, 16(sp)
    lw s4, 20(sp)
    lw s5, 24(sp)
    lw s6, 28(sp)
    lw s7, 32(sp)
    addi sp, sp, 36

    ret


#================================================================
# int** read_matrix_info(char* a1)
# Allocate memory for rows and cols, and load matrix.
# args:
#   a1 = pointer to filename
# return:
#   a0 = (pointer to (rows, cols), pointer to matrix)
#================================================================
read_matrix_info:
    # prologue
    addi sp, sp, -12
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)

    # save argument
    mv s1, a1

    # allocate memory for rows and cols
    li a0, 8  # bytes to allocate
    jal malloc
    jal check_malloc_error
    mv s0, a0  # pointer to (rows, cols)

    # load matrix
    mv a0, s1  # pointer to filename
    addi a1, s0, 0  # pointer to row number
    addi a2, s0, 4  # pointer to column number
    jal read_matrix
    mv s1, a0  # pointer to matrix
 
    # allocate memory for ((rows, cols), matrix)
    li a0, 8
    jal malloc
    jal check_malloc_error
    sw s0, 0(a0)
    sw s1, 4(a0)

    # epilogue
    lw ra, 0(sp)
    lw s0, 4(sp)
    lw s1, 8(sp)
    addi sp, sp, 12

    ret


#================================================================
# int** mult_matrix(int** a0, int** a1)
# args:
#   a0: matrix 0
#   a1: matrix 1
# return:
#   a0: destination matrix
#================================================================
mult_matrix:
    # prologue
    addi sp, sp, -28
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)

    # save argument
    lw s0, 0(a0)  # m0 -> (rows, cols)
    lw s1, 4(a0)  # m0
    lw s2, 0(a1)  # m1 -> (rows, cols)
    lw s3, 4(a1)  # m1

    # allocate memory for md -> (rows, cols)
    li a0, 8
    jal malloc
    jal check_malloc_error
    mv s4, a0  # md -> (rows, cols)
    lw t0, 0(s0)  # m0.rows
    lw t1, 4(s2)  # m1.cols
    sw t0, 0(s4)
    sw t1, 4(s4)

    # allocate memory for md
    mul a0, t0, t1  # element number
    slli a0, a0, 2
    jal malloc
    jal check_malloc_error
    mv s5, a0  # md

    # multiply matrices
    mv a0, s1
    lw a1, 0(s0)
    lw a2, 4(s0)
    mv a3, s3
    lw a4, 0(s2)
    lw a5, 4(s2)
    mv a6, s5
    jal matmul

    # allocate memory for ((rows, cols), matrix)
    li a0, 8
    jal malloc
    jal check_malloc_error
    sw s4, 0(a0)
    sw s5, 4(a0)

    # epilogue
    lw ra, 0(sp)
    lw s0, 4(sp)
    lw s1, 8(sp)
    lw s2, 12(sp)
    lw s3, 16(sp)
    lw s4, 20(sp)
    lw s5, 24(sp)
    addi sp, sp, 28

    ret


#================================================================
# void free_matrix(int** a1)
# Free all memory of this matrix.
# args:
#   a1 = (pointer to (rows, cols), pointer to matrix)
# return:
#   None
#================================================================
free_matrix:
    # prologue
    addi, sp, sp, -8
    sw ra, 0(sp)
    sw s0, 4(sp)

    # save argument
    mv s0, a1

    # free (row, col)
    lw a0, 0(s0)
    jal free

    # free matrix
    lw a0, 4(s0)
    jal free

    # free s0
    mv a0, s0
    jal free
    
    # epilogue
    lw ra, 0(sp)
    lw s0, 4(sp)
    addi, sp, sp, 8

    ret


check_arguments:
    li t0, 5
    li a1, 89
    bne a0, t0, exit_with_error
    ret


check_malloc_error:
    li a1, 88
    beq a0, x0, exit_with_error
    ret


exit_with_error:
    j exit2

