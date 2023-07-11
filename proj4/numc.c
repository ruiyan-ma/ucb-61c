#include "numc.h"
#include <structmember.h>

PyTypeObject Matrix61cType;

/* Helper functions for initalization of matrices and vectors */

/*
 * Return a tuple given rows and cols
 */
PyObject *get_shape(int rows, int cols) {
  if (rows == 1 || cols == 1) {
    return PyTuple_Pack(1, PyLong_FromLong(rows * cols));
  } else {
    return PyTuple_Pack(2, PyLong_FromLong(rows), PyLong_FromLong(cols));
  }
}
/*
 * Matrix(rows, cols, low, high). Fill a matrix random double values
 */
int init_rand(PyObject *self, int rows, int cols, unsigned int seed, double low,
              double high) {
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    rand_matrix(new_mat, seed, low, high);
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * Matrix(rows, cols, val). Fill a matrix of dimension rows * cols with val
 */
int init_fill(PyObject *self, int rows, int cols, double val) {
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed)
        return alloc_failed;
    else {
        fill_matrix(new_mat, val);
        ((Matrix61c *)self)->mat = new_mat;
        ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    }
    return 0;
}

/*
 * Matrix(rows, cols, 1d_list). Fill a matrix with dimension rows * cols with 1d_list values
 */
int init_1d(PyObject *self, int rows, int cols, PyObject *lst) {
    if (rows * cols != PyList_Size(lst)) {
        PyErr_SetString(PyExc_ValueError, "Incorrect number of elements in list");
        return -1;
    }
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    int count = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            set(new_mat, i, j, PyFloat_AsDouble(PyList_GetItem(lst, count)));
            count++;
        }
    }
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * Matrix(2d_list). Fill a matrix with dimension len(2d_list) * len(2d_list[0])
 */
int init_2d(PyObject *self, PyObject *lst) {
    int rows = PyList_Size(lst);
    if (rows == 0) {
        PyErr_SetString(PyExc_ValueError,
                        "Cannot initialize numc.Matrix with an empty list");
        return -1;
    }
    int cols;
    if (!PyList_Check(PyList_GetItem(lst, 0))) {
        PyErr_SetString(PyExc_ValueError, "List values not valid");
        return -1;
    } else {
        cols = PyList_Size(PyList_GetItem(lst, 0));
    }
    for (int i = 0; i < rows; i++) {
        if (!PyList_Check(PyList_GetItem(lst, i)) ||
                PyList_Size(PyList_GetItem(lst, i)) != cols) {
            PyErr_SetString(PyExc_ValueError, "List values not valid");
            return -1;
        }
    }
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            set(new_mat, i, j,
                PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(lst, i), j)));
        }
    }
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * This deallocation function is called when reference count is 0
 */
void Matrix61c_dealloc(Matrix61c *self) {
    deallocate_matrix(self->mat);
    Py_TYPE(self)->tp_free(self);
}

/* For immutable types all initializations should take place in tp_new */
PyObject *Matrix61c_new(PyTypeObject *type, PyObject *args,
                        PyObject *kwds) {
    /* size of allocated memory is tp_basicsize + nitems*tp_itemsize*/
    Matrix61c *self = (Matrix61c *)type->tp_alloc(type, 0);
    return (PyObject *)self;
}

/*
 * This matrix61c type is mutable, so needs init function. Return 0 on success otherwise -1
 */
