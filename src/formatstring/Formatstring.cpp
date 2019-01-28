// formatstring/Formatstring.cpp
//
// Implementation for the Formatstring class.

#include "formatstring/Formatstring.h"

#include <iostream>

#include "formatstring/err/FormatException.h"
#include "formatstring/util/Assert.h"


namespace fs
{

Formatstring::Formatstring(std::string format):
		format_(std::move(format)),
		variables_(),
		segments_(),
		printed_(false)
{
	parseFormat();
}

Formatstring::Formatstring(const Formatstring& copy):
		format_(copy.format_),
		variables_(),
		segments_(copy.segments_),
		printed_(copy.printed_)
{
	variables_.resize(copy.variables_.size());
	for (size_t i = 0; i < copy.variables_.size(); ++i) {
		variables_[i] = copy.variables_[i]->clone();
	}
}

Formatstring::Formatstring(Formatstring&& move) noexcept:
		Formatstring()
{
	swap(move);
}

Formatstring& Formatstring::operator =(Formatstring assign)
{
	swap(assign);
	return *this;
}

void Formatstring::swap(Formatstring& rhs) noexcept
{
	using std::swap;
	swap(format_, rhs.format_);
	swap(variables_, rhs.variables_);
	swap(segments_, rhs.segments_);
	swap(printed_, rhs.printed_);
}

std::string Formatstring::getFormat() const
{
	return format_;
}

void Formatstring::setFormat(std::string format)
{
	format_ = std::move(format);
	parseFormat();
}

size_t Formatstring::countRequestedVariables() const
{
	size_t vars = 0;
	for (const Segment& s: segments_) {
		if (s.type == SegmentType::Variable)
			vars = std::max(vars, s.variable + 1);
	}
	return vars;
}

size_t Formatstring::countProvidedVariables() const
{
	return variables_.size();
}

bool Formatstring::wasPrinted() const
{
	return printed_;
}

void Formatstring::resetPrintedFlag()
{
	printed_ = false;
}

void Formatstring::clear()
{
	variables_.clear();
}

void Formatstring::addVariable(U<Variable> var)
{
	if (wasPrinted()) {
		clear();
		resetPrintedFlag();
	}
	
	variables_.emplace_back(std::move(var));
}

std::string escapedSubstring(const std::string& s, size_t begin, size_t end)
{
	assertmsg(begin <= s.length() && end <= s.length() && begin <= end,
			"Begin and end are out of range.\ns.length()=" << s.length()
			<< ", begin=" << begin << ", end=" << end);
	std::string out;
	out.reserve(end - begin);
	
	for (size_t i = begin; i < end; ++i) {
		char c = s[i];
		if (c == '{') {
			++i;
			assertmsg(i < end && s[i] == '{', "Input string wasn't escaped\n"<<s);
		} else if (c == '}') {
			++i;
			assertmsg(i < end && s[i] == '}', "Input string wasn't escaped\n"<<s);
		}
		out += c;
	}
	
	return out;
}

std::string Formatstring::str() const
{
	std::string out;
	
	for (const Segment& s: segments_) {
		switch (s.type)
		{
		case SegmentType::Substring:
			out += format_.substr(s.begin, s.end - s.begin);
			break;
			
		case SegmentType::Variable:
			if (s.variable >= variables_.size())
				throw err::FormatException("Not enough variables provided", format_);
			out += variables_[s.variable]->toString(escapedSubstring(format_, s.begin, s.end));
			break;
		}
	}
	
	printed_ = true;
	return out;
}

void Formatstring::write(std::ostream& stream) const
{
	stream << str();
}

void Formatstring::writeln(std::ostream& stream) const
{
	stream << str() << '\n';
}

void Formatstring::print() const
{
	write(std::cout);
}

void Formatstring::println() const
{
	writeln(std::cout);
}

Formatstring::operator std::string() const
{
	return str();
}

void Formatstring::parseFormat()
{
	segments_.clear();
	printed_ = false;
	
	size_t l = format_.length();
	size_t b = 0, i = 0;
	size_t var_counter = 0;
	
	while (l > i) {
		if (format_[i] == '{') {
			// New variable or escaped brace
			// 1. Emit last substring
			if (i > b)
				segments_.push_back({SegmentType::Substring, 0, b, i});
			
			// 2. Check if this is a variable or an escaped brace
			if (++i < l) {
				if (format_[i] == '{') {
					// Escaped brace. Start a new substring from here so that
					// the second brace character is included in it.
					b = i;
					
				} else {
					// Found a new variable!
					// Check for a variable ID first...
					size_t id;
					if (isdigit(format_[i])) {
						size_t begin = i;
						id = 0;
						while (i < l && isdigit(format_[i])) {
							id = id * 10 + format_[i] - '0';
							++i;
						}
						// IDs in formatter should start at 1, but they start at
						// 0 in code...
						if (id == 0)
							throw err::FormatException("Variable IDs start at 1",
									format_, begin);
						--id;
					} else {
						id = var_counter++;
					}
					
					Segment var{SegmentType::Variable, id, i, i};
					
					// If a colon follows, a format specifier can be expected
					// Otherwise, a closing brace is expected
					if (i < l && format_[i] == ':') {
						var.begin = ++i;
						while (i < l) {
							if (format_[i] == '}') {
								// Count the number of closing braces. If it is
								// odd, end the formatter at the first one. If
								// it is even, pass them on as formatting
								// elements.
								unsigned count = 1;
								unsigned off = 1;
								while (l > i + off && format_[i + off] == '}') {
									++count;
									++off;
								}
								if (count % 2) {
									// Odd number: this format object ends.
									var.end = i;
									break;
									
								} else {
									// Even number: add the escaped braces and
									// continue
									i += off - 1;
								}
							}
							++i;
						}
						if (i >= l)
							throw err::FormatException("EOF within variable specifier",
									format_, i);
						
					} else {
						if (i >= l)
							throw err::FormatException("EOF within variable specifier",
									format_, i);
						else if (format_[i] != '}')
							throw err::FormatException("Closing brace or colon expected",
									format_, i);
					}
					
					// At this point, the closing brace has been detected at the
					// current cursor position.
					segments_.push_back(var);
					b = i + 1;
				}
				
			} else {
				throw err::FormatException("EOF within variable specifier", format_, i);
			}
			
		} else if (format_[i] == '}') {
			// Expecting a escaped brace
			if (i + 1 >= l || format_[i + 1] != '}')
				throw err::FormatException("Unexpected closing brace", format_, i);
			// End the current substring and start the next
			if (i > b)
				segments_.push_back({SegmentType::Substring, 0, b, i});
			b = ++i;
		}
		
		++i;
	}
	
	if (i > b)
		segments_.push_back({SegmentType::Substring, 0, b, i});
}

void swap(Formatstring& lhs, Formatstring& rhs)
{
	lhs.swap(rhs);
}
	
} // namespace formatstring
