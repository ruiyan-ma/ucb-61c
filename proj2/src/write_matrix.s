.globl write_matrix

.text
# ==============================================================================
# FUNCTION: Writes a matrix of integers into a binary file
# FILE FORMAT:
#   The first 8 bytes of the file will be two 4 byte ints representing the
#   numbers of rows and columns respectively. Every 4 bytes thereafter is an
#   element of the matrix in row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is the pointer to the start of the matrix in memory
#   a2 (int)   is the number of rows in the matrix
#   a3 (int)   is the number of columns in the matrix
# Returns:
#   None
# Exceptions:
# - If you receive an fopen error or eof,
#   this function terminates the program with error code 93.
# - If you receive an fwrite error or eof,
#   this function terminates the program with error code 94.
# - If you receive an fclose error or eof,
#   this function terminates the program with error code 95.
# ==============================================================================
write_matrix:

    # Prologue
    addi sp, sp, -24
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)

    # save arguments
    mv s0, a0  # pointer to filename
    mv s1, a1  # pointer to matrix
    mv s2, a2  # row number
    mv s3, a3  # column number

    # open file
    mv a1, s0  # filepath
    li a2, 1  # permission: write only
    jal fopen
    jal check_fopen_error
    # save file descriptor
    mv s0, a0

    # allocate memory for row number and column number
    li a0, 8  # bytes to allocate
    jal malloc
    mv s4, a0  # save the memory pointer
    sw s2, 0(s4)  # store row number
    sw s3, 4(s4)  # store column number

    # write row number and column number
    mv a1, s0  # file descriptor
    mv a2, s4  # buffer to read from
    li a3, 2  # number of items to read
    li, a4, 4  # size of each item
    jal fwrite
    # set argument for check fwrite error
    li a1, 2
    jal check_fwrite_error

    # write matrix
    mv a1, s0  # file descriptor
    mv a2, s1  # buffer to read from
    mul a3, s2, s3  # number of items to read
    li, a4, 4  # size of each item
    jal fwrite
    # set argument for check fwrite error
    mul a1, s2, s3
    jal check_fwrite_error

    # close file
    mv a1, s0
    jal fclose
    jal check_fclose_error

    # free memory
    mv a0, s4
    jal free

    # Epilogue
    lw ra, 0(sp)
    lw s0, 4(sp)
    lw s1, 8(sp)
    lw s2, 12(sp)
    lw s3, 16(sp)
    lw s4, 20(sp)
    addi sp, sp, 24

    ret

check_fopen_error:
    li t0, -1
    li a1, 93
    beq a0, t0, exit_with_error
    ret

check_fwrite_error:
    mv t0, a1
    li a1, 94
    bne a0, t0, exit_with_error
    ret

check_fclose_error:
    li a1, 95
    bne a0, x0, exit_with_error
    ret

exit_with_error:
    j exit2