int Matrix61c_init(PyObject *self, PyObject *args, PyObject *kwds) {
    /* Generate random matrices */
    if (kwds != NULL) {
        PyObject *rand = PyDict_GetItemString(kwds, "rand");
        if (!rand) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
        if (!PyBool_Check(rand)) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
        if (rand != Py_True) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }

        PyObject *low = PyDict_GetItemString(kwds, "low");
        PyObject *high = PyDict_GetItemString(kwds, "high");
        PyObject *seed = PyDict_GetItemString(kwds, "seed");
        double double_low = 0;
        double double_high = 1;
        unsigned int unsigned_seed = 0;

        if (low) {
            if (PyFloat_Check(low)) {
                double_low = PyFloat_AsDouble(low);
            } else if (PyLong_Check(low)) {
                double_low = PyLong_AsLong(low);
            }
        }

        if (high) {
            if (PyFloat_Check(high)) {
                double_high = PyFloat_AsDouble(high);
            } else if (PyLong_Check(high)) {
                double_high = PyLong_AsLong(high);
            }
        }

        if (double_low >= double_high) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }

        // Set seed if argument exists
        if (seed) {
            if (PyLong_Check(seed)) {
                unsigned_seed = PyLong_AsUnsignedLong(seed);
            }
        }

        PyObject *rows = NULL;
        PyObject *cols = NULL;
        if (PyArg_UnpackTuple(args, "args", 2, 2, &rows, &cols)) {
            if (rows && cols && PyLong_Check(rows) && PyLong_Check(cols)) {
                return init_rand(self, PyLong_AsLong(rows), PyLong_AsLong(cols), unsigned_seed, double_low,
                                 double_high);
            }
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    }
    PyObject *arg1 = NULL;
    PyObject *arg2 = NULL;
    PyObject *arg3 = NULL;
    if (PyArg_UnpackTuple(args, "args", 1, 3, &arg1, &arg2, &arg3)) {
        /* arguments are (rows, cols, val) */
        if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && (PyLong_Check(arg3)
                || PyFloat_Check(arg3))) {
            if (PyLong_Check(arg3)) {
                return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), PyLong_AsLong(arg3));
            } else
                return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), PyFloat_AsDouble(arg3));
        } else if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && PyList_Check(arg3)) {
            /* Matrix(rows, cols, 1D list) */
            return init_1d(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), arg3);
        } else if (arg1 && PyList_Check(arg1) && arg2 == NULL && arg3 == NULL) {
            /* Matrix(rows, cols, 1D list) */
            return init_2d(self, arg1);
        } else if (arg1 && arg2 && PyLong_Check(arg1) && PyLong_Check(arg2) && arg3 == NULL) {
            /* Matrix(rows, cols, 1D list) */
            return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), 0);
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return -1;
    }
}

/*
 * List of lists representations for matrices
 */
PyObject *Matrix61c_to_list(Matrix61c *self) {
    int rows = self->mat->rows;
    int cols = self->mat->cols;
    PyObject *py_lst = NULL;
    if (self->mat->is_1d) {  // If 1D matrix, print as a single list
        py_lst = PyList_New(rows * cols);
        int count = 0;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                PyList_SetItem(py_lst, count, PyFloat_FromDouble(get(self->mat, i, j)));
                count++;
            }
        }
    } else {  // if 2D, print as nested list
        py_lst = PyList_New(rows);
        for (int i = 0; i < rows; i++) {
            PyList_SetItem(py_lst, i, PyList_New(cols));
            PyObject *curr_row = PyList_GetItem(py_lst, i);
            for (int j = 0; j < cols; j++) {
                PyList_SetItem(curr_row, j, PyFloat_FromDouble(get(self->mat, i, j)));
            }
        }
    }
    return py_lst;
}

PyObject *Matrix61c_class_to_list(Matrix61c *self, PyObject *args) {
    PyObject *mat = NULL;
    if (PyArg_UnpackTuple(args, "args", 1, 1, &mat)) {
        if (!PyObject_TypeCheck(mat, &Matrix61cType)) {
            PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
            return NULL;
        }
        Matrix61c* mat61c = (Matrix61c*)mat;
        return Matrix61c_to_list(mat61c);
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }
}

/*
 * Add class methods
 */
PyMethodDef Matrix61c_class_methods[] = {
    {"to_list", (PyCFunction)Matrix61c_class_to_list, METH_VARARGS, "Returns a list representation of numc.Matrix"},
    {NULL, NULL, 0, NULL}
};

/*
 * Matrix61c string representation. For printing purposes.
 */
PyObject *Matrix61c_repr(PyObject *self) {
    PyObject *py_lst = Matrix61c_to_list((Matrix61c *)self);
    return PyObject_Repr(py_lst);
}

/* NUMBER METHODS */

/*
 * Setup result matrix: set shape and allocate memory space.
 *
 * Return true if setup successfully.
 */
