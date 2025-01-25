# Coding Standard

### Integers and Floats
Integer and floating point variables are declared and defined according to the types in stdint.h. 
Signed integer variables are given the prefix "i", unsigned are given "u", floats are given "f". 
The size of the integer or float in bits follows.

Example:
```c
uint8_t     u8_foo;
int16_t     i16_bar;
float64_t   f64_baz;
```

### Booleans

Booleans are declared and defined according to stdbool.h/ using the `bool` type. 
A boolean variable uses the "b" prefix.

Example:
```c
bool b_buffer_full;
```

### Single Characters

Single characters are declared and defined with a "c" prefix.

Example:
```c
char c_current_char;
```

### Structs, Enums, and Unions

Standalone (i.e. not nested) structs, enums, and unions are always typedef'd and prefixed with the name of their module in capital letters, and postfixed with "_t".
They are also forward aliased with a leading underscore (and no "_t").
All typedefs should have an associated doxygen-style comment of reasonable length.
Given a file vehicle.h, the following illustrates this convention for a struct:

Example Struct:
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

Enumeration constants of enum typedefs should be defined in capital letters and should be prefixed with the module name in which they are defined. Their prefix should also contain the typedef of the enum. All enums should have a sentinel entry indicating the number of enumeration constants. Ten forward slashes should precede this sentinel entry and its name should be derived from its module and the name of the enum typedef itself.

Example Enum:
```c
/**
 *  Vehicle type IDs
 */
typedef enum _VEHICLE_type_id
{
	VEHICLE_TYPE_ID_CAR = 0,
	VEHICLE_TYPE_ID_TRUCK,
	VEHICLE_TYPE_ID_MOTORCYCLE,
	VEHICLE_TYPE_ID_BOAT,
	//////////
	VEHICLE_TYPE_ID_NUM_TYPE_IDS
} VEHICLE_type_id_t;
```

The standard for unions is fairly straightforward. Internal composite members should be forward-declared but not typedef'd.

Example Union:

```c
/**
 *  Vehicle configuration options
 */
typedef union _VEHICLE_config
{
	struct _VEHICLE_specs
	{
		uint32_t u32_engine_capacity;
		uint8_t  u8_number_of_wheels;
		uint8_t  u8_has_sunroof;
		uint16_t u16_max_speed;
	} specs;

	uint64_t u64_config_value;
	uint8_t u8_bytes[8];

} VEHICLE_config_t;
```

### Pointers, Arrays, Const

Pointers to and arrays of any type take a "p" prefix. Pointer declarations should always have a space on either side of their asterisk(s). The number of "p"s should correspond to the depth of indirection of the pointer. `const` variables take a "k" prefix;

Example:

```c
uint8_t pu8_buffer[JSON_BUFFER_SIZE];
char * pc_name;
char ** ppc_names = {"foo", "bar", "baz"};
const char * kpc_part_number = "ATSAMC21E18A";
```

Actual arrays of pointers need not follow the multiple "p" prefix rule. The following example is valid.

```c
char * pc_names[IDENTIFIERS_NUM_NAMES];
```
