#include "matrix.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <immintrin.h>
#include <x86intrin.h>
#endif

/* Below are some intel intrinsics that might be useful
 * void _mm256_storeu_pd (double * mem_addr, __m256d a)
 * __m256d _mm256_set1_pd (double a)
 * __m256d _mm256_set_pd (double e3, double e2, double e1, double e0)
 * __m256d _mm256_loadu_pd (double const * mem_addr)
 * __m256d _mm256_add_pd (__m256d a, __m256d b)
 * __m256d _mm256_sub_pd (__m256d a, __m256d b)
 * __m256d _mm256_fmadd_pd (__m256d a, __m256d b, __m256d c)
 * __m256d _mm256_mul_pd (__m256d a, __m256d b)
 * __m256d _mm256_cmp_pd (__m256d a, __m256d b, const int imm8)
 * __m256d _mm256_and_pd (__m256d a, __m256d b)
 * __m256d _mm256_max_pd (__m256d a, __m256d b)
*/

/*
 * Generates a random double between `low` and `high`.
 */
double rand_double(double low, double high) {
    double range = (high - low);
    double div = RAND_MAX / range;
    return low + (rand() / div);
}

/*
 * Generates a random matrix with `seed`.
 */
void rand_matrix(matrix *result, unsigned int seed, double low, double high) {
    srand(seed);
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set(result, i, j, rand_double(low, high));
        }
    }
}

/*
 * Allocate space for a matrix struct pointed to by the double pointer mat with
 * `rows` rows and `cols` columns. You should also allocate memory for the data array
 * and initialize all entries to be zeros. Remember to set all fieds of the matrix struct.
 * `parent` should be set to NULL to indicate that this matrix is not a slice.
 * You should return -1 if either `rows` or `cols` or both have invalid values, or if any
 * call to allocate memory in this function fails. If you don't set python error messages here upon
 * failure, then remember to set it in numc.c.
 * Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix(matrix **mat, int rows, int cols) {
    // check if the dimensions are valid. 
    if (rows <= 0 || cols <= 0) {
        return -1;
    }

    // allocate space for the new matrix struct. 
    matrix *newMat = (matrix *) malloc(sizeof(matrix));
    if (newMat == NULL) {
        return -2;
    }

    // allocate space for the data array
    newMat->data = (double **) malloc(rows * sizeof(double*));
    if (newMat->data == NULL) {
        return -2;
    }

    for (int i = 0; i < rows; i++) {
        newMat->data[i] = (double *) calloc(cols, sizeof(double));
        if (newMat->data[i] == NULL) {
            return -2;
        }
    }

    // set is_1d field. 
    if (rows == 1 || cols == 1) {
        newMat->is_1d = 1;
    } else {
        newMat->is_1d = 0;
    }

    // set all other fields. 
    newMat->rows = rows;
    newMat->cols = cols;
    newMat->parent = NULL;
    newMat->ref_cnt = 1;

    // store the address of the new matrix in mat. 
    *mat = newMat;
    return 0;
}

/*
 * Allocate space for a matrix struct pointed to by `mat` with `rows` rows and `cols` columns.
 * This is equivalent to setting the new matrix to be
 * from[row_offset:row_offset + rows, col_offset:col_offset + cols]
 * If you don't set python error messages here upon failure, then remember to set it in numc.c.
 * Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix_ref(matrix **mat, matrix *from, int row_offset, int col_offset,
                        int rows, int cols) {
    // check if offsets are valid.
    if (row_offset < 0 || col_offset < 0) {
        return -1;
    }

    // check if the dimensions are valid.
    if (rows <= 0 || cols <= 0) {
        return -1;
    }

    // check new matrix out of bound.
    if (row_offset + rows > from->rows || col_offset + cols > from->cols) {
        return -1;
    }

    // allocate space for the new matrix struct. 
    matrix *newMat = (matrix *) malloc(sizeof(matrix));
    if (newMat == NULL) {
        return -2;
    }

    // set the data array of the new matrix.
    newMat->data = (double **) malloc(rows * sizeof(double*));
    if (newMat->data == NULL) {
        return -2;
    }

    #pragma omp parallel for
    for (int i = 0; i < rows; ++i) {
        newMat->data[i] = from->data[i + row_offset] + col_offset;
    }

    // set is_1d field.
    if (rows == 1 || cols == 1) {
        newMat->is_1d = 1;
    } else {
        newMat->is_1d = 0;
    }

    // set all other fields.
    newMat->parent = from;
    newMat->rows = rows;
    newMat->cols = cols;
    newMat->ref_cnt = 1;

    // add ref_cnt of parent matrix by one
    from->ref_cnt += 1;

    // store the address of the new matrix in mat.
    *mat = newMat;
    return 0;
}

/*
 * This function will be called automatically by Python when a numc matrix loses all of its
 * reference pointers.
 * You need to make sure that you only free `mat->data` if no other existing matrices are also
 * referring this data array.
 * See the spec for more information.
 */
