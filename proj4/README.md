# numc

Project document: https://inst.eecs.berkeley.edu/~cs61c/fa20/projects/proj4/

Here's what I did in project 4:

### Task1

Implement matrix (a 2d double array) allocation and deallocation methods. 

Implement matrix get and set methods. 

Implement basic calculation methods including add, subtract, multiply, power, negation and absolute. 

### Task2

Write the setup file and use distutils to build and install my numc modules. 

### Task3

Write the Python/C API. 

First, overload operators for `numc.Matrix` objects, including

- Element-wise sum: `a+b`
- Element-wise subtraction: `a-b`
- Matrix multiplication: `a*b`
- Element-wise negation: `-a`
- Element-wise absolute value:  `abs(a)`
- Matrix power: `a ** pow` 

Then, implement two instance methods for `numc.Matrix` : `get` and `set` methods. 

Finally, implement two instance methods for getting slice and changing the value of a slice. 

- `subscript` : takes a matrix and the key to index into the matrix, return a slice. 
- `set_subscript`: takes a matrix, the key to index into the matrix, and the value to set the new slice. 

To implement these two methods, I write some hepler methods: 

- `parse_key` : parse the key to index into the matrix, compute the start index and the length of slice. 
- `set_element` : set value for a single element in matrix. 
- `set_col_slice` : set value for a 1 by n slice. This method need to use `set_element`. 
- `set_row_slice` : set value for a n by 1 slice. This method need to use `set_element`. 
- `set_2d_slice` : set value for a 2d slice. This method need to use the above three methods. 

For `subscript` method, after using `parse_key` to get the start index and the length of slice, we just need to build result matrix and call `allocate_matrix_ref` to allocate space for it. 

For `set_subscript` method, first we use `parse_key` to get the start index and the length of slice. Then we use `set_2d_slice` to set values for the target slice. 

### Task4

Speed up matrix operations. 

There are two techniques we can use to speed up matrix calculation: 

- SIMD instructions: use Intel AVX extensions to perform operations in parallel. 
- OpenMP: use multiple threads to parallelize computation. 

Because my computer use mac M1 chip and there is no Intel AVX extensions support on this computer, I cannot use Intel AVX extensions in my code. 

I use OpenMP to speed up operations. 

This directive can help parallelize for loops. 

```C
#pragma omp parallel for
```

The `collapse` caluse can collapse a nested loop into one large iteration space. 

```c
#pragma omp parallel for collapse(2)
```