int setup_matrix(Matrix61c* self, int rows, int cols) {
    self->shape = get_shape(rows, cols);

    int alloc_code = allocate_matrix(&(self->mat), rows, cols);
    if (alloc_code == -1) {
        PyErr_SetString(PyExc_ValueError, "Invalid dimensions.");
    } else if (alloc_code == -2) {
        PyErr_SetString(PyExc_RuntimeError, "Allocation unsuccessful.");
    }

    return alloc_code == 0;
}

/*
 * Add the second numc.Matrix (Matrix61c) object to the first one. The first operand is
 * self, and the second operand can be obtained by casting `args`.
 */
PyObject *Matrix61c_add(Matrix61c* self, PyObject* args) {
    // check Matrix61cType
    if (!PyObject_TypeCheck(args, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "B is not of type Matrix.");
        return NULL;
    }

    // check dimensions
    Matrix61c *other = (Matrix61c *) args;
    if (self->mat->rows != other->mat->rows || self->mat->cols != other->mat->cols) {
        PyErr_SetString(PyExc_ValueError, "A and B do not have the same dimensions.");
        return NULL;
    }

    Matrix61c *result = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    int success = setup_matrix(result, self->mat->rows, self->mat->cols);
    if (!success) return NULL;

    add_matrix(result->mat, self->mat, other->mat);
    return (PyObject *) result;
}

/*
 * Substract the second numc.Matrix (Matrix61c) object from the first one. The first operand is
 * self, and the second operand can be obtained by casting `args`.
 */
PyObject *Matrix61c_sub(Matrix61c* self, PyObject* args) {
    // check Matrix61cType
    if (!PyObject_TypeCheck(args, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "B is not of type Matrix.");
        return NULL;
    }

    // check dimensions
    Matrix61c *other = (Matrix61c *) args;
    if (self->mat->rows != other->mat->rows || self->mat->cols != other->mat->cols) {
        PyErr_SetString(PyExc_ValueError, "A and B do not have the same dimensions.");
        return NULL;
    }

    Matrix61c *result = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    int success = setup_matrix(result, self->mat->rows, self->mat->cols);
    if (!success) return NULL;

    sub_matrix(result->mat, self->mat, other->mat);
    return (PyObject *) result;
}

/*
 * NOT element-wise multiplication. The first operand is self, and the second operand
 * can be obtained by casting `args`.
 */
PyObject *Matrix61c_multiply(Matrix61c* self, PyObject *args) {
    // check Matrix61cType
    if (!PyObject_TypeCheck(args, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "B is not of type Matrix.");
        return NULL;
    }

    // check dimensions
    Matrix61c *other = (Matrix61c *) args;
    if (self->mat->cols != other->mat->rows) {
        PyErr_SetString(PyExc_ValueError, "A's number of columns is not equal to B's number of rows.");
        return NULL;
    }

    Matrix61c *result = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    int success = setup_matrix(result, self->mat->rows, other->mat->cols);
    if (!success) return NULL;
 
    mul_matrix(result->mat, self->mat, other->mat);
    return (PyObject *) result;
}

/*
 * Negates the given numc.Matrix.
 */
PyObject *Matrix61c_neg(Matrix61c* self) {
    Matrix61c *result = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    int success = setup_matrix(result, self->mat->rows, self->mat->cols);
    if (!success) return NULL;
 
    neg_matrix(result->mat, self->mat);
    return (PyObject *) result;
}

/*
 * Take the element-wise absolute value of this numc.Matrix.
 */
PyObject *Matrix61c_abs(Matrix61c *self) {
    Matrix61c *result = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    int success = setup_matrix(result, self->mat->rows, self->mat->cols);
    if (!success) return NULL;
 
    abs_matrix(result->mat, self->mat);
    return (PyObject *) result;
}

/*
 * Raise numc.Matrix (Matrix61c) to the `pow`th power. You can ignore the argument `optional`.
 */
