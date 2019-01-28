/** @file formatstring/QuickFormat.h
 *
 * This header contains several easy-to-use methods to start a string formatting
 * process.
 */

#ifndef FORMATSTRING_QUICKFORMAT_H
#define FORMATSTRING_QUICKFORMAT_H

#include "formatstring/Formatstring.h"

#include <iostream>


namespace fs {

/** Helper method to avoid copying parameters that are only used temporarily. */
template <typename T>
inline U<Variable> wrapInReference(T&& value)
{
	using Tderef = typename std::remove_reference<T>::type;
	return mkU<VariableRawReference<Tderef>>(&value);
}

/** Helper method to avoid copying parameters that are only used temporarily. */
template <typename>
inline U<Variable> wrapInReference(U<Variable> var)
{
	return var;
}


/**
 * Returns a Formatstring initialized with the given format and the given
 * variables as references.
 * A Formatstring automatically converts to a std::string.
 */
template <typename... Args>
inline Formatstring formats(const std::string& format, Args&&... args)
{
	Formatstring f(format);
	f.args(wrapInReference<Args>(std::forward<Args>(args))...);
	return f;
}

template <typename T, typename... Args>
inline std::string format(const std::string& format, Args&&... args)
{
	return Formatstring(format)
	        .args(wrapInReference<Args>(std::forward<Args>(args))...)
	        .str();
}

/** Formats the variables according to the fmt string and prints it to cout. */
template <typename... Args>
inline void print(const std::string& fmt, Args&&... args)
{
	formats(fmt, args...).print();
}

/** Prints the given string to cout. */
inline void print(const std::string& fmt)
{
	std::cout << fmt;
}

/** Formats the variables according to the fmt string and prints it to cout. */
template <typename... Args>
inline void println(const std::string& fmt, Args&&... args)
{
	formats(fmt, args...).println();
}

/** Prints the given string and a newline to cout. */
inline void println(const std::string& fmt)
{
	std::cout << fmt << '\n';
}

/** Prints a newline to cout. */
inline void println()
{
	std::cout << '\n';
}

/** Formats the variables and prints the result to the stream. */
template <typename... Args>
inline void write(std::iostream& s, const std::string& fmt, Args&&... args)
{
	formats(fmt, args...).write(s);
}

/** Formats the variables and prints the result to the stream. */
template <typename... Args>
inline void writeln(std::iostream& s, const std::string& fmt, Args&&... args)
{
	formats(fmt, args...).writeln(s);
}

} // namespace fs

#endif //FORMATSTRING_QUICKFORMAT_H
