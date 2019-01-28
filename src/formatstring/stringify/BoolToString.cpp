// formatstring/stringify/BoolToString.cpp
//
// toString implementation for bool

#include "formatstring/stringify/BoolToString.h"

#include "formatstring/err/FormatException.h"
#include "formatstring/stringify/FormatHelper.h"


namespace fs {

std::string str(bool value, const std::string& fmt)
{
	size_t l = fmt.length();
	size_t i = 0;
	Alignformat af{};
	if (!fmt.empty() && fmt[0] != 'd' && fmt[0] != 'D')
		af = parseAlignformat(fmt, i);
	
	std::string ntrue, nfalse;
	
	if (i < l && fmt[i] == 'n') {
		++i;
		ntrue = readSingleQuotedString(fmt, i);
		if (i >= l || fmt[i++] != ' ')
			throw err::FormatException("Space expected", fmt, i-1);
		nfalse = readSingleQuotedString(fmt, i);
	} else {
		ntrue = "true";
		nfalse = "false";
	}
	
	return padStringToWidth(value ? ntrue : nfalse, af);
}

} // namespace fs
