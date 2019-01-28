/** @file formatstring/Formatstring.h
 *
 * The Formatstring class takes a formatting string argument and variables, to
 * combine those to one formatted output string.
 */

#ifndef FORMATSTRING_FORMATSTRING_H
#define FORMATSTRING_FORMATSTRING_H

#include <string>
#include <iosfwd>
#include <vector>

#include "formatstring/util/PointerUtil.h"
#include "formatstring/detail/Variable.h"


namespace fs {

class Formatstring
{
public:
	/** Creates a new Formatstring with the given format. */
	explicit Formatstring(std::string format = {});
	
	/** Copies the given Formatstring including variables. */
	Formatstring(const Formatstring& copy);
	/** Moves the given Formatstring. */
	Formatstring(Formatstring&& move) noexcept;
	/** Assigns the given Formatstring to this one. */
	Formatstring& operator=(Formatstring assign);
	
	/** Swaps the given Formatstring with this one. */
	void swap(Formatstring& rhs) noexcept;
	
	//--------------------------------------------------------------------------
	
	/** Returns the current format string. */
	std::string getFormat() const;
	/** Sets a new format string. This causes the parser to run again. */
	void setFormat(std::string format);
	
	
	/** Counts the number of variables requested by the current formatstring. */
	size_t countRequestedVariables() const;
	/** Counts the number of variables currently stored in this object. */
	size_t countProvidedVariables() const;
	
	
	/**
	 * Returns whether this specific formatstring with its variables was
	 * printed already. If that is the case, the current variables will be
	 * erased if a new one is added. This flag is reset automatically when the
	 * format string is changed or variables are added/removed.
	 */
	bool wasPrinted() const;
	/** Resets the wasPrinted flag. */
	void resetPrintedFlag();
	
	
	/** Removes all stored variables from this Formatstring. */
	void clear();
	
	
	/** Adds the given variable to this Formatstring. */
	void addVariable(U<Variable> var);
	
	
	/** Creates a string with the output of this Formatstring. */
	std::string str() const;
	/** Writes the output of this Formatstring to the given stream. */
	void write(std::ostream& stream) const;
	/**
	 * Writes the output of this Formatstring to the given stream and appends a
	 * newline.
	 */
	void writeln(std::ostream& stream) const;
	/** Writes the output of this Formatstring to stdout. */
	void print() const;
	/** Writes the output of this Formatstring to stdout and appends a newline. */
	void println() const;
	/** Converts this Formatstring to a string with its output. */
	operator std::string() const;
	
	
	// Templates and inlines
	
	/** Adds the given argument as a variable to this Formatstring. */
	inline Formatstring& arg(U<Variable> var)
	{
		addVariable(std::move(var));
		return *this;
	}
	
	/**
	 * Adds the given argument as a variable to this Formatstring. The value
	 * will be copied!
	 */
	template <typename T>
	inline Formatstring& arg(const T& obj)
	{
		U<Variable> v = mkU<VariableCopy<T>>(obj);
		arg(std::move(v));
		return *this;
	}
	
	/** Adds the given arguments as variables to this Formatstring. */
	template <typename T, typename... Rest>
	inline Formatstring& args(T&& first, Rest&&... rest)
	{
		arg(std::forward<T>(first));
		args(std::forward<Rest>(rest)...);
		return *this;
	}
	
	/** Recursion end condition. */
	inline Formatstring& args() { return *this; }
	
	/** Adds the given argument as a variable to this Formatstring. */
	template <typename T>
	inline Formatstring& operator%(T&& obj)
	{
		return arg(std::forward<T>(obj));
	}
	
private:
	enum class SegmentType {
		Substring, Variable
	};
	
	struct Segment {
		SegmentType type;
		size_t variable;
		size_t begin;
		size_t end;
	};
	
	void parseFormat();
	
	
	std::string format_;
	std::vector<U<Variable>> variables_;
	std::vector<Segment> segments_;
	mutable bool printed_;
};

void swap(Formatstring& lhs, Formatstring& rhs);

} // namespace fs

#endif //FORMATSTRING_FORMATSTRING_H
