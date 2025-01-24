# Coding Standard

## Style and Naming Conventions
Generally, we use snake case with Hungarian prefix notation. 
Prefixes correspond with a variable's type. Allman-style braces are used.

### Basic Types

#### Integers and Floats
Integer and floating point variables are declared and defined according to the types in stdint.h. 
Signed integer variables are given the prefix "i", unsigned are given "u", floats are given "f". 
The size of the integer or float in bits follows.

Some examples:
```c
uint8_t     u8_foo;
int16_t     i16_bar;
float64_t   f64_baz;
```

#### Booleans

Booleans are declared and defined according to stdbool.h/ using the `bool` type. 
A boolean variable uses the "b" prefix.

Example:
```c
bool b_buffer_full;
```

#### Characters

Characters are declared and defined with a "c" prefix.

Example:
```c
char c_current_char;
```

### Structs

Structs, are always typedef'd and prefixed with the name of their module in capital letters, and postfixed with "_t".
They are also forward aliased with a leading underscore (and no "_t").
All typedefs should have an associated doxygen-style comment of reasonable length.
Given a file vehicle.h, the following example illustrates this convention:

```c
/**
 *  Basic car implementation
 */
typedef struct _VEHICLE_car
{
    uint32_t    u32_price;
    uint8_t     u8_max_passengers;
} VEHICLE_car_t;
```

Unlike builtin types, an instance of a struct is declared without a prefix:

```c
VEHICLE_car_t car;
```

If the instance is to be declcared and initialized, designated initializers are used,
and their values should be tab-aligned.

```c
VEHICLE_car_t car =
{
    .u32_price          = 10000,
    .u8_max_passengers  = 5
};
```

### Pointers and Arrays

Pointers to and arrays of any type take a "p" prefix.

### const and static variables