/** @file formatstring/detail/Variable.h
 *
 * The Variable class is a type erased holder for references to or copies of
 * arbitrary values.
 */

#ifndef FORMATSTRING_VARIABLE_H
#define FORMATSTRING_VARIABLE_H

#include <string>
#include <formatstring/err/FormatException.h>

#include "formatstring/util/PointerUtil.h"
#include "formatstring/ToString.h"


namespace fs
{

class Variable
{
public:
	virtual ~Variable() = default;
	
	virtual std::string toString(const std::string& format) const = 0;
	virtual U<Variable> clone() const = 0;
};


template <typename T>
class VariableCopy: public Variable
{
public:
	VariableCopy(const T value): value_(std::move(value)) {}
	
	std::string toString(const std::string& format) const override
	{
		return fs::toString(value_, format);
	}
	
	U<Variable> clone() const override
	{
		return mkU<VariableCopy>(value_);
	}

private:
	const T value_;
};


template <typename T>
class VariableReference: public Variable
{
public:
	VariableReference(W<const T> ref): reference_(std::move(ref)) {}
	
	std::string toString(const std::string& format) const override
	{
		S<const T> ptr = reference_.lock();
		if (ptr)
			return fs::toString(*ptr, format);
		else
			return "nullptr";
	}
	
	U<Variable> clone() const override
	{
		return mkU<VariableReference>(reference_);
	}

private:
	W<const T> reference_;
};


template <typename T>
class VariableRawReference: public Variable
{
public:
	VariableRawReference(P<const T> ref): reference_(ref) {}
	
	std::string toString(const std::string& format) const override
	{
		if (reference_)
			return fs::toString(*reference_, format);
		else
			return "nullptr";
	}
	
	U<Variable> clone() const override
	{
		return mkU<VariableRawReference>(reference_);
	}

private:
	P<const T> reference_;
};


class VariableFormat: public Variable
{
public:
	VariableFormat(U<Variable> var, std::string format):
			var_(std::move(var)), format_(std::move(format)) {}
	
	std::string toString(const std::string&) const override
	{
		return var_->toString(format_);
	}
	
	U<Variable> clone() const override
	{
		return mkU<VariableFormat>(var_->clone(), format_);
	}

private:
	U<Variable> var_;
	std::string format_;
};

} // namespace fs

#endif //FORMATSTRING_VARIABLE_H