PyObject *Matrix61c_pow(Matrix61c *self, PyObject *pow, PyObject *optional) {
    // check if pow is an integer
    if (!PyLong_Check(pow)) {
        PyErr_SetString(PyExc_TypeError, "Pow is not an integer.");
        return NULL;
    }

    // check if A is not a square matrix
    if (self->mat->rows != self->mat->cols) {
        PyErr_SetString(PyExc_ValueError, "A is not a square matrix.");
        return NULL;
    }

    // check if pow is negative
    int int_pow = PyLong_AsLong(pow);
    if (int_pow < 0) {
        PyErr_SetString(PyExc_ValueError, "Pow is negative");
        return NULL;
    }

    Matrix61c *result = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    int success = setup_matrix(result, self->mat->rows, self->mat->cols);
    if (!success) return NULL;
 
    pow_matrix(result->mat, self->mat, int_pow);
    return (PyObject *) result;
}

/*
 * Create a PyNumberMethods struct for overloading operators with all the number methods you have
 * define. You might find this link helpful: https://docs.python.org/3.6/c-api/typeobj.html
 */
PyNumberMethods Matrix61c_as_number = {
    .nb_add = (binaryfunc) Matrix61c_add,
    .nb_subtract = (binaryfunc) Matrix61c_sub,
    .nb_multiply = (binaryfunc) Matrix61c_multiply,
    .nb_negative = (unaryfunc) Matrix61c_neg,
    .nb_absolute = (unaryfunc) Matrix61c_abs,
    .nb_power = (ternaryfunc) Matrix61c_pow,
};


/* INSTANCE METHODS */

/*
 * Given a numc.Matrix self, parse `args` to (int) row, (int) col, and (double/int) val.
 * Return None in Python (this is different from returning null).
 */
PyObject *Matrix61c_set_value(Matrix61c *self, PyObject* args) {
    int row, col;
    double val;

    // check the number of arguments
    if (!PyArg_ParseTuple(args, "iid", &row, &col, &val)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments.");
        return NULL;
    }

    // check row and col are in range
    if (row < 0 || col < 0 || row >= self->mat->rows || col >= self->mat->cols) {
        PyErr_SetString(PyExc_IndexError, "Arguments are out of range.");
        return NULL;
    }

    set(self->mat, row, col, val);

    // build a None object
    return Py_BuildValue("");
}

/*
 * Given a numc.Matrix `self`, parse `args` to (int) row and (int) col.
 * Return the value at the `row`th row and `col`th column, which is a Python
 * float/int.
 */
PyObject *Matrix61c_get_value(Matrix61c *self, PyObject* args) {
    int row, col;

    // check the number of arguments
    if (!PyArg_ParseTuple(args, "ii", &row, &col)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments.");
        return NULL;
    }

    // check row and col are in range 
    if (row < 0 || col < 0 || row >= self->mat->rows || col >= self->mat->cols) {
        PyErr_SetString(PyExc_IndexError, "Arguments are out of range.");
        return NULL;
    }

    double result = get(self->mat, row, col);
    return PyFloat_FromDouble(result);
}

/*
 * Create an array of PyMethodDef structs to hold the instance methods.
 * Name the python function corresponding to Matrix61c_get_value as "get" and Matrix61c_set_value
 * as "set"
 * You might find this link helpful: https://docs.python.org/3.6/c-api/structures.html
 */
PyMethodDef Matrix61c_methods[] = {
    {"set", (PyCFunction) Matrix61c_set_value, METH_VARARGS, "Sets the value at the given location."},
    {"get", (PyCFunction) Matrix61c_get_value, METH_VARARGS, "Returns the value at the given location."},
    {NULL, NULL, 0, NULL}
};

/* INDEXING */

/*
 * Parse key arguments into row infos and column infos.
 */
