// test/TestFormatException.cpp
//
// Tests the FormatException class

#include "catch2/catch.hpp"
#include "formatstring/err/FormatException.h"

#include <cstring>

using namespace fs::err;

TEST_CASE("Format Exception string output", "[FormatException]") {
	
	FormatException e1("Msg1", "abc{}");
	FormatException e2("Msg2", "def{}", 3);
	
	SECTION("Standard Getters") {
		CHECK(e1.getMessage() == "Msg1");
		CHECK(e1.getFormat() == "abc{}");
		CHECK(e1.getPosition() == no_value);
		CHECK(e2.getMessage() == "Msg2");
		CHECK(e2.getFormat() == "def{}");
		CHECK(e2.getPosition() == 3);
	}
	
	SECTION("Formatted Getter") {
		const char expect1[] = "Format Exception: Msg1\n"
						       "In formatstring: \"abc{}\"";
		const char expect2[] = "Format Exception: Msg2\n"
						 	   "In formatstring: \"def{}\"\n"
		   					   "                     ^";
		
		CHECK(e1.describe() == expect1);
		CHECK(strcmp(e1.what(), expect1) == 0);
		CHECK(e2.describe() == expect2);
		CHECK(strcmp(e2.what(), expect2) == 0);
	}
}
