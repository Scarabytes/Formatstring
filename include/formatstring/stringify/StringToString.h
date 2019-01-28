/** @file formatstring/stringify/StringToString.h
 *
 * This header provides the str() method for string types, so that they can be
 * formatted with or simply passed through toString().
 */
 
#ifndef FORMATSTRING_STRINGTOSTRING_H
#define FORMATSTRING_STRINGTOSTRING_H

#include <string>

namespace fs {

/**
 * This set of str() functions can format a string value.
 *
 * Format string syntax
 * --------------------
 *
 *     [format_specifier] := [[fill]align][#][width][substring][replace]
 *     [fill]             := any character
 *     [align]            := "<" | ">" | "^"
 *     [width]            := integer
 *     [substring]        := "s" integer "-" integer
 *     [replace]          := ( "r" replace_target "-" replace_target )*
 *     [replace_target]   := any character | "'" string "'"
 *
 * align/fill
 * ----------
 * If align is specified, it can be padded by any character to be used as a
 * fill character. The default is a space. An alignment option only has an
 * effect if a width is specified as well.
 *
 * The meaning of the alignment options is as follows:
 * - ">" Forces the string to be right-aligned.
 * - "<" Forces the string to be left-aligned. This is the default.
 * - "^" Centers the string within the available space.
 *
 * truncation "#"
 * -------------------
 * If "#" is specified, the input string is shortened to the given width if it
 * was longer.
 *
 * width
 * -----
 * This value defines the width that the number should fill. If the number is
 * shorter, it will be padded to the width with the fill char. If the number is
 * longer, it will be rounded to the amount of visible characters. If that isn't
 * enough, it might exceed this width limit.
 *
 * substring
 * ---------
 * The substring segment begins with the 's' character. It can be separated from
 * the other segments by spaces, but may not contain spaces itself. Following
 * the 's' can be either a single number or two numbers separated by a dash.
 * The meaning is as follows:
 * - s <n>: Only forward the first n characters of the input
 * - s - <n>: Only forward the first n characters of the input
 * - s <n> -: Forward the characters starting at the nth one to the end of
 *     the input.
 * - s <a> - <b>: Forward the characters from the index a to b-1 or to the
 *     end of the input, whichever comes first.
 * Unlike in this documentation, there shouldn't be any spaces between the
 * numbers and the "s" or "-".
 *
 * replacement
 * -----------
 * This segment consists of any number of replacement statements which are
 * applied sequentially. A replacement statement may be separated from other
 * statements by spaces, but may not contain spaces itself, unless in a quoted
 * string. The statements begins with an 'r', followed by the search term.
 * After that, a dash '-' introduces the replacement term. Both search and
 * replacement term can either be a single character or a string enclosed in
 * single quotes. Single quotes themselves can be escaped with a backslash. Note
 * that in C++ code, you need a double backslash!
 * The following are examples for valid replacement terms:
 *
 *     rx-y       // Replace all occurrences of x with a y
 *     r*-'Hello' // Replace all asterisks * with the word Hello
 *     r' '-''    // Remove all spaces
 *     r'Alice'-- // Replace all occurences of "Alice" with a dash
 *     r"-'\''    // Replace all double quotes with single quotes
 *
 * single characters
 * -----------------
 *
 * For a char, if the format begins with an 'i', it is formatted as an
 * 8bit integer, otherwise like a one-character string.
 *
 *     i#x        // Formats the char as a hex integer with the 0x prefix
 *     ra-b       // Formats the char like a string and changes an "a" to a "b"
 */
 
// Specially named str() to hinder the compiler from using implicit conversions
// on custom types to use this method instead of the provided one.
std::string str_string(const std::string& value, const std::string& format);

std::string str(const char* value, const std::string& format);

std::string str(char value, const std::string& format);

} // namespace fs

#endif //FORMATSTRING_STRINGTOSTRING_H
