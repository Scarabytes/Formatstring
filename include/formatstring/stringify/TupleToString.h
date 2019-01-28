/** @file formatstring/stringify/TupleToString.h
 *
 * Implements the str() method for tuples.
 */

#ifndef FORMATSTRING_TUPLETOSTRING_H
#define FORMATSTRING_TUPLETOSTRING_H

#include <array>
#include <tuple>
#include <utility>

#include "formatstring/err/FormatException.h"
#include "formatstring/stringify/FormatHelper.h"


namespace fs {

// Forward declaration to print the value type
template <typename T>
std::string toString(const T&, const std::string&);

template <size_t N, size_t I, typename... Args>
struct Concatenator
{
	void operator()(std::string& str, const std::string& divider,
	const std::tuple<Args...>& tuple,
	const std::array<std::string, sizeof...(Args)>& forward)
	{
		if (I > 0)
			str += divider;
		str += toString(std::get<I>(tuple), forward[I]);
		Concatenator<N, I + 1, Args...>()(str, divider, tuple, forward);
	}
};

template <size_t N, typename... Args>
struct Concatenator<N, N, Args...> {
	void operator()(std::string&, const std::string&,
			const std::tuple<Args...>&,
			const std::array<std::string, sizeof...(Args)>&) {}
};

/**
 * This template of the str() function can format a std::tuple.
 *
 * Format string syntax
 * --------------------
 *
 *     [format_specifier] := [[fill]align][width][decorators][:forward]
 *     [fill]             := any character
 *     [align]            := "<" | ">" | "^"
 *     [width]            := integer
 *     [decorators]       := "d" part " " part " " part
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
 * decorators
 * ----------
 * The style of the pair can be changed with the decorators option. This option,
 * initiated with the character "d", defines three parts: The prefix, the value
 * divider and the suffix, themselves divided by spaces. Each can be a single
 * character or a single quoted string. The default behaviour without any
 * formatting would be equivalent to this:
 *
 *     d( ', ' )  // An opening parenthesis, a comma and space as a separator
 *                // and a closing parenthesis
 *
 * forward
 * -------
 * The forward field can be used to forward a formatting string to the entries
 * of the tuple. As each values can be formatted individually, there are as many
 * forwarding fields as elements in the tuple, separated by colons. If you need
 * a colon _within_ any field, you can escape it with a backslash.
 *
 *     :x:o       // Prints the first value as a hexadecimal and the second as
 *                // an octal value.
 */
template <typename... Args>
std::string str(const std::tuple<Args...>& tuple, const std::string& fmt) {
	size_t l = fmt.length();
	size_t i = 0;
	Alignformat af{};
	if (!fmt.empty() && fmt[0] != 'd')
		af = parseAlignformat(fmt, i);
	
	std::string prefix, divider, suffix;
	if (i < l && fmt[i] == 'd') {
		++i;
		prefix = readSingleQuotedString(fmt, i);
		if (i >= l || fmt[i++] != ' ')
			throw err::FormatException("Space expected", fmt, i-1);
		divider = readSingleQuotedString(fmt, i);
		if (i >= l || fmt[i++] != ' ')
			throw err::FormatException("Space expected", fmt, i-1);
		suffix = readSingleQuotedString(fmt, i);
	} else {
		prefix = "(";
		divider = ", ";
		suffix = ")";
	}
	
	std::array<std::string, sizeof...(Args)> forward;
	int n = 0;
	while (i+1 < l && fmt[i] == ':') {
		++i;
		while (i < l && fmt[i] != ':') {
			if (fmt[i] == '\\' && i+1 < l && fmt[i+1] == ':')
				++i;
			forward[n] += fmt[i++];
		}
		++n;
	}
	
	std::string out = prefix;
	Concatenator<sizeof...(Args), 0, Args...>()(out, divider, tuple, forward);
	out += suffix;
	
	return padStringToWidth(out, af);
}

} // namespace fs

#endif //FORMATSTRING_TUPLETOSTRING_H
