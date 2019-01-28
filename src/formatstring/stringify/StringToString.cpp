// formatstring/stringify/StringToString.cpp
//
// toString implementation for strings.

#include "formatstring/stringify/StringToString.h"

#include <vector>
#include <formatstring/err/FormatException.h>

#include "formatstring/stringify/FormatHelper.h"


namespace fs {

struct Stringformat: Alignformat {
	bool truncate {false};
	int substring_begin {-1};
	int substring_end {-1};
	std::vector<std::pair<std::string, std::string>> replacements {};
};

Stringformat parseStringFormat(const std::string& fmt) {
	Stringformat sf{};
	
	if (!fmt.empty()) {
		size_t l = fmt.length();
		size_t i = 0;
		
		// Alignment
		if (l > 0 && (fmt[0] == '<' || fmt[0] == '>' || fmt[0] == '^')) {
			sf.align = fmt[0];
			i = 1;
			
		} else if (l > 1 && (fmt[1] == '<' || fmt[1] == '>' || fmt[1] == '^')) {
			sf.align = fmt[1];
			sf.fill = fmt[0];
			i = 2;
		}
		
		// Truncation
		if (l > i && fmt[i] == '#') {
			sf.truncate = true;
			++i;
		}
		
		// Width
		if (l > i && isdigit(fmt[i])) {
			sf.width = 0;
			while (l > i && isdigit(fmt[i])) {
				sf.width = sf.width * 10 + fmt[i] - '0';
				++i;
			}
		}
		
		while (l > i && fmt[i] == ' ') ++i;
		
		// Substring
		if (l > i && fmt[i] == 's') {
			++i;
			int i1 = isdigit(fmt[i]) ? 0 : -1;
			while (l > i && isdigit(fmt[i])) {
				i1 = i1 * 10 + fmt[i] - '0';
				++i;
			}
			bool minus = fmt[i] == '-';
			if (minus) ++i;
			size_t second = i;
			int i2 = isdigit(fmt[i]) ? 0 : -1;
			while (l > i && isdigit(fmt[i])) {
				i2 = i2 * 10 + fmt[i] - '0';
				++i;
			}
			
			if (minus) {
				if (i2 < i1 && i2 != -1)
					throw err::FormatException("substring end less than begin",
							fmt, second);
				
				sf.substring_begin = std::max(0, i1);
				sf.substring_end = i2;
			} else {
				if (i1 > 0) {
					sf.substring_begin = 0;
					sf.substring_end = i1;
				}
			}
		}
		
		// Replacement
		while (l > i && fmt[i] == ' ') ++i;
		
		while (l > i && fmt[i] == 'r') {
			++i;
			std::string find = readSingleQuotedString(fmt, i);
			if (fmt[i] != '-')
				throw err::FormatException("'-' expected in replace expression",
						fmt, i);
			++i;
			std::string replace = readSingleQuotedString(fmt, i);
			sf.replacements.emplace_back(find, replace);
			
			while (l > i && fmt[i] == ' ') ++i;
		}
	}
	
	return sf;
}

std::string replace(
		std::string value, const std::string& find, const std::string& replace)
{
	std::string out;
	size_t i;
	
	for (i = 0; i <= value.length() - find.length(); ++i) {
		if (value[i] == find[0]) {
			if (value.compare(i, find.length(), find) == 0) {
				out += replace;
				i += find.length() - 1;
			} else {
				out += value[i];
			}
			
		} else {
			out += value[i];
		}
	}
	
	out.append(&value[i], value.length() - i);
	
	return out;
}

std::string str_string(const std::string& value, const std::string& format)
{
	if (format.empty())
		return value;
	
	Stringformat sf = parseStringFormat(format);
	
	std::string out;
	
	// Substring
	if (!value.empty() && sf.substring_begin != -1) {
		if (static_cast<size_t>(sf.substring_begin) < value.length()) {
			if (sf.substring_end == -1)
				out = value.substr(static_cast<size_t>(sf.substring_begin),
						static_cast<size_t>(value.length() - sf.substring_begin));
			else
				out = value.substr(static_cast<size_t>(sf.substring_begin),
						static_cast<size_t>(sf.substring_end - sf.substring_begin));
		}
	} else {
		out = value;
	}
	
	// Replacement
	if (!out.empty() && !sf.replacements.empty()) {
		for (auto replacement: sf.replacements)
			out = replace(out, replacement.first, replacement.second);
	}
	
	// Truncation or padding
	if (sf.width != -1 && out.length() > static_cast<size_t>(sf.width)) {
		return out.substr(0, static_cast<size_t>(sf.width));
	} else {
		return padStringToWidth(out, sf);
	}
}

std::string str(const char* value, const std::string& format)
{
	return str_string(value, format);
}

std::string str(unsigned char, const std::string&);

std::string str(char value, const std::string& format)
{
	if (!format.empty() && format[0] == 'i') {
		return str(static_cast<unsigned char>(value), format.substr(1));
	}
	
	return str_string(std::string(1, value), format);
}

} // namespace fs
