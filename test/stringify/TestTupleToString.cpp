// test/stringify/TestTupleToString.cpp
//
// Tests toString() for tuples.

#include "catch2/catch.hpp"
#include "formatstring/ToString.h"


using namespace fs;

TEST_CASE("TupleToString", "[toString][TupleToString]")
{
	SECTION("Default usage") {
		auto a = std::make_tuple(1, 'a', true);
		CHECK(toString(a) == "(1, a, true)");
		auto b = std::make_tuple();
		CHECK(toString(b) == "()");
	}
	
	SECTION("Width formatting") {
		auto a = std::make_tuple(1, 2);
		CHECK(toString(a, "8") == "(1, 2)  ");
	}
	
	SECTION("Decorators") {
		auto a = std::make_tuple(3.7, 'x', "foo");
		CHECK(toString(a, "d[ '; ' ]") == "[3.7; x; foo]");
	}
	
	SECTION("Forwarding") {
		auto a = std::make_tuple(3, 10, 42);
		CHECK(toString(a, ":b:o:x") == "(11, 12, 2a)");
	}
	
	SECTION("Exceptions") {
		std::tuple<> a{};
		CHECK_THROWS_WITH(toString(a, "d()"), Catch::Contains("Space expected"));
	}
}