int parse_key(Matrix61c *self, PyObject *key, 
        Py_ssize_t *row_start, Py_ssize_t *row_stop, Py_ssize_t *row_step, Py_ssize_t *row_slice_length, 
        Py_ssize_t *col_start, Py_ssize_t *col_stop, Py_ssize_t *col_step, Py_ssize_t *col_slice_length) {

    if (self->mat->is_1d == 0) {
        // if self->mat is a 2D matrix
        if (PyLong_Check(key)) {
            // key is int
            *row_start = PyLong_AsLong(key);
            *row_slice_length = 1;
            *col_start = 0;
            *col_slice_length = self->mat->cols;
        } else if (PySlice_Check(key)) {
            // key is slice
            PySlice_GetIndicesEx(key, self->mat->rows, row_start, row_stop, row_step, row_slice_length);
            *col_start = 0;
            *col_slice_length = self->mat->cols;
        } else if (PyTuple_Check(key)) {
            // key is tuple: (int, int) or (int, slice) or (slice, int) or (slice, slice)
            PyObject *k1 = PyTuple_GetItem(key, 0);
            PyObject *k2 = PyTuple_GetItem(key, 1);

            if (PyLong_Check(k1) && PyLong_Check(k2)) {
                // key is (int, int)
                *row_start = PyLong_AsLong(k1);
                *row_slice_length = 1;
                *col_start = PyLong_AsLong(k2);
                *col_slice_length = 1;
            } else if (PyLong_Check(k1) && PySlice_Check(k2)) {
                // key is (int, slice)
                *row_start = PyLong_AsLong(k1);
                *row_slice_length = 1;
                PySlice_GetIndicesEx(k2, self->mat->cols, col_start, col_stop, col_step, col_slice_length);
            } else if (PySlice_Check(k1) && PyLong_Check(k2)) {
                // key is (slice, int)
                PySlice_GetIndicesEx(k1, self->mat->rows, row_start, row_stop, row_step, row_slice_length);
                *col_start = PyLong_AsLong(k2);
                *col_slice_length = 1;
            } else if (PySlice_Check(k1) && PySlice_Check(k2)) {
                // key is (slice, slice)
                PySlice_GetIndicesEx(k1, self->mat->rows, row_start, row_stop, row_step, row_slice_length);
                PySlice_GetIndicesEx(k2, self->mat->cols, col_start, col_stop, col_step, col_slice_length);
            } else {
                PyErr_SetString(PyExc_TypeError, "Key is not an integer, a slice, or a tuple of slices/ints.");
                return -1;
            }
        } else {
            PyErr_SetString(PyExc_TypeError, "Key is not an integer, a slice, or a tuple of slices/ints.");
            return -1;
        }
    } else {
        // if self->mat is a 1D list
        if (PyLong_Check(key)) {
            // key is int
            if (self->mat->rows == 1) {
                *row_start = 0;
                *row_slice_length = 1;
                *col_start = PyLong_AsLong(key);
                *col_slice_length = 1;
            } else {
                *row_start = PyLong_AsLong(key);
                *row_slice_length = 1;
                *col_start = 0;
                *col_slice_length = 1;
            }
        } else if (PySlice_Check(key)) {
            // key is slice
            if (self->mat->rows == 1) {
                *row_start = 0;
                *row_slice_length = 1;
                PySlice_GetIndicesEx(key, self->mat->cols, col_start, col_stop, col_step, col_slice_length);
            } else {
                PySlice_GetIndicesEx(key, self->mat->rows, row_start, row_stop, row_step, row_slice_length);
                *col_start = 0;
                *col_slice_length = 1;
            }
        } else {
            PyErr_SetString(PyExc_TypeError, "Key is not an integer or a slice.");
            return -1;
        }
    }

    // throw ValueError if step size is not 1
    if (*row_step != 1 || *col_step != 1) {
        PyErr_SetString(PyExc_ValueError, "Slice info not valid.");
        return -2;
    }

    // throw ValueError if slice length < 1
    if (*row_slice_length < 1 || *col_slice_length < 1) {
        PyErr_SetString(PyExc_ValueError, "Slice info not valid.");
        return -2;
    }

    // throw IndexError if index is out of range
    if (*row_start < 0 || *col_start < 0 
            || *row_start + *row_slice_length > self->mat->rows 
            || *col_start + *col_slice_length > self->mat->cols) {
        PyErr_SetString(PyExc_IndexError, "Index out of range.");
        return -3;
    }

    return 0;
}

/*
 * Given a numc.Matrix `self`, index into it with `key`. Return the indexed result.
 */
