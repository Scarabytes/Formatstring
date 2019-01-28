// formatstring/err/FormatException.cpp
//
// Contains the implementation of the FormatException class.

#include <formatstring/err/FormatException.h>


namespace fs {
namespace err {



FormatException::FormatException(std::string message, std::string format, size_t pos):
		message_(std::move(message)),
		format_(std::move(format)),
		position_(pos),
		description_()
{
	description_ += "Format Exception: " + message_;
	
	if (!format_.empty()) {
		description_ += "\nIn formatstring: \"" + format_ + "\"";
		if (position_ != no_value) {
			description_ += "\n                  ";
			for (size_t i = 0; i < position_; ++i)
				description_ += ' ';
			description_ += '^';
		}
	}
}

std::string FormatException::getMessage() const
{
	return message_;
}

std::string FormatException::getFormat() const
{
	return format_;
}

size_t FormatException::getPosition() const
{
	return position_;
}

std::string FormatException::describe() const
{
	return description_;
}

const char* FormatException::what() const noexcept
{
	return description_.c_str();
}
	
} // namespace err
} // namespace fs
