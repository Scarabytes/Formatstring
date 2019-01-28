/** @file formatstring/ToString.h
 *
 * This header defines the toString() method, which is used to turn any type
 * into a string.
 */

#ifndef FORMATSTRING_TOSTRING_H
#define FORMATSTRING_TOSTRING_H

#include <string>
#include <sstream>

// Include all standard toString implementations:
#include "formatstring/stringify/BoolToString.h"
#include "formatstring/stringify/IntToString.h"
#include "formatstring/stringify/FloatToString.h"
#include "formatstring/stringify/StringToString.h"
#include "formatstring/stringify/CollectionToString.h"
#include "formatstring/stringify/PairToString.h"
#include "formatstring/stringify/TupleToString.h"

// Include toStringHandler last
#include "formatstring/detail/ToStringHandler.h"

namespace fs {

/**
 * The toString method can be used to turn any object into a string.
 * For this to work, at least one of the following methods must be implemented
 * for the object's type. The string parameter is the format string. The topmost
 * method that exists will be used for the conversion.
 *
 * - a free method str(const T&, const string&)->string
 * - a free method str(const T&)->string
 * - a member function str(const string&)->string
 * - a member function str()->string
 * - a conversion function from T to string
 * - the stream operator stream << const T&
 *
 * Default implementations are provided for all basic types and most standard
 * library types. If you are unhappy with these default implementations, you can
 * overload toStringHandler() in the fs namespace and provide your own.
 *
 * @param object The object to turn into a string
 * @param format The format to use for the conversion, defaults to ""
 * @return The converted string
 * @throws ConversionException if the format string cannot be parsed by the
 *         implementation for the given type
 */
template <typename T>
inline std::string toString(const T& object, const std::string& format)
{
	return ::fs::toStringHandler(object, format);
}

template <typename T>
inline std::string toString(const T& object)
{
	return ::fs::toStringHandler(object, {});
}

} // namespace fs


#endif //FORMATSTRING_TOSTRING_H