PyObject *Matrix61c_subscript(Matrix61c* self, PyObject* key) {
    Py_ssize_t row_start = 0, row_stop = 1, row_step = 1, row_slice_length = 1;
    Py_ssize_t col_start = 0, col_stop = 1, col_step = 1, col_slice_length = 1;

    int ret_val = parse_key(self, key, &row_start, &row_stop, &row_step, &row_slice_length, 
            &col_start, &col_stop, &col_step, &col_slice_length);
    if (ret_val != 0) return NULL;

    // if row slice length and column slice length both are 1, return a single element
    if (row_slice_length == 1 && col_slice_length == 1) {
        PyObject *new_key = Py_BuildValue("ii", row_start, col_start);
        return Matrix61c_get_value(self, new_key);
    }

    Matrix61c *result = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    result->shape = get_shape(row_slice_length, col_slice_length);

    int alloc_code = allocate_matrix_ref(&(result->mat), self->mat, 
            row_start, col_start, row_slice_length, col_slice_length);
    if (alloc_code == -1) {
        PyErr_SetString(PyExc_ValueError, "Invalid dimensions.");
        return NULL;
    } else if (alloc_code == -2) {
        PyErr_SetString(PyExc_RuntimeError, "Allocation unsuccessful.");
        return NULL;
    }

    return (PyObject *) result;
}

/*
 * Set a single element.
 *
 * row: the row index.
 * col: the column index.
 * v: the target value, must be an int or float.
 */
int set_element(Matrix61c* self, int row, int col, PyObject *v) {
    // throw ValueError if resulting slice is 1D or 2D, but the innermost 
    // element is not an int or float.
    if (!PyLong_Check(v) && !PyFloat_Check(v)) {
        PyErr_SetString(PyExc_ValueError, "V is not a float or int.");
        return -2;
    }

    double val = PyFloat_AsDouble(v);
    PyObject *new_key = Py_BuildValue("iid", row, col, val);
    Matrix61c_set_value(self, new_key);
    return 0;
}

/*
 * Set a column slice: a 1 by n slice.
 *
 * row: the row index.
 * col_start: the start index of column.
 * col_slice_length: the length of column slice.
 * v: the target slice, must be a list of int or float.
 */
int set_col_slice(Matrix61c* self, int row, int col_start, int col_slice_length, PyObject *v) {
    // throw TypeError if resulting slice is not 1 by 1, but v is not a list.
    if (!PyList_Check(v)) {
        PyErr_SetString(PyExc_TypeError, "V is not a list.");
        return -1;
    }

    // throw ValueError if slice is 1D, but v has the wrong length.
    if (col_slice_length != PyList_Size(v)) {
        PyErr_SetString(PyExc_ValueError, "V has the wrong length.");
        return -2;
    }

    for (int step = 0; step < col_slice_length; ++step) {
        int ret_val = set_element(self, row, col_start + step, PyList_GetItem(v, step));
        if (ret_val != 0) return ret_val;
    }

    return 0;
}

/*
 * Set a row slice: a n by 1 slice.
 *
 * row_start: the start index of row.
 * row_slice_length: the length of row slice.
 * col: the column index.
 * v: the target slice, must be a list of int or float.
 */
int set_row_slice(Matrix61c* self, int row_start, int row_slice_length, int col, PyObject *v) {
    // throw TypeError if resulting slice is not 1 by 1, but v is not a list.
    if (!PyList_Check(v)) {
        PyErr_SetString(PyExc_TypeError, "V is not a list.");
        return -1;
    }

    // throw ValueError if slice is 1D, but v has the wrong length.
    if (row_slice_length != PyList_Size(v)) {
        PyErr_SetString(PyExc_ValueError, "V has the wrong length");
        return -2;
    }

    for (int step = 0; step < row_slice_length; ++step) {
        int ret_val = set_element(self, row_start + step, col, PyList_GetItem(v, step));
        if (ret_val != 0) return ret_val;
    }

    return 0;
}

/*
 * Set a 2D slice.
 *
 * row_start: the start index of row.
 * row_slice_length: the length of row slice.
 * col_start: the start index of column. 
 * col_slice_length: the length of column slice.
 * v: the target slice, must be a 2D list.
 */