void deallocate_matrix(matrix *mat) {
    // decrease ref count by one
    mat->ref_cnt -= 1;

    // if ref count is 0, deallocate mat
    if (mat->ref_cnt == 0) {
        if (mat->parent == NULL) {
            for (int i = 0; i < mat->rows; ++i) {
                free(mat->data[i]);
            }
        } else {
            // try to deallocate its parent
            deallocate_matrix(mat->parent);
        }

        free(mat->data);
        free(mat);
    }
}

/*
 * Return the double value of the matrix at the given row and column.
 * You may assume `row` and `col` are valid.
 */
double get(matrix *mat, int row, int col) {
    return mat->data[row][col];
}

/*
 * Set the value at the given row and column to val. You may assume `row` and
 * `col` are valid
 */
void set(matrix *mat, int row, int col, double val) {
    mat->data[row][col] = val;
}

/*
 * Set all entries in mat to val
 */
void fill_matrix(matrix *mat, double val) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < mat->rows; ++i) {
        for (int j = 0; j < mat->cols; ++j) {
            mat->data[i][j] = val;
        }
    }
}

/**
 * Copy mat to result.
 */
int copy_matrix(matrix *result, matrix *mat) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < mat->rows; ++i) {
        for (int j = 0; j < mat->cols; ++j) {
            result->data[i][j] = mat->data[i][j];
        }
    }

    return 0;
}

/*
 * Store the result of adding mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int add_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < result->rows; ++i) {
        for (int j = 0; j < result->cols; ++j) {
            result->data[i][j] = mat1->data[i][j] + mat2->data[i][j];
        }
    }
    return 0;
}

/*
 * Store the result of subtracting mat2 from mat1 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int sub_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < result->rows; ++i) {
        for (int j = 0; j < result->cols; ++j) {
            result->data[i][j] = mat1->data[i][j] - mat2->data[i][j];
        }
    }
    return 0;
}

/*
 * Store the result of multiplying mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that matrix multiplication is not the same as multiplying individual elements.
 */
int mul_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    // set result to zeros
    fill_matrix(result, 0);

    #pragma omp parallel for
    // ordering ikj and kij have the best cache performance
    for (int i = 0; i < mat1->rows; ++i) {
        for (int k = 0; k < mat1->cols; ++k) {
            for (int j = 0; j < mat2->cols; ++j) {
                // result[i * n + j] += mat1[i * n + k] * mat2[k * n + j]
                result->data[i][j] += mat1->data[i][k] * mat2->data[k][j];
            }
        }
    }
    return 0;
}

/*
 * Store the result of raising mat to the (pow)th power to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that pow is defined with matrix multiplication, not element-wise multiplication.
 */
int pow_matrix(matrix *result, matrix *mat, int pow) {
    // set result to the identity matrix
    fill_matrix(result, 0);
    #pragma omp parallel for
    for (int i = 0; i < mat->rows; ++i) {
        result->data[i][i] = 1;
    }

    matrix *temp = NULL;
    allocate_matrix(&temp, mat->rows, mat->cols);

    matrix *copy = NULL;
    allocate_matrix(&copy, mat->rows, mat->cols);
    copy_matrix(copy, mat);

    while (pow > 0) {
        if (pow % 2 == 1) {
            mul_matrix(temp, result, copy);
            copy_matrix(result, temp);
            pow -= 1;
        } else {
            mul_matrix(temp, copy, copy);
            copy_matrix(copy, temp);
            pow /= 2;
        }
    }

    // deallocate temp and copy
    deallocate_matrix(temp);
    deallocate_matrix(copy);

    return 0;
}

/*
 * Store the result of element-wise negating mat's entries to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int neg_matrix(matrix *result, matrix *mat) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < mat->rows; ++i) {
        for (int j = 0; j < mat->cols; ++j) {
            result->data[i][j] = -mat->data[i][j];
        }
    }
    return 0;
}

/*
 * Store the result of taking the absolute value element-wise to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int abs_matrix(matrix *result, matrix *mat) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < mat->rows; ++i) {
        for (int j = 0; j < mat->cols; ++j) {
            double val = mat->data[i][j];
            if (val < 0) val = -val;
            result->data[i][j] = val;
        }
    }
    return 0;
}

