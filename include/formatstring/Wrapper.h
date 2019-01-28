/** @file formatstring/Wrapper.h
 *
 * This header defines several methods that wrap values into Variables.
 */

#ifndef FORMATSTRING_WRAPPER_H
#define FORMATSTRING_WRAPPER_H

#include "formatstring/detail/Variable.h"


namespace fs {

/** Copies the given value into a variable wrapper. */
template <typename T>
inline U<Variable> copy(const T& value)
{
	return mkU<VariableCopy<T>>(value);
}

/** References the value in a variable wrapper using a raw pointer. */
template <typename T>
inline U<Variable> ref(P<T> ref)
{
	return mkU<VariableRawReference<T>>(ref);
}

/** References the value in a variable wrapper using a smart pointer. */
template <typename T>
inline U<Variable> ref(W<T> ref)
{
	return mkU<VariableReference<T>>(ref);
}

/** Ties the given variable to the given format. */
inline U<Variable> fmt(U<Variable> var, const std::string& fmt) {
	return mkU<VariableFormat>(std::move(var), fmt);
}

}

#endif //FORMATSTRING_WRAPPER_H