int set_2d_slice(Matrix61c* self, int row_start, int row_slice_length, 
                        int col_start, int col_slice_length, PyObject *v) {
    // if row slice length and column slice length are both 1, set a single element.
    if (row_slice_length == 1 && col_slice_length == 1) {
        // throw TypeError if resulting slice is 1 by 1, but v is not a float or int.
        if (!PyLong_Check(v) && !PyFloat_Check(v)) {
            PyErr_SetString(PyExc_TypeError, "V is not a float or int.");
            return -1;
        }

        return set_element(self, row_start, col_start, v);
    }

    // if row slice length is 1, set column slice.
    if (row_slice_length == 1) {
        return set_col_slice(self, row_start, col_start, col_slice_length, v);
    }

    // if column slice length is 1, set row slice.
    if (col_slice_length == 1) {
        return set_row_slice(self, row_start, row_slice_length, col_start, v);
    }

    // throw TypeError if resulting slice is not 1 by 1, but v is not a list.
    if (!PyList_Check(v)) {
        PyErr_SetString(PyExc_TypeError, "V is not a list.");
        return -1;
    }

    // throw ValueError if slice is 2D, but v has the wrong length.
    if (row_slice_length != PyList_Size(v)) {
        PyErr_SetString(PyExc_ValueError, "V has the wrong length.");
        return -2;
    }

    for (int row = row_start; row < row_start + row_slice_length; ++row) {
        int ret_val = set_col_slice(self, row, col_start, col_slice_length, PyList_GetItem(v, row));
        if (ret_val != 0) return ret_val;
    }

    return 0;
}

/*
 * Given a numc.Matrix `self`, index into it with `key`, and set the indexed result to `v`.
 */
int Matrix61c_set_subscript(Matrix61c* self, PyObject *key, PyObject *v) {
    Py_ssize_t row_start = 0, row_stop = 1, row_step = 1, row_slice_length = 1;
    Py_ssize_t col_start = 0, col_stop = 1, col_step = 1, col_slice_length = 1;

    int ret_val = parse_key(self, key, &row_start, &row_stop, &row_step, &row_slice_length,
            &col_start, &col_stop, &col_step, &col_slice_length);
    if (ret_val != 0) return ret_val;

    return set_2d_slice(self, row_start, row_slice_length, col_start, col_slice_length, v);
}

PyMappingMethods Matrix61c_mapping = {
    NULL,
    (binaryfunc) Matrix61c_subscript,
    (objobjargproc) Matrix61c_set_subscript,
};

/* INSTANCE ATTRIBUTES*/
PyMemberDef Matrix61c_members[] = {
    {
        "shape", T_OBJECT_EX, offsetof(Matrix61c, shape), 0,
        "(rows, cols)"
    },
    {NULL}  /* Sentinel */
};

PyTypeObject Matrix61cType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "numc.Matrix",
    .tp_basicsize = sizeof(Matrix61c),
    .tp_dealloc = (destructor)Matrix61c_dealloc,
    .tp_repr = (reprfunc)Matrix61c_repr,
    .tp_as_number = &Matrix61c_as_number,
    .tp_flags = Py_TPFLAGS_DEFAULT |
    Py_TPFLAGS_BASETYPE,
    .tp_doc = "numc.Matrix objects",
    .tp_methods = Matrix61c_methods,
    .tp_members = Matrix61c_members,
    .tp_as_mapping = &Matrix61c_mapping,
    .tp_init = (initproc)Matrix61c_init,
    .tp_new = Matrix61c_new
};


struct PyModuleDef numcmodule = {
    PyModuleDef_HEAD_INIT,
    "numc",
    "Numc matrix operations",
    -1,
    Matrix61c_class_methods
};

/* Initialize the numc module */
PyMODINIT_FUNC PyInit_numc(void) {
    PyObject* m;

    if (PyType_Ready(&Matrix61cType) < 0)
        return NULL;

    m = PyModule_Create(&numcmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&Matrix61cType);
    PyModule_AddObject(m, "Matrix", (PyObject *)&Matrix61cType);
    printf("CS61C Fall 2020 Project 4: numc imported!\n");
    fflush(stdout);
    return m;
}
