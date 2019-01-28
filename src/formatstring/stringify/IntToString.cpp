// formatstring/stringify/IntToString.cpp
//
// toString implementation for integers.

#include "formatstring/stringify/IntToString.h"

#include <limits>
#include <iostream>

#include "formatstring/err/FormatException.h"
#include "formatstring/stringify/FormatHelper.h"
#include "formatstring/util/Assert.h"

namespace fs
{

template <typename T>
std::string intToString(T value, const std::string& format)
{
	// Parse format parameter
	Numformat nf = parseNumformat(format);
	if (!nf.type.empty() && nf.type != "d" && nf.type != "x" && nf.type != "X"
						 && nf.type != "o" && nf.type != "b")
		throw err::FormatException("Unknown type parameter \"" + nf.type + "\"",
				format, nf.parsed_until - nf.type.length());
	
	// Generate digits
	// Get unsigned type
	using UT = typename std::make_unsigned<T>::type;
	
	// Define base and maximum number of digits
	int base = 10;
	int maxDigits = std::numeric_limits<UT>::digits10 + 1;
	if (nf.type == "b") {
		base = 2;
		maxDigits = std::numeric_limits<UT>::digits;
	} else if (nf.type == "o") {
		base = 8;
		maxDigits = std::numeric_limits<UT>::digits / 3;
	} else if (nf.type == "x" || nf.type == "X") {
		base = 16;
		maxDigits = std::numeric_limits<UT>::digits / 4;
	}
	
	char digits[maxDigits];
	
	// Define lookup
	const char* lowercase_lookup = "0123456789abcdef";
	const char* uppercase_lookup = "0123456789ABCDEF";
	const char* lookup = nf.type == "X" ? uppercase_lookup : lowercase_lookup;
	
	// Check for negative values
	bool negative = value < 0;
	UT absValue = negative ? -value : value;
	
	// Fill the buffer from the end
	int index = 0;
	while (absValue > 0) {
		int digit = absValue % base;
		assertmsg(digit < base, "digit=" << digit << ", base=" << base);
		absValue /= base;
		
		++index;
		assertmsg(index <= maxDigits, "index=" << index << ", base" << base);
		digits[maxDigits - index] = lookup[digit];
	}
	
	//--------------------------------------------------------------------------
	// Construct a string from the data
	std::string out;
	
	if (nf.zero) {
		nf.align = '=';
		nf.fill = '0';
	}
	
	// Output sign
	if (negative) {
		out += '-';
	} else {
		if (nf.sign == '+')
			out += '+';
		else if (nf.sign == ' ')
			out += ' ';
	}
	
	// Output base if needed
	if (nf.alternate && base != 10) {
		switch (base) {
		case 2:  out += "0b"; break;
		case 8:  out += "0o"; break;
		case 16: out += "0x"; break;
		}
	}
	
	size_t center = out.length();
	
	// Output digits
	if (value == 0)
		out += '0';
	else
		out.append(&digits[maxDigits - index], static_cast<size_t>(index));
	
	return padStringToWidth(out, nf, center, '>');
}


std::string str(signed char value, const std::string& format)
{
	return intToString(value, format);
}

std::string str(signed short value, const std::string& format)
{
	return intToString(value, format);
}

std::string str(signed int value, const std::string& format)
{
	return intToString(value, format);
}

std::string str(signed long value, const std::string& format)
{
	return intToString(value, format);
}

std::string str(signed long long value, const std::string& format)
{
	return intToString(value, format);
}

std::string str(unsigned char value, const std::string& format)
{
	return intToString(value, format);
}

std::string str(unsigned short value, const std::string& format)
{
	return intToString(value, format);
}

std::string str(unsigned int value, const std::string& format)
{
	return intToString(value, format);
}

std::string str(unsigned long value, const std::string& format)
{
	return intToString(value, format);
}

std::string str(unsigned long long value, const std::string& format)
{
	return intToString(value, format);
}
	
} // namespace fs
