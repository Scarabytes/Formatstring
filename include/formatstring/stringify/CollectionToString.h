/** @file formatstring/stringify/CollectionToString.h
 *
 * Implements the str() method for collections.
 */

#ifndef FORMATSTRING_COLLECTIONTOSTRING_H
#define FORMATSTRING_COLLECTIONTOSTRING_H

#include "formatstring/err/FormatException.h"
#include "formatstring/stringify/FormatHelper.h"
#include "formatstring/util/Metafunctions.h"


namespace fs {

// Forward declaration to print the value type
template <typename T>
std::string toString(const T&, const std::string&);

// Metafunctions

GENERATE_SUBTYPE_METAFUNCTION(has_value_type, value_type);
GENERATE_SUBTYPE_METAFUNCTION(has_mapped_type, mapped_type);
GENERATE_SUBTYPE_METAFUNCTION(auto_format_forbidden, fs_no_auto_formatting);

GENERATE_EXIST_METAFUNCTION(has_const_begin, std::declval<const T>().begin(), typename T::const_iterator, T);
GENERATE_EXIST_METAFUNCTION(has_const_end, std::declval<const T>().end(), typename T::const_iterator, T);
GENERATE_EXIST_METAFUNCTION(has_begin, std::declval<const T>().begin(), typename T::iterator, T);
GENERATE_EXIST_METAFUNCTION(has_end, std::declval<const T>().end(), typename T::iterator, T);
GENERATE_EXIST_METAFUNCTION(has_empty, std::declval<const T>().empty(), bool, T);

/**
 * This template of the str() function can format a linear collection of values.
 *
 * For a linear collection to be enabled, it needs to define const begin() and
 * end() methods returning iterators, which themselves are defined as the
 * typedef T::const_iterator or T::iterator. Furthermore, a const empty() method
 * is needed returning a bool. Last but not least, the type T::value_type needs
 * to exist. If T::mapped_type exists as well, this is treated as an associative
 * collection.
 * If you want to disable automatic formatting using this method, simply typedef
 * T::fs_no_auto_formatting in your class or override toStringHandler.
 *
 * Format string syntax
 * --------------------
 *
 *     [format_specifier] := [[fill]align][width][inline|decorators][:forward]
 *     [fill]             := any character
 *     [align]            := "<" | ">" | "^"
 *     [width]            := integer
 *     [inline]           := "i" | "m"
 *     [decorators]       := default_decorator | full_decorator
 *     [default_decorator]:= "d" part " " part " " part
 *     [full_decorator]   := "D" part " " part " " part " " part
 *     [part]             := any character | "'" any string "'"
 *     [forward]          := any string
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
 * This value defines the width that the output should fill. If the output is
 * shorter, it will be padded to the width with the fill char. If the output is
 * longer, nothing happens and the output exceeds the width. Note that padding
 * is disabled in multiline mode.
 *
 * inline
 * ------
 * This value can force the output to be in a single line or in multiple lines.
 * The value "i" sets inline output, the default, while "m" sets multiline
 * output. The latter might be useful for longer values, though alignment
 * options are disabled in this mode.
 *
 * decorators
 * ----------
 * As an alternative to the inline option, the individual pre- and suffixes can
 * be set individually. The decorator option, initiated with the character "d",
 * defines three parts: The array prefix, the value divider and the array
 * suffix, themselves divided by spaces. Each can be a single character or a
 * single quoted string. The default behaviour without any formatting would be
 * equivalent to this:
 *
 *     d[ ', ' ]  // An opening bracket, a comma and space as a separator and a
 *                // closing bracket
 *
 * It might be desirable to set a special representation for an empty
 * collection. In that case, the "D" decorator can be used instead, which has an
 * additional part which is used in case the collection is empty. The default
 * multiline behaviour is similar to the following:
 *
 *     D'[\n' ',\n' '\n]' '[\n]'
 *
 * forward
 * -------
 * The forward field can be used to forward a formatting string to the entries
 * of the collection. Appending a colon and the desired string to this object's
 * format string will forward it to the values.
 *
 *     i:x        // Prints an inline list of hex values
 */
template <typename T>
typename std::enable_if<
        has_value_type<T>::value
        && !has_mapped_type<T>::value
        && !auto_format_forbidden<T>::value
        && (has_begin<T>::value || has_const_begin<T>::value)
        && (has_end<T>::value || has_const_end<T>::value)
        && has_empty<T>::value, std::string>::type
str(const T& collection, const std::string& fmt)
{
	size_t l = fmt.length();
	size_t i = 0;
	Alignformat af{};
	if (!fmt.empty() && fmt[0] != 'd' && fmt[0] != 'D')
		af = parseAlignformat(fmt, i);
	
	std::string prefix, divider, suffix, empty;
	
	if (i < l && fmt[i] == 'm') {
		prefix = "[\n";
		divider = ",\n";
		suffix = "\n]";
		empty = "[\n]";
		++i;
	} else if (i < l && (fmt[i] == 'd' || fmt[i] == 'D')) {
		bool empty_provided = fmt[i] == 'D';
		++i;
		prefix = readSingleQuotedString(fmt, i);
		if (i >= l || fmt[i++] != ' ')
			throw err::FormatException("Space expected", fmt, i-1);
		divider = readSingleQuotedString(fmt, i);
		if (i >= l || fmt[i++] != ' ')
			throw err::FormatException("Space expected", fmt, i-1);
		suffix = readSingleQuotedString(fmt, i);
		if (empty_provided) {
			if (i >= l || fmt[i++] != ' ')
				throw err::FormatException("Space expected", fmt, i-1);
			empty = readSingleQuotedString(fmt, i);
		} else {
			empty = prefix + suffix;
		}
	} else {
		if (i < l && fmt[i] == 'i')
			++i;
		prefix = "[";
		divider = ", ";
		suffix = "]";
		empty = "[]";
	}
	
	std::string forwarded_format;
	if (i+1 < l && fmt[i] == ':')
		forwarded_format = fmt.substr(i+1);
	
	std::string out;
	if (collection.empty()) {
		out = empty;
	} else {
		out = prefix;
		bool first = true;
		
		for (const typename T::value_type& value: collection) {
			if (first)
				first = false;
			else
				out += divider;
			out += fs::toString(value, forwarded_format);
		}
		out += suffix;
	}
	
	return padStringToWidth(out, af);
}

/**
 * This template of the str() function can format a map of keys and values.
 *
 * For a map class to be enabled, it needs to define const begin() and
 * end() methods returning iterators, which themselves are defined as the
 * typedef T::const_iterator or T::iterator. Furthermore, a const empty() method
 * is needed returning a bool. Last but not least, the types T::value_type and
 * T::mapped_type are required.
 * If you want to disable automatic formatting using this method, simply typedef
 * T::fs_no_auto_formatting in your class or override toStringHandler.
 *
 * Format string syntax
 * --------------------
 *
 *     [format_specifier] := [[fill]align][width][inline|decorators][:forward]
 *     [fill]             := any character
 *     [align]            := "<" | ">" | "^"
 *     [width]            := integer
 *     [inline]           := "i" | "m"
 *     [decorators]       := default_decorator | full_decorator
 *     [default_decorator]:= "d" part " " part " " part " " part
 *     [full_decorator]   := "D" part " " part " " part " " part " " part
 *     [part]             := any character | "'" any string "'"
 *     [forward]          := ":" any string [ ":" any string ]
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
 * inline
 * ------
 * This value can force the output to be in a single line or in multiple lines.
 * The value "i" sets inline output, the default, while "m" sets multiline
 * output. The latter might be useful for longer values, though alignment
 * options are disabled in this mode.
 *
 * decorators
 * ----------
 * As an alternative to the inline option, the individual pre- and suffixes can
 * be set individually. The decorator option, initiated with the character "d",
 * defines four parts: The map prefix, the key-value combiner, the value
 * divider and the map suffix, themselves divided by spaces. Each can be a
 * single character or a single quoted string. The default behaviour without any
 * formatting would be equivalent to this:
 *
 *     d[ ': ' ', ' ]  // An opening bracket, a colon and a space for key and
 *                     // value, a comma and space as a separator and a closing
 *                     // bracket at the end
 *
 * It might be desirable to set a special representation for an empty
 * collection. In that case, the "D" decorator can be used instead, which has an
 * additional part which is used in case the collection is empty. The default
 * multiline behaviour is similar to the following:
 *
 *     D'[\n' ': ' ',\n' '\n]' '[\n]'
 *
 * forward
 * -------
 * The forward field can be used to forward a formatting string to the entries
 * of the collection. As both key and value can be formatted individually, there
 * are two forwarding fields, separated by colons. The first is used for the
 * key, the second for the value. If you need a colon _within_ the first field,
 * you can escape it with a backslash.
 *
 *     i:x:o      // Prints an inline map of octal values, indexed by hex keys
 */
template <typename T>
typename std::enable_if<
		has_value_type<T>::value
		&& has_mapped_type<T>::value
		&& !auto_format_forbidden<T>::value
		&& (has_begin<T>::value || has_const_begin<T>::value)
		&& (has_end<T>::value || has_const_end<T>::value)
		&& has_empty<T>::value, std::string>::type
str(const T& map, const std::string& fmt)
{
	size_t l = fmt.length();
	size_t i = 0;
	Alignformat af{};
	if (!fmt.empty() && fmt[0] != 'd' && fmt[0] != 'D')
		af = parseAlignformat(fmt, i);
	
	std::string prefix, pairer, divider, suffix, empty;
	
	if (i < l && fmt[i] == 'm') {
		prefix = "[\n";
		pairer = ": ";
		divider = ",\n";
		suffix = "\n]";
		empty = "[\n]";
		++i;
	} else if (i < l && (fmt[i] == 'd' || fmt[i] == 'D')) {
		bool empty_provided = fmt[i] == 'D';
		++i;
		prefix = readSingleQuotedString(fmt, i);
		if (i >= l || fmt[i++] != ' ')
			throw err::FormatException("Space expected", fmt, i-1);
		pairer = readSingleQuotedString(fmt, i);
		if (i >= l || fmt[i++] != ' ')
			throw err::FormatException("Space expected", fmt, i-1);
		divider = readSingleQuotedString(fmt, i);
		if (i >= l || fmt[i++] != ' ')
			throw err::FormatException("Space expected", fmt, i-1);
		suffix = readSingleQuotedString(fmt, i);
		if (empty_provided) {
			if (i >= l || fmt[i++] != ' ')
				throw err::FormatException("Space expected", fmt, i-1);
			empty = readSingleQuotedString(fmt, i);
		} else {
			empty = prefix + suffix;
		}
	} else {
		if (i < l && fmt[i] == 'i')
			++i;
		prefix = "[";
		pairer = ": ";
		divider = ", ";
		suffix = "]";
		empty = "[]";
	}
	
	std::string key_forwarded_format;
	if (i+1 < l && fmt[i] == ':') {
		++i;
		while (i < l && fmt[i] != ':') {
			if (fmt[i] == '\\' && i+1 < l && fmt[i+1] == ':')
				++i;
			key_forwarded_format += fmt[i++];
		}
	}
	std::string value_forwarded_format;
	if (i+1 < l && fmt[i] == ':')
		value_forwarded_format = fmt.substr(i+1);
	
	std::string out;
	if (map.empty()) {
		out = empty;
	} else {
		out = prefix;
		bool first = true;
		
		for (const auto& value: map) {
			if (first)
				first = false;
			else
				out += divider;
			out += fs::toString(value.first, key_forwarded_format);
			out += pairer;
			out += fs::toString(value.second, value_forwarded_format);
		}
		out += suffix;
	}
	
	return padStringToWidth(out, af);
}

} // namespace fs

#endif //FORMATSTRING_COLLECTIONTOSTRING_H
