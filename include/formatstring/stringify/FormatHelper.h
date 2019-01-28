/** @file formatstring/stringify/FormatHelper.h
 *
 * Contains helper methods for formatting values.
 */

#ifndef FORMATSTRING_FORMATHELPER_H
#define FORMATSTRING_FORMATHELPER_H

#include <string>


namespace fs
{

/** A struct defining how an output string should be aligned in a given width.*/
struct Alignformat
{
	int width {-1};
	char align {'\0'};
	char fill {' '};
};

/** A struct to store the data from the standard numeric format. */
struct Numformat: Alignformat
{
	char sign {'-'};
	int max_precision {-1};
	int min_precision {-1};
	std::string type {};
	bool alternate {false};
	bool zero {false};
	size_t parsed_until {0};
};

/** Parses the standard numberformat as described in FloatToString.h. */
Numformat parseNumformat(const std::string& fmt);

/** Parses the standard alignment format used by most types. */
Alignformat parseAlignformat(const std::string& fmt, size_t& i);

/** Reads a single character or a string within single quotes. */
std::string readSingleQuotedString(const std::string& fmt, size_t& i);

/**
 * Inserts padding characters around the given string to bring it up to the
 * specified width. The center parameter determines at which point padding chars
 * could be inserted into the string, e.g. for sign aware padding (+____4.3).
 */
std::string padStringToWidth(const std::string& source, const Alignformat& af,
		size_t center = 0, char default_align = '<');

} // namespace fs

#endif //FORMATSTRING_FORMATHELPER_H
