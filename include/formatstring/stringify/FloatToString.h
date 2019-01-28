/** @file formatstring/stringify/FloatToString.h
 *
 * This header provides the str() method for floating point types so that they
 * can be used with toString().
 */

#ifndef FORMATSTRING_FLOATTOSTRING_H
#define FORMATSTRING_FLOATTOSTRING_H

#include <string>


namespace fs {

/**
 * This set of str() functions can format any floating point value as a string.
 *
 * Format string syntax
 * --------------------
 *
 *     [format_specifier] := [[fill]align][sign][#][0][width]["." precision][type]
 *     [fill]             := <any character>
 *     [align]            := "<" | ">" | "^" | "="
 *     [sign]             := "+" | "-" | " "
 *     [width]            := integer
 *     [precision]        := exact_precision | min_precision "-" max_precision
 *     [exact_precision]  := integer
 *     [min_precision]    := integer
 *     [max_precision]    := integer
 *     [type]             := "g" | "G" | "e" | "E" | "ee" | "EE" | "si" |  "f"
 *
 * align/fill
 * ----------
 * If align is specified, it can be preceded by any character to be used as a
 * fill character. The default is a space. An alignment option only has an
 * effect if a width is specified as well.
 *
 * The meaning of the alignment options is as follows:
 * - ">" Forces the value to be right-aligned. This is the default for numbers.
 * - "<" Forces the value to be left-aligned.
 * - "^" Centers the value within the available space.
 * - "=" Fills all available space with the fill char between the sign and the
 *       number (e.g. +001.23).
 *
 * sign
 * ----
 * The sign option can be one of the following:
 * - "-" Indicates that the sign should only be used for negative values. This
 *       is the default behaviour.
 * - "+" Indicates that a sign should be used for all values.
 * - " " Indicates that negative numbers should be preceded by a - and positive
 *       numbers by a space.
 *
 * alternate style "#"
 * -------------------
 * If "#" is specified, the decimal point is always shown, even if the number
 * actually has an integer value (e.g. 4. instead of 4).
 *
 * zero padding "0"
 * ----------------
 * This flag is equivalent to specifying "0=" as the alignment. It has priority
 * over the alignment flag though.
 *
 * width
 * -----
 * This value defines the width that the number should fill. If the number is
 * shorter, it will be padded to the width with the fill char. If the number is
 * longer, nothing happens and the output exceeds the  width.
 *
 * precision
 * ---------
 * The precision value determines the number or range of significant digits to
 * print. If only one single number is given, every value is printed with
 * exactly that precision. If a range (e.g. 4-8) is given, the number uses the
 * precision in this range that fits its value the best. If the type is set to
 * "f" (fixed-point), the precision defined the number of decimal digits,
 * otherwise it defines the number of significant digits.
 *
 * type
 * ----
 * This option specifies the type of output. The following values are allowed:
 * - "g" General format. The number is displayed in fixed-point or scientific
 * 	     notation depending on it's magnitude (Scientific for magnitudes > 10^9
 * 	     or < 10^-3). Scientific notation uses "e" for the exponent. This is the
 * 	     default type.
 * - "G" Similar to "g" but uses "E" for the exponent.
 * - "e" Exponent notation. Prints the number in scientific format using "e" for
 *       the exponent.
 * - "E" Similar to "e" but uses "E" for the exponent.
 * - "ee" Engineering notation. Similar to "e", but only uses exponents that are
 *       multiples of 3 (e.g. 12e3 instead of 1.2e4).
 * - "EE" Similar to "ee" but uses "E" for the exponent.
 * - "si" Similar to "ee" but uses SI prefixes instead of an exponent (e.g. 12k
 *       instead of 12e3).
 * - "f" Fixed notation. Displays the number as a fixed-point number.
 *
 * special values
 * --------------
 * Infinity is displayed as "+/-inf" or "+/-Inf" in uppercase mode.
 * NaN is displayed as "nan" or "NaN" respectively.
 */
std::string str(float value, const std::string& format);
std::string str(double value, const std::string& format);
std::string str(long double value, const std::string& format);


namespace detail {

struct decimal {
	std::string digits;
	int exponent;
};

struct fp {
	uint64_t f;
	int32_t e;
};

decimal grisu2(fp v);

} // namespace detail
} // namespace fs

#endif //FORMATSTRING_FLOATTOSTRING_H
