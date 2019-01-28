/** @file formatstring/util/Metafunctions.h
 *
 * Contains generator macros for template metafunctions.
 */

#ifndef FORMATSTRING_METAFUNCTIONS_H
#define FORMATSTRING_METAFUNCTIONS_H

#include <type_traits>

/**
 * Internal helper to generate a "exists" metafunction. See
 * GENERATE_EXIST_METAFUNCTION for details.
 *
 * Requires the template arguments in three modes:
 * 1. Normal, with typename
 * 2. Modified name, with typename
 * 3. Modified name, without typename
 *
 * To add commas in these arguments, the COMMA macro can be used.
 *
 * Example:
 *
 * GENERATE_EXIST_METAFUNCTION_INTERNAL(exists_foo, std::declval<T>().foo(), R,
 *     typename T COMMA typename R,   // Normal
 *     typename _T COMMA typename _R, // Modified
 *     _T COMMA _R)                   // Modified without typename
 */
#define GENERATE_EXIST_METAFUNCTION_INTERNAL( \
			FUNCTION_NAME, CHECK_EXPRESSION, RETURN_TYPE, \
			TEMPLATE_PARAMS_NORMAL, \
			TEMPLATE_PARAMS_MODIFIED, TEMPLATE_PARAMS_MODIFIED_NO_TYPENAME) \
	\
	template <TEMPLATE_PARAMS_MODIFIED> \
	class FUNCTION_NAME \
	{ \
		typedef char (&yes)[2]; typedef char no;\
		template <TEMPLATE_PARAMS_NORMAL> static yes Check( \
			typename std::enable_if<std::is_convertible<decltype(CHECK_EXPRESSION), RETURN_TYPE>::value, int>::type); \
		template <TEMPLATE_PARAMS_NORMAL> static no Check(...); \
		public: static constexpr bool value = (sizeof(Check<TEMPLATE_PARAMS_MODIFIED_NO_TYPENAME>(0)) == sizeof(yes)); \
	}; \
	template <TEMPLATE_PARAMS_MODIFIED> \
	constexpr bool FUNCTION_NAME<TEMPLATE_PARAMS_MODIFIED_NO_TYPENAME>::value;
// GENERATE_EXIST_METAFUNCTION_INTERNAL

/// A helper macro to pass a comma in a macro argument
#define COMMA ,

/**
 * Generates a metafunction named FUNCTION_NAME to check for the existence of
 * a function or variable of a given type.
 *
 * The created metafunction checks, if a CHECK_EXPRESSION (with one
 * template parameter) can be evaluated and returns the same type as
 * RETURN_TYPE. If so, the value variable of the function is set to true. If
 * either the types don't match or the expression can't be evaluated with the
 * template parameter, the value is false.
 *
 * This macro only allows for one template parameter. If you need more, check
 * GENERATE_EXIST_METAFUNCTION_2, _3 or use GENERATE_EXIST_METAFUNCTION_INTERNAL
 *
 * Example:
 *
 * void foo(float);
 * void foo(bool);
 *
 * GENERATE_EXIST_METAFUNCTION(exists_foo, foo(std::declval<T>()), void, T)
 *
 * exists_foo<int> -> true (int is implicitly converted to float)
 * exists_foo<bool> -> true
 * exists_foo<std::string> -> false
 */
