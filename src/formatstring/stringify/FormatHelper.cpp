// formatstring/stringify/FormatHelper.cpp
//
// Implementation for the FormatHelper methods.

#include <formatstring/err/FormatException.h>
#include <formatstring/stringify/FormatHelper.h>

#include "formatstring/stringify/FormatHelper.h"

#include "formatstring/util/Assert.h"

namespace fs {

Numformat parseNumformat(const std::string& fmt)
{
	Numformat nf{};
	
	if (!fmt.empty()) {
		size_t l = fmt.length();
		size_t i = 0;
		
		// Alignment
		if (l > 0 && (fmt[0] == '<' || fmt[0] == '>' || fmt[0] == '^' || fmt[0] == '=')) {
			nf.align = fmt[0];
			i = 1;
			
		} else if (l > 1 && (fmt[1] == '<' || fmt[1] == '>' || fmt[1] == '^' || fmt[1] == '=')) {
			nf.align = fmt[1];
			nf.fill = fmt[0];
			i = 2;
		}
		
		// Sign
		if (l > i && (fmt[i] == '+' || fmt[i] == '-' || fmt[i] == ' ')) {
			nf.sign = fmt[i];
			++i;
		}
		
		// Alternate
		if (l > i && fmt[i] == '#') {
			nf.alternate = true;
			++i;
		}
		
		// Zero fill
		if (l > i && fmt[i] == '0') {
			nf.zero = true;
			++i;
		}
		
		// Width
		if (l > i && isdigit(fmt[i])) {
			nf.width = 0;
			while (l > i && isdigit(fmt[i])) {
				nf.width = nf.width * 10 + fmt[i] - '0';
				++i;
			}
		}
		
		// Precision
		if (l > i && fmt[i] == '.') {
			if (l > ++i && isdigit(fmt[i])) {
				nf.min_precision = 0;
				while (l > i && isdigit(fmt[i])) {
					nf.min_precision = nf.min_precision * 10 + fmt[i] - '0';
					++i;
				}
			} else {
				nf.min_precision = 0;
			}
			if (l > i && fmt[i] == '-') {
				// Min-max term
				size_t second_num = ++i;
				
				if (l <= i || !isdigit(fmt[i]))
					throw err::FormatException("Maximum precision expected after '-'",
							fmt, i);
				
				nf.max_precision = 0;
				while (l > i && isdigit(fmt[i])) {
					nf.max_precision = nf.max_precision * 10 + fmt[i] - '0';
					++i;
				}
				if (nf.max_precision < nf.min_precision)
					throw err::FormatException("Maximum precision less than minimum",
							fmt, second_num);
				
			} else {
				// Exact term
				nf.max_precision = nf.min_precision;
			}
		}
		
		// Type ID
		while (l > i && fmt[i] != ':')
			nf.type += fmt[i++];
		
		nf.parsed_until = i;
	}
	
	return nf;
	
}

Alignformat parseAlignformat(const std::string& fmt, size_t& i)
{
	Alignformat af{};
	
	size_t l = fmt.length();
	
	// Alignment
	if (l > 0 && (fmt[0] == '<' || fmt[0] == '>' || fmt[0] == '^')) {
		af.align = fmt[0];
		++i;
		
	} else if (l > 1 && (fmt[1] == '<' || fmt[1] == '>' || fmt[1] == '^')) {
		af.align = fmt[1];
		af.fill = fmt[0];
		i += 2;
	}
	
	// Width
	if (l > i && isdigit(fmt[i])) {
		af.width = 0;
		while (l > i && isdigit(fmt[i])) {
			af.width = af.width * 10 + fmt[i] - '0';
			++i;
		}
	}
	
	return af;
}

std::string readSingleQuotedString(const std::string& fmt, size_t& i)
{
	std::string out;
	size_t l = fmt.length();
	
	if (i < l && fmt[i] == '\'') {
		// Multi character
		++i;
		while (i < l && fmt[i] != '\'') {
			if (fmt[i] == '\\' && i+1 < l && fmt[i+1] == '\'') {
				out += '\'';
				++i;
			} else {
				out += fmt[i];
			}
			++i;
		}
		++i;
		
	} else {
		// Single character
		out = std::string(1, fmt[i++]);
	}
	return out;
}

std::string padStringToWidth(const std::string& source, const Alignformat& af,
		size_t center, char default_align)
{
	size_t width = static_cast<size_t>(af.width);
	
	if (af.width == -1 || source.length() >= width)
		return source;
	if (source.find('\n') != std::string::npos)
		return source;
	
	size_t padding = width - source.length();
	std::string out;
	out.reserve(width);
	
	char align = af.align;
	if (align == '\0')
		align = default_align;
	
	// Output leading padding if needed
	size_t leading_pad = 0;
	if (align == '>') {
		leading_pad = padding;
	} else if (align == '^') {
		leading_pad = padding / 2;
		padding -= leading_pad;
	}
	for (size_t i = 0; i < leading_pad; ++i)
		out += af.fill;
	
	// Output string
	if (align != '=') {
		out += source;
	} else {
		out.append(&source[0], center);
		for (size_t i = 0; i < padding; ++i)
			out += af.fill;
		out.append(&source[center], source.length() - center);
	}
	
	// Output trailing padding if needed
	if (align == '<' || align == '^') {
		for (size_t i = 0; i < padding; ++i)
			out += af.fill;
	}
	
	assertmsg(out.length() == width, "length is " << out.length() <<
	        " but should be " << width << "\n\"" << out << '"');
	
	return out;
}

} // namespace fs
