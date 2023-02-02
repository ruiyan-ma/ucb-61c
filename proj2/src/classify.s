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
    addi sp, sp, 0
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    
    # save arguments
    mv s0, a0  # argc
    mv s1, a1  # argv
    mv s2, a2  # print flag

    # check arguments
    jal check_arguments

	# =====================================
    # LOAD MATRICES
    # =====================================

    # allocate memory for m0.rows and m0.cols
    li a0, 8  # bytes to allocate
    jal malloc
    jal check_malloc_error
    mv s3, a0  # pointer to (m0.rows, m0.cols)

    # load pretrained m0
    lw a0, 4(s1)  # pointer to filename
    addi a1, s3, 0  # pointer to row number
    addi a2, s3, 4  # pointer to column number
    jal read_matrix
    mv s4, a0  # pointer to m0

    # allocate memory for m1.rows and m1.cols
    li a0, 8  # bytes to allocate
    jal malloc
    jal check_malloc_error
    mv s5, a0  # pointer to (m0.rows, m0.cols)

    # load pretrained m1
    lw a0, 8(s1)  # pointer to filename
    addi a1, s5, 0  # pointer to row number
    addi a2, s5, 4  # pointer to column number
    jal read_matrix
    mv s6, a0  # pointer to m0

    # allocate memory for input.rows and input.cols
    li a0, 8  # bytes to allocate
    jal malloc
    jal check_malloc_error
    mv s7, a0  # pointer to (m0.rows, m0.cols)

    # load input matrix
    lw a0, 12(s1)  # pointer to filename
    addi a1, s7, 0  # pointer to row number
    addi a2, s7, 4  # pointer to column number
    jal read_matrix
    mv s8, a0  # pointer to m0
   

    # =====================================
    # RUN LAYERS
    # =====================================
    # 1. LINEAR LAYER:    m0 * input
    # 2. NONLINEAR LAYER: ReLU(m0 * input)
    # 3. LINEAR LAYER:    m1 * ReLU(m0 * input)


    # =====================================
    # WRITE OUTPUT
    # =====================================
    # Write output matrix

    # =====================================
    # CALCULATE CLASSIFICATION/LABEL
    # =====================================
    # Call argmax

    # Print classification

    # Print newline afterwards for clarity

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
    mv s1, a0  # pointer to m0
 
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

