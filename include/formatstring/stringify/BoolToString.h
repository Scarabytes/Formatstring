/** @file formatstring/stringify/BoolToString.h
 *
 * This header provides the str() methods for bool.
 */

#ifndef FORMATSTRING_BOOLTOSTRING_H
#define FORMATSTRING_BOOLTOSTRING_H

#include <string>

namespace fs {

/**
 * This set of str() functions can format a string value.
 *
 * Format string syntax
 * --------------------
 *
 *     [format_specifier] := [[fill]align][width][names]
 *     [fill]             := any character
 *     [align]            := "<" | ">" | "^"
 *     [width]            := integer
 *     [names]            := "n" true_name " " false_name
 *     [true/false_name]  := any character | "'" string "'"
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
 * width
 * -----
 * This value defines the width that the number should fill. If the number is
 * shorter, it will be padded to the width with the fill char. If the number is
 * longer, it will be rounded to the amount of visible characters. If that isn't
 * enough, it might exceed this width limit.
 *
 * names
 * -----
 * If you want to define custom names for true and false, you can use this value
 * to set them. The default would be equivalent to:
 *
 *     n'true' 'false'
 *
 */
std::string str(bool value, const std::string& format);

} // namespace fs

#endif //FORMATSTRING_BOOLTOSTRING_H
