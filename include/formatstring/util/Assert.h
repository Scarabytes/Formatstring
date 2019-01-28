/** @file formatstring/util/Assert.h
 *
 * Defines a custom assertion macro with an additional debug message.
 */

#ifndef FORMATSTRING_ASSERT_H
#define FORMATSTRING_ASSERT_H

#ifndef NDEBUG

#include <sstream>
#include <cstdio>

#define assertmsg(condition, message) \
	if (!(condition)) {\
		std::stringstream _error_message;\
		_error_message << __FILE__ << ":" << __LINE__ << ": " << __func__ <<\
		"()\nAssertion failed: " << #condition << "\nMessage: " << message;\
		fputs(_error_message.str().c_str(), stderr);\
		std::abort();\
	}

#else

#define assertmsg(condition, message) ;

#endif // defined NDEBUG

#endif //FORMATSTRING_ASSERT_H
