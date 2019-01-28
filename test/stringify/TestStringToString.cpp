// test/stringify/TestStringToString.cpp
//
// Tests the formatting of strings.

#include "catch2/catch.hpp"
#include "formatstring/ToString.h"


using namespace fs;

TEST_CASE("CharacterToString", "[toString][CharacterToString]")
{

}

TEST_CASE("StringToString", "[toString][StringToString]")
{
	SECTION("Handles std::string and chars") {
		std::string std("std::string");
		const char* cstr = "const char*";
		CHECK(toString(std) == "std::string");
		CHECK(toString(cstr) == "const char*");
		CHECK(toString('x') == "x");
	}
	
	SECTION("Chars can be converted to ints") {
		CHECK(toString('A', "i#x") == "0x41");
	}
	
	SECTION("Width formatting") {
		CHECK(toString("abc", "5") == "abc  ");
		CHECK(toString("abc", ".^6") == ".abc..");
		CHECK(toString("abcdef", "#3") == "abc");
	}
	
	SECTION("Substringing") {
		CHECK(toString("Hello World", "s3-8") == "lo Wo");
		CHECK(toString("Hello World", "s-5") == "Hello");
		CHECK(toString("Hello World", "s6-") == "World");
		CHECK(toString("Hello World", "s5") == "Hello");
		CHECK(toString("Hello", "s-10") == "Hello");
		CHECK(toString("Hi", "s3-").empty());
	}
	
	SECTION("Replacements") {
		CHECK(toString("Hellu Wurld", "ru-o") == "Hello World");
		CHECK(toString("Hello *", "r*-'Replace'") == "Hello Replace");
		CHECK(toString("Full Word!", "r'Word'-'Replace'") == "Full Replace!");
		CHECK(toString("bebegin", "r'begin'-'st!'") == "best!");
		CHECK(toString("multi replace", "r'multi'-'really' r'replace'-'cool'") == "really cool");
		CHECK(toString("abc", "ra-b r'bb'-'cc'") == "ccc");
		CHECK(toString("foo bar baz", "r' '-''") == "foobarbaz");
	}
	
	SECTION("Exceptions") {
		CHECK_THROWS_WITH(toString("", "ra"), Catch::Contains("'-' expected in replace expression"));
		CHECK_THROWS_WITH(toString("", "s4-2"), Catch::Contains("substring end less than begin"));
	}
}
