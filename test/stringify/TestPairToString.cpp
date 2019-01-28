// test/stringify/TestPairToString.cpp
//
// Test toString() for pairs

#include "catch2/catch.hpp"
#include "formatstring/ToString.h"


using namespace fs;

TEST_CASE("PairToString", "[toString][PairToString]")
{
	SECTION("Default usage") {
		auto a = std::make_pair(42, 1.5);
		CHECK(toString(a) == "(42, 1.5)");
	}
	
	SECTION("Width formatting") {
		auto a = std::make_pair(1, 2);
		CHECK(toString(a, "8") == "(1, 2)  ");
	}
	
	SECTION("Decorators") {
		auto a = std::make_pair("pi", 3.14159);
		CHECK(toString(a, "d'' ' = ' ''") == "pi = 3.14159");
	}
	
	SECTION("Forwarding") {
		auto a = std::make_pair(42, 3.5e-5);
		CHECK(toString(a, ":#x:si") == "(0x2a, 35u)");
		auto b = std::make_pair(std::vector<int>{1}, 1);
		CHECK(toString(b, ":\\:+") == "([+1], 1)");
	}
	
	SECTION("Exceptions") {
		std::pair<int, int> a{};
		CHECK_THROWS_WITH(toString(a, "d()"), Catch::Contains("Space expected"));
	}
}
