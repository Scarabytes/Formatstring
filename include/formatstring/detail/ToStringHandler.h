/** @file formatstring/stringify/detail/ToStringHandler.h
 *
 * Declares toStringHandler methods for all required types.
 */

#ifndef FORMATSTRING_TOSTRINGHANDLER_H
#define FORMATSTRING_TOSTRINGHANDLER_H

#include <string>
#include <sstream>

#include "formatstring/util/Metafunctions.h"
#include "formatstring/stringify/StringToString.h"


namespace fs {

// Metafunctions

// does str(const T&, const string&)->string exist?
GENERATE_EXIST_METAFUNCTION(free_str_with_param_exists,
		str(std::declval<const T>(), std::declval<std::string>()), std::string, T);

// does str(const T&)->string exist?
GENERATE_EXIST_METAFUNCTION(free_str_exists,
		str(std::declval<const T>()), std::string, T);

// does const T.str(const string&)->string exist?
GENERATE_EXIST_METAFUNCTION(member_str_with_param_exists,
		std::declval<const T>().str(std::declval<std::string>()), std::string, T);

// does const T.str()->string exist?
GENERATE_EXIST_METAFUNCTION(member_str_exists,
		std::declval<const T>().str(), std::string, T);

// is T castable to string?
GENERATE_EXIST_METAFUNCTION(cast_to_string_exists,
static_cast<std::string>(std::declval<const T>()), std::string, T);

// does stream << const T exist?
// Note: must be a stream reference, because a template for rvalue ostream&& and
// all types exists in the standard.
GENERATE_EXIST_METAFUNCTION(stream_operator_exists,
		std::declval<std::ostream&>() << std::declval<const T>(), std::ostream&, T);

//==============================================================================
// toStringHandler forward declarations

template <typename T> inline
typename std::enable_if<free_str_with_param_exists<T>::value, std::string>::type
toStringHandler(const T& object, const std::string& format)
{
	return str(object, format);
}

template <typename T> inline
typename std::enable_if<
		free_str_exists<T>::value
		&& !free_str_with_param_exists<T>::value, std::string>::type
toStringHandler(const T& object, const std::string&)
{
	return str(object);
}

template <typename T> inline
typename std::enable_if<
		member_str_with_param_exists<T>::value
		&& !free_str_exists<T>::value
		&& !free_str_with_param_exists<T>::value, std::string>::type
toStringHandler(const T& object, const std::string& format)
{
	return object.str(format);
}

template <typename T> inline
typename std::enable_if<
		member_str_exists<T>::value
		&& !member_str_with_param_exists<T>::value
		&& !free_str_exists<T>::value
		&& !free_str_with_param_exists<T>::value, std::string>::type
toStringHandler(const T& object, const std::string&)
{
	return object.str();
}

template <typename T> inline
typename std::enable_if<
		cast_to_string_exists<T>::value
		&& !member_str_exists<T>::value
		&& !member_str_with_param_exists<T>::value
		&& !free_str_exists<T>::value
		&& !free_str_with_param_exists<T>::value, std::string>::type
toStringHandler(const T& object, const std::string&)
{
	return static_cast<std::string>(object);
}

template <typename T> inline
typename std::enable_if<
		stream_operator_exists<T>::value
		&& !cast_to_string_exists<T>::value
		&& !member_str_exists<T>::value
		&& !member_str_with_param_exists<T>::value
		&& !free_str_exists<T>::value
		&& !free_str_with_param_exists<T>::value, std::string>::type
toStringHandler(const T& object, const std::string&)
{
	std::stringstream s;
	s << object;
	return s.str();
}

template <typename T> inline
typename std::enable_if<
		!stream_operator_exists<T>::value
		&& !cast_to_string_exists<T>::value
		&& !member_str_exists<T>::value
		&& !member_str_with_param_exists<T>::value
		&& !free_str_exists<T>::value
		&& !free_str_with_param_exists<T>::value, std::string>::type
toStringHandler(const T&, const std::string&)
{
	static_assert(stream_operator_exists<T>::value,
			"\n### No conversion for the given type <T> to string was found.\n"
			"At least one of the following methods must be provided in order "
			"to use fs::toString() with this type:\n"
			" - a free method str(const T&, const string&)->string\n"
			" - a free method str(const T&)->string\n"
			" - a const member function str(const string&)->string\n"
			" - a const member function str()->string\n"
			" - a conversion operator from const T to string\n"
			" - the stream operator ostream << const T&");
	return {};
}

inline std::string toStringHandler(const std::string& object, const std::string& format)
{
	return str_string(object, format);
}

} // namespace fs

#endif //FORMATSTRING_TOSTRINGHANDLER_H
