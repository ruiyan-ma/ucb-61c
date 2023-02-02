.globl read_matrix

.text
# ==============================================================================
# FUNCTION: Allocates memory and reads in a binary file as a matrix of integers
#
# FILE FORMAT:
#   The first 8 bytes are two 4 byte ints representing the # of rows and columns
#   in the matrix. Every 4 bytes afterwards is an element of the matrix in
#   row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is a pointer to an integer, we will set it to the number of rows
#   a2 (int*)  is a pointer to an integer, we will set it to the number of columns
# Returns:
#   a0 (int*)  is the pointer to the matrix in memory
# Exceptions:
# - If malloc returns an error,
#   this function terminates the program with error code 88.
# - If you receive an fopen error or eof, 
#   this function terminates the program with error code 90.
# - If you receive an fread error or eof,
#   this function terminates the program with error code 91.
# - If you receive an fclose error or eof,
#   this function terminates the program with error code 92.
# ==============================================================================
read_matrix:

    # Prologue
    addi sp, sp, -28
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)

    # save arguments
    mv s0, a0  # pointer to filename
    mv s1, a1  # pointer for row number
    mv s2, a2  # pointer for column number

    # open file
    mv a1, s0  # filepath
    li a2, 0  # permission: read only
    jal fopen
    jal check_fopen_error
    # save file descriptor
    mv s0, a0

    # read row number
    mv a1, s0  # file descriptor
    mv a2, s1  # write the read bytes to s1
    li a3, 4  # number of bytes to be read
    jal fread
    jal check_fread_error

    # read column number
    mv a1, s0  # file descriptor
    mv a2, s2  # write the read bytes to s2
    li a3, 4  # number of bytes to be read
    jal fread
    jal check_fread_error

    # allocate memory
    lw t0, 0(s1)  # row number
    lw t1, 0(s2)  # column number
    mul s3, t0, t1  # element number
    slli a0, s3, 2
    jal malloc
    jal check_malloc_error
    # save memory pointer
    mv s4, a0

    # read matrix
    li s5, 0  # i = 0
read_loop:
    beq s5, s3, loop_end

    # set current memory pointer
    slli t0, s5, 2
    add t0, t0, s4

    # read next element
    mv a1, s0  # file descriptor
    mv a2, t0  # write the read bytes to t0
    li a3, 4  # number of bytes to be read
    jal fread
    jal check_fread_error

    # i += 1
    addi s5, s5, 1

    j read_loop

loop_end:
    # close file
    mv a1, s0  # file descriptor
    jal fclose
    jal check_fclose_error

    # set return value
    mv a0, s4

    # Epilogue
    lw ra, 0(sp)
    lw s0, 4(sp)
    lw s1, 8(sp)
    lw s2, 12(sp)
    lw s3, 16(sp)
    lw s4, 20(sp)
    lw s5, 24(sp)
    addi sp, sp, 28

    ret

check_malloc_error:
    li a1, 88
    beq a0, x0, exit_with_error
    ret

check_fopen_error:
    li t0, -1
    li a1, 90
    beq a0, t0, exit_with_error
    ret

check_fread_error:
    li t0, 4
    li a1, 91
    bne a0, t0, exit_with_error
    ret

check_fclose_error:
    li a1, 92
    bne a0, x0, exit_with_error
    ret

exit_with_error:
    j exit2
