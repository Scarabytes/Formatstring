/** @file formatstring/err/FormatException.h
 *
 * A FormatException is thrown when a formatstring can't be parsed.
 */

#ifndef FORMATSTRING_FORMATEXCEPTION_H
#define FORMATSTRING_FORMATEXCEPTION_H

#include <exception>
#include <string>


namespace fs {
namespace err {

constexpr size_t no_value = static_cast<size_t>(-1);

class FormatException: public std::exception {
public:
	/** Constructs a new FormatException with the given data. */
	FormatException(std::string message, std::string format = {}, size_t pos = no_value);
	
	/** Returns the message of this FormatException. */
	std::string getMessage() const;
	
	/** Returns the format string that caused this exception. */
	std::string getFormat() const;
	
	/** Returns the position in the formatstring where the exception occurred.*/
	size_t getPosition() const;
	
	/** Returns a descriptive string of the error. */
	std::string describe() const;
	
	/** Returns the same as descibe() but as a c string. */
	const char* what() const noexcept;
	
private:
	std::string message_;
	std::string format_;
	size_t position_;
	std::string description_;
};

} // namespace err
} // namespace fs

#endif //FORMATSTRING_FORMATEXCEPTION_H
