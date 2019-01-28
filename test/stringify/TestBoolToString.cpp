// test/stringify/TestBoolToString.cpp
//
// Test toString() for bool.

#include "catch2/catch.hpp"
#include "formatstring/ToString.h"


using namespace fs;

TEST_CASE("BoolToString", "[toString][BoolToString]")
{
	SECTION("Default usage") {
		CHECK(toString(true) == "true");
		CHECK(toString(false) == "false");
	}
	
	SECTION("Width formatting") {
		CHECK(toString(true, "5") == "true ");
	}
	
	SECTION("Custom names") {
		CHECK(toString(true, "n1 0") == "1");
		CHECK(toString(false, "n'yea' 'nay'") == "nay");
	}
	
	SECTION("Exceptions") {
		CHECK_THROWS_WITH(toString(true, "nab"), Catch::Contains("Space expected"));
	}
}