#define GENERATE_EXIST_METAFUNCTION(FUNCTION_NAME, CHECK_EXPRESSION, RETURN_TYPE, TEMPLATE_PARAM_1) \
	GENERATE_EXIST_METAFUNCTION_INTERNAL(FUNCTION_NAME, CHECK_EXPRESSION, RETURN_TYPE, \
		typename TEMPLATE_PARAM_1, typename _##TEMPLATE_PARAM_1, _##TEMPLATE_PARAM_1)
// GENERATE_EXIST_METAFUNCTION


/**
 * Generates a metafunction named FUNCTION_NAME to check for the existence of
 * a function or variable of a given type.
 *
 * The created metafunction checks, if a CHECK_EXPRESSION (with two
 * template parameters) can be evaluated and returns the same type as
 * RETURN_TYPE. If so, the value variable of the function is set to true. If
 * either the types don't match or the expression can't be evaluated with the
 * template parameter, the value is false.
 *
 * This macro allows for two template parameter. If you need just one, check
 * GENERATE_EXIST_METAFUNCTION. If you need more, check
 * GENERATE_EXIST_METAFUNCTION_3 or use GENERATE_EXIST_METAFUNCTION_INTERNAL
 * directly.
 *
 * Example:
 *
 * void foo(float);
 * std::string foo(bool);
 *
 * // The first R is the return type, after that are the two template parameters
 * GENERATE_EXIST_METAFUNCTION_2(exists_foo, foo(std::declval<T>()), R, T, R)
 *
 * exists_foo<int, void> -> true (int is implicitly converted to float)
 * exists_foo<bool, std::string> -> true
 * exists_foo<int, std::string> -> false
 */
#define GENERATE_EXIST_METAFUNCTION_2(FUNCTION_NAME, CHECK_EXPRESSION, RETURN_TYPE, TEMPLATE_PARAM_1, TEMPLATE_PARAM_2) \
	GENERATE_EXIST_METAFUNCTION_INTERNAL(FUNCTION_NAME, CHECK_EXPRESSION, RETURN_TYPE, \
		typename TEMPLATE_PARAM_1 COMMA typename TEMPLATE_PARAM_2, \
		typename _##TEMPLATE_PARAM_1 COMMA typename _##TEMPLATE_PARAM_2, \
		_##TEMPLATE_PARAM_1 COMMA _##TEMPLATE_PARAM_2)
// GENERATE_EXIST_METAFUNCTION_2


/**
 * Generates a metafunction named FUNCTION_NAME to check for the existence of
 * a function or variable of a given type.
 *
 * The created metafunction checks, if a CHECK_EXPRESSION (with three
 * template parameters) can be evaluated and returns the same type as
 * RETURN_TYPE. If so, the value variable of the function is set to true. If
 * either the types don't match or the expression can't be evaluated with the
 * template parameter, the value is false.
 *
 * This macro allows for three template parameter. If you need less, check
 * GENERATE_EXIST_METAFUNCTION_2 or _1. If you need more, check
 * GENERATE_EXIST_METAFUNCTION_INTERNAL.
 *
 * Example:
 *
 * bool foo(double, double);
 * std::string foo(std::string, int);
 *
 * // The first R is the return type, after that are the template parameters
 * GENERATE_EXIST_METAFUNCTION_3(exists_foo, foo(std::declval<A>(), std::declval<B>()), R, A, B, R)
 *
 * exists_foo<double, int, bool> -> true (int is implicitly converted to double)
 * exists_foo<std::string, int, std::string> -> true
 * exists_foo<std::string, double, std::string> -> false
 */
#define GENERATE_EXIST_METAFUNCTION_3(FUNCTION_NAME, CHECK_EXPRESSION, RETURN_TYPE, \
			TEMPLATE_PARAM_1, TEMPLATE_PARAM_2, TEMPLATE_PARAM_3) \
	\
	GENERATE_EXIST_METAFUNCTION_INTERNAL(FUNCTION_NAME, CHECK_EXPRESSION, RETURN_TYPE, \
		typename TEMPLATE_PARAM_1 COMMA typename TEMPLATE_PARAM_2 COMMA typename TEMPLATE_PARAM_3, \
		typename _##TEMPLATE_PARAM_1 COMMA typename _##TEMPLATE_PARAM_2 COMMA typename _##TEMPLATE_PARAM_3, \
		_##TEMPLATE_PARAM_1 COMMA _##TEMPLATE_PARAM_2 COMMA _##TEMPLATE_PARAM_3)
// GENERATE_EXIST_METAFUNCTION_3

/**
 * Generates a metafunction named FUNCTION_NAME to check for the existence of
 * a subtype (typedef within the checked class) of the name SUBTYPE.
 *
 * The created metafunction checks, if T::SUBTYPE is defined. If so, the value
 * variable of the function is set to true. If not, the value is false.
 *
 * Example:
 *
 * struct Foo { typedef int value_type; };
 * struct Bar {};
 *
 * GENERATE_SUBTYPE_METAFUNCTION(has_value_type, value_type)
 *
 * has_value_type<A> -> true
 * has_value_type<B> -> false
 * has_value_type<std::vector<int>> -> true
 */
#define GENERATE_SUBTYPE_METAFUNCTION(FUNCTION_NAME, SUBTYPE) \
	template <typename T>\
	class FUNCTION_NAME \
	{ \
		typedef char (&yes)[2]; typedef char no;\
		template <typename T_> static yes Check(decltype(std::declval<typename T_::SUBTYPE>(), int())); \
		template <typename> static no Check(...); \
		public: static constexpr bool value = (sizeof(Check<T>(0)) == sizeof(yes)); \
	}; \
	template <typename T> \
	constexpr bool FUNCTION_NAME<T>::value;
// GENERATE_SUBTYPE_METAFUNCTION

#endif //FORMATSTRING_METAFUNCTIONS_H
