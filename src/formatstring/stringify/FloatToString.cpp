// FloatToString.cpp
//
// toString implementation for floating-point numbers.

#include "formatstring/stringify/FloatToString.h"

#include <algorithm>

#include "formatstring/QuickFormat.h"
#include "formatstring/err/FormatException.h"
#include "formatstring/stringify/FormatHelper.h"
#include "formatstring/util/Assert.h"


namespace fs
{

using detail::fp;
using detail::decimal;

struct decomposition {
	fp v;
	bool sign;
	bool special;
};

decomposition decomposeFloat(float f)
{
	static_assert(std::numeric_limits<float>::is_iec559, "float is not IEEE754");
	static_assert(sizeof(float) == sizeof(int32_t), "float is not 32bit");
	
	uint32_t bits = reinterpret_cast<uint32_t&>(f);
	
	bool sign = (bits & (1 << 31)) != 0;
	
	int32_t e_ieee = (bits >> 23) & 0xff;
	uint32_t f_ieee = (bits & 0x7fffff);
	
	bool special = false;
	if (e_ieee == 0xff)
		special = true;
	else if (e_ieee != 0)
		f_ieee |= 0x800000; // Add hidden bit
	else
		f_ieee <<= 1; // Denormal; shift into position of standard number
	
	return {{f_ieee, e_ieee - 127 - 23}, sign, special};
}

decomposition decomposeFloat(double f)
{
	static_assert(std::numeric_limits<double>::is_iec559, "double is not IEEE754");
	static_assert(sizeof(double) == sizeof(int64_t), "double is not 64bit");
	
	uint64_t bits = reinterpret_cast<uint64_t&>(f);
	
	bool sign = (bits & (1ull << 63)) != 0;
	
	int32_t e_ieee = static_cast<int32_t>((bits >> 52) & 0x7ff);
	uint64_t f_ieee = (bits & ~(0xfffull << 52));
	
	bool special = false;
	if (e_ieee == 0x7ff)
		special = true;
	else if (e_ieee != 0)
		f_ieee |= 1ull << 52; // Add hidden bit
	else
		f_ieee <<= 1; // Denormal; shift into position of standard number
	
	return {{f_ieee, e_ieee - 1023 - 52}, sign, special};
}

decomposition decomposeFloat(long double f)
{
	// Maybe add special method in the future?
	// For now it's too much effort for too little payoff.
	return decomposeFloat(static_cast<double>(f));
}

//------------------------------------------------------------------------------

bool roundDecimal(decimal& d, int lsd_exponent)
{
	bool changed_msd_or_lsd = false;
	char last = '0';
	while (lsd_exponent > d.exponent) {
		last = d.digits.back();
		d.digits.pop_back();
		++d.exponent;
	}
	if (last >= '5' && last <= '9')
		++d.digits[d.digits.length() - 1];
	while (d.digits.length() > 1 && d.digits[d.digits.length() - 1] == '9' + 1) {
		d.digits.pop_back();
		++d.digits[d.digits.length() - 1];
		++d.exponent;
		changed_msd_or_lsd = true;
	}
	if (d.digits.length() == 1 && d.digits[0] == '9' + 1) {
		d.digits = "1";
		++ d.exponent;
		changed_msd_or_lsd = true;
	}
	if (d.digits.length() == 0) {
		d.digits = "0";
		d.exponent = 0;
		changed_msd_or_lsd = true;
	}
	
	return changed_msd_or_lsd;
}

std::string specialToString(decomposition d, Numformat nf)
{
	if (d.special && d.v.f == 0) {
		std::string out;
		
		// Output sign
		if (d.sign) {
			out += '-';
		} else {
			if (nf.sign == '+')
				out += '+';
			else if (nf.sign == ' ')
				out += ' ';
		}
		
		size_t center = out.length();
		out += "Inf";
		
		return padStringToWidth(out, nf, center, '>');
		
	} else if (d.special && d.v.f != 0) {
		return padStringToWidth("NaN", nf);
		
	} else {
		assertmsg(d.special, "specialToString() called on normal float.");
		return {};
	}
}

std::string floatToFixed(decimal d, bool negative, const std::string& type, Numformat nf)
{
	// Determine print boundaries and length
	int num_digits = static_cast<int>(d.digits.length());
	int msd_exponent = std::max(d.exponent + num_digits - 1, 0);
	int lsd_exponent = std::min(d.exponent, 0);
	int round_lsd_exponent = lsd_exponent;
	
	if (type == "f") {
		// Precision defines number of decimal digits
		if (nf.max_precision != -1)
			lsd_exponent = std::max(lsd_exponent, -nf.max_precision);
		if (nf.min_precision != -1)
			lsd_exponent = std::min(lsd_exponent, -nf.min_precision);
		
		round_lsd_exponent = lsd_exponent;
		
	} else {
		// Precision defines number of significant digits
		if (nf.max_precision != -1)
			round_lsd_exponent = std::max(round_lsd_exponent, msd_exponent + 1 - nf.max_precision);
		if (nf.min_precision != -1)
			round_lsd_exponent = std::min(round_lsd_exponent, msd_exponent + 1 - nf.min_precision);
		
		lsd_exponent = std::min(round_lsd_exponent, 0);
	}
	
	assertmsg(lsd_exponent <= 0, "lsd_exp=" << lsd_exponent);
	
	// Round the number to the needed number of digits
	if (roundDecimal(d, round_lsd_exponent)) {
		// Readjust values on significant changes
		int digits = static_cast<int>(d.digits.length());
		msd_exponent = std::max(digits + d.exponent - 1, msd_exponent);
		
		if (d.exponent > round_lsd_exponent) {
			int max_lsd_exponent = 0;
			if (nf.min_precision != -1) {
				if (type == "f")
					max_lsd_exponent = -nf.min_precision;
				else
					max_lsd_exponent = msd_exponent + 1 - nf.min_precision;
			}
			lsd_exponent = std::min(max_lsd_exponent, d.exponent);
		}
	}
	
	//--------------------------------------------------------------------------
	// Construct a string from the data
	
	if (nf.zero) {
		nf.align = '=';
		nf.fill = '0';
	}
	
	std::string out;
	
	// Output sign
	if (negative) {
		out += '-';
	} else {
		if (nf.sign == '+')
			out += '+';
		else if (nf.sign == ' ')
			out += ' ';
	}
	
	size_t center = out.length();
	
	// Output digits
	for (int exp = msd_exponent; exp >= lsd_exponent; --exp) {
		if (exp == -1)
			out += '.';
		char c = '0';
		int index = static_cast<int>(d.digits.length()) - exp - 1 + d.exponent;
		if (index >= 0 && index < static_cast<int>(d.digits.length()))
			c = d.digits[index];
		out += c;
	}
	
	if (nf.alternate && lsd_exponent >= 0)
		out += '.';
	
	return padStringToWidth(out, nf, center, '>');
}

std::string floatToScientific(decimal d, bool negative, const std::string& type, Numformat nf)
{
	// Determine print boundaries and length
	int num_digits = static_cast<int>(d.digits.length());
	int msd_exponent = d.exponent + num_digits - 1;
	int lsd_exponent = d.exponent;
	
	// Precision defines number of significant digits
	if (nf.max_precision != -1)
		lsd_exponent = std::max(lsd_exponent, msd_exponent + 1 - nf.max_precision);
	if (nf.min_precision != -1)
		lsd_exponent = std::min(lsd_exponent, msd_exponent + 1 - nf.min_precision);
	
	// Round the number to the needed number of digits
	if (roundDecimal(d, lsd_exponent)) {
		// Readjust values on significant changes
		int digits = static_cast<int>(d.digits.length());
		msd_exponent = std::max(digits + d.exponent - 1, msd_exponent);
		
		if (d.exponent > lsd_exponent) {
			int max_lsd_exponent = 0;
			if (nf.min_precision != -1)
				max_lsd_exponent = msd_exponent + 1 - nf.min_precision;
			lsd_exponent = std::min(max_lsd_exponent, d.exponent);
		}
	}
	
	// Determining the exponent to display
	int display_exponent = msd_exponent;
	if (type == "ee" || type == "si") {
		if (display_exponent > 0)
			display_exponent = (display_exponent / 3) * 3;
		else
			display_exponent = ((display_exponent - 2) / 3) * 3;
		assertmsg(display_exponent % 3 == 0, "exponent=" << display_exponent);
		lsd_exponent = std::min(lsd_exponent, display_exponent);
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
	
	size_t center = out.length();
	
	// Output digits
	for (int exp = msd_exponent; exp >= lsd_exponent; --exp) {
		if (exp == display_exponent - 1)
			out += '.';
		char c = '0';
		int index = static_cast<int>(d.digits.length()) - exp - 1 + d.exponent;
		if (index >= 0 && index < static_cast<int>(d.digits.length()))
			c = d.digits[index];
		out += c;
	}
	
	if (nf.alternate)
		out += '.';
	
	// Output exponent
	bool done = false;
	if (type == "si") {
		done = true;
		switch (display_exponent) {
		case -24: out += 'y'; break;   // yocto = 10^-24
		case -21: out += 'z'; break;   // zepto = 10^-21
		case -18: out += 'a'; break;   // atto  = 10^-18
		case -15: out += 'f'; break;   // femto = 10^-15
		case -12: out += 'p'; break;   // pico  = 10^-12
		case -9:  out += 'n'; break;   // nano  = 10^-9
		case -6:  out += 'u'; break;   // micro = 10^-6
		case -3:  out += 'm'; break;   // milli = 10^-3
		case 0:   	          break;
		case 3:   out += 'k'; break;   // kilo  = 10^3
		case 6:   out += 'M'; break;   // mega  = 10^6
		case 9:   out += 'G'; break;   // giga  = 10^9
		case 12:  out += 'T'; break;   // tera  = 10^12
		case 15:  out += 'P'; break;   // peta  = 10^15
		case 18:  out += 'E'; break;   // exa   = 10^18
		case 21:  out += 'Z'; break;   // zetta = 10^21
		case 24:  out += 'Y'; break;   // yotta = 10^24
		default:
			done = false;
		}
	}
	
	if (!done) {
		if (isupper(nf.type[0]))
			out += 'E';
		else
			out += 'e';
		
		// Output the exponent's value
		bool exp_sign = display_exponent < 0;
		if (exp_sign) {
			out += '-';
			display_exponent = -display_exponent;
		} else if (nf.sign == '+') {
			out += '+';
		}
		
		bool printing = false;
		unsigned exponentMagnitude = 1000;
		while (exponentMagnitude > 0) {
			int digit = display_exponent / exponentMagnitude;
			display_exponent %= exponentMagnitude;
			exponentMagnitude /= 10;
			if (digit != 0 || exponentMagnitude == 0)
				printing = true;
			if (printing)
				out += static_cast<char>(digit + '0');
		}
		assertmsg(display_exponent == 0, "exponent=" << display_exponent);
	}
	
	return padStringToWidth(out, nf, center, '>');
}

template <typename T>
std::string floatToString(T value, const std::string& format)
{
	// Parse format parameters
	Numformat nf = parseNumformat(format);
	std::string type = nf.type;
	std::transform(type.begin(), type.end(), type.begin(), tolower);
	
	if (!type.empty() && type != "g" && type != "e" && type != "f" && type != "ee" && type != "si")
		throw err::FormatException("Unknown type parameter \"" + nf.type + "\"",
				format, nf.parsed_until - nf.type.length());
	
	// Check for special types
	decomposition d = decomposeFloat(value);
	if (d.special)
		return specialToString(d, nf);
	
	// Generate digits
	decimal dec = detail::grisu2(d.v);
	
	// Decide on fixed or scientific style
	if (type == "e" || type == "ee" || type == "si") {
		return floatToScientific(dec, d.sign, type, nf);
	} else if (type == "f") {
		return floatToFixed(dec, d.sign, type, nf);
	} else {
		if (value != 0 && (std::abs(value) < 1e-3 || std::abs(value) >= 1e10))
			return floatToScientific(dec, d.sign, type, nf);
		else
			return floatToFixed(dec, d.sign, type, nf);
	}
}

std::string str(float value, const std::string& format)
{
	return floatToString(value, format);
}

std::string str(double value, const std::string& format)
{
	return floatToString(value, format);
}

std::string str(long double value, const std::string& format)
{
	return floatToString(value, format);
}
	
} // namespace fs
