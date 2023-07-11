#include <stdio.h>
#include "bit_ops.h"

// Return the nth bit of x.
// Assume 0 <= n <= 31
unsigned get_bit(unsigned x,
                 unsigned n)
{
    x = x >> n;
    return x & 1;
}

// Set the nth bit of the value of x to v.
// Assume 0 <= n <= 31, and v is 0 or 1
void set_bit(unsigned * x,
             unsigned n,
             unsigned v)
{
    // x = x & 11..101..11 (0 is the nth bit)
    // x = xx..0..xx (0 is the n bit)
    *x = *x & ~(1 << n);

    // x = x | 00..0v0..00 (v is the nth bit)
    // x = xx..v..xx (v is the nth bit)
    *x = *x | (v << n);
}

// Flip the nth bit of the value of x.
// Assume 0 <= n <= 31
void flip_bit(unsigned * x,
              unsigned n)
{
    // x = xx..v..xx ^ 00..1..0
    // x ^ 0 = x
    // v ^ 1 = ~v
    *x = *x ^ (1 << n);
}
