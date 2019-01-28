/** @file formatstring/stringify/IntToString.h
 *
 * This header provides the str() method for integral types, so that they can be
 * used in toString().
 */

#ifndef FORMATSTRING_INTTOSTRING_H
#define FORMATSTRING_INTTOSTRING_H

#include <string>

namespace fs {

/**
 * This set of str() functions can format any integral value as a string.
 *
 * Format string syntax
 * --------------------
 *
 *     [format_specifier] := [[fill]align][sign][#][0][width][type]
 *     [fill]             := any character
 *     [align]            := "<" | ">" | "^" | "="
 *     [sign]             := "+" | "-" | " "
 *     [width]            := integer
 *     [type]             := "d" | "b" | "o" | "x" | "X"
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
 *       number (e.g. +000012).
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
 * If the "#" is specified, an alternate style is used. For integers, this means
 * that a base prefix is prepended to the number on non-base-10 output.
 * For octal output this is 0o, for hexadecimal 0x and for binary 0b.
 *
 * zero padding "0"
 * ----------------
 * This flag is equivalent to specifying "0=" as the alignment. It has priority
 * over the alignment flag though.
 *
 * width
 * -----
 * This value defines the width that the number should fill. The number may take
 * more space if required.
 *
 * type
 * ----
 * This option specifies the type of output. The following values are allowed:
 * - "d" Decimal output. This is the default.
 * - "b" Binary output.
 * - "o" Octal output.
 * - "x" Hexadecimal output with lowercase characters.
 * - "X" Hexadecimal output with uppercase characters.
 */
std::string str(int, const std::string&);


std::string str(signed char 		value, const std::string& format);
std::string str(signed short 		value, const std::string& format);
std::string str(signed int 			value, const std::string& format);
std::string str(signed long 		value, const std::string& format);
std::string str(signed long long 	value, const std::string& format);

std::string str(unsigned char 		value, const std::string& format);
std::string str(unsigned short 		value, const std::string& format);
std::string str(unsigned int 		value, const std::string& format);
std::string str(unsigned long 		value, const std::string& format);
std::string str(unsigned long long 	value, const std::string& format);

}

#endif //FORMATSTRING_INTTOSTRING_H
