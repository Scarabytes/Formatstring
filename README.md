Formatstring
============

Formatstring is a standalone string formatting library written in C++. It offers
typesafe formatting similar in style to Python's format method. It is fully 
extensible to work with custom types - simply by writing a conversion method - 
and templated implementations are provided for most stl classes. Furthermore, if
a class has a stream output operator, it can be printed without any additional
work.

Features
--------

 * Typesafe, versatile formatting using a formatting string to define location
   and format of variables.
 * Easy and simple formatting based on a single string, not on dozens of stream 
   manipulators.
 * Formatting of all primitive types and most stl classes.
 * No internal stringstreams, thus higher performance.
 * Easily extensible for custom types simply by providing a single method.
 * Highly customizable output for most types.
 * Special features not found in other formatting libraries:
   * Print floating point values with [SI metric prefixes][SI Prefixes]:
     e.g. `4.52e-8` can be formatted as `45.2n`.
   * Floating point numbers can be formatted with a variable precision (e.g. 
     3-6 significant digits).
   * Strings can be substringed or parts replaced from within a format string.
   * Containers, Pairs and Tuples can be formatted with custom style, e.g. with
     a custom choice of brackets and delimiter. Single- and multiline modes are
     available.
 
[SI Prefixes]: https://en.wikipedia.org/wiki/Metric_prefix
 
Quickstart
----------

The easiest way to start is by including the header `formatstring/QuickFormat.h`.
This header defines several methods that can be used to immediately format 
values and print to a stream or a string. 

    #include "formatstring/QuickFormat.h"
    int main() {
        std::vector<int> vals{1, 2, 3};
        fs::println("Message: {}\nID: {3}\nValues: {2}", "Hello fs", vals, 42);
        return 0;
    }

This simple line will output the following to the standard output:

    Message: Hello fs
    ID: 42
    Values: [1, 2, 3]
   
Most values can be formatted to a given width.

    fs::println("Right:  {1:.>8}\nLeft:   {1:.<8}\nCenter: {1:.^8}", 123);
    // Output: 
    Right:  .....123
    Left:   123.....
    Center: ...123..
    
Integers can be formatted in binary, octal or hex. Floating point numbers can
be formatted in fixed-point, scientific, engineering or SI mode.

    fs::println("Int:   {:#x} = {1:#b}\nFloat: {:E} = {2:EE} = {2:si}", 42, 12345.);
    // Output:
    Int: 0x2a = 0b101010
    Float: 1.2345E4 = 12.345E3 = 12.345k

Floats can have fixed or floating precision.

    fs::println("{:.5} = {1:.2-5} = {1:.-2}", 1.234);
    // Output:
    1.2340 = 1.234 = 1.2
    
`QuickFormat.h` provides methods to write to cout (`print()`, `println()`), to
any ostream (`write()`, `writeln()`) or to a string (`format()`). 
There is also `formats()`, which returns an instance of the `Formatstring` 
class. This can be used to define a formatting string but add the variables 
later using `args()` or `operator%`. Formatstring implicitly 
converts to `std::string` and offers the `str()` method to convert it 
explicitly.

Documentation
-------------

A more detailed description for each type can be found in the individual
ToString headers located in `include/formatstring/stringify`. 

Custom Types
------------

To add formatting support to a custom type, one of the following methods must be
provided. They are evaluated in the order shown below, with higher ones 
shadowing later ones.

1. A free function: `std::string str(const T&, const std::string& fmt)`
2. A free function: `std::string str(const T&)`
3. A const member function: `std::string str(const std::string& fmt) const`
4. A const member function: `std::string str() const`
5. A conversion function: `operator std::string() const`
6. The stream operator: `std::ostream& operator<<(std::ostream&, const T&)`

Note that the stream operator has poor performance as an instance of 
std::stringstream needs to be constructed to use it. It is only provided as a 
fallback method.

If none of those methods are provided, compilation fails with an explanatory
error message.
