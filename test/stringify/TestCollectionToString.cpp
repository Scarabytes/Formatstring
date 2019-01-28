// test/stringify/TestCollectionToString.cpp
//
// Test toString() for diverse collections.

#include "catch2/catch.hpp"
#include "formatstring/ToString.h"

#include <array>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <unordered_map>

TEST_CASE("CollectionToString", "[toString][CollectionToString]")
{
	SECTION("Standard usage") {
		std::array<int, 5> a {1, 2, 3, 4, 5};
		CHECK(fs::toString(a) == "[1, 2, 3, 4, 5]");
	}
	
	SECTION("Inline vs multiline") {
		std::vector<int> a {1, 2, 3};
		CHECK(fs::toString(a, "i") == "[1, 2, 3]");
		CHECK(fs::toString(a, "m") == "[\n1,\n2,\n3\n]");
	}
	
	SECTION("Width formatting") {
		std::list<int> a {1};
		CHECK(fs::toString(a, ">5") == "  [1]");
		CHECK(fs::toString(a, ">8m") == "[\n1\n]");
		std::vector<int> b {};
		CHECK(fs::toString(b, "^5m") == "[\n]");
		CHECK(fs::toString(b, "^5i") == " []  ");
	}
	
	SECTION("Custom decorators") {
		std::deque<int> a {1, 2};
		// Note: Not a width term, even though [[fill]align] would match.
		CHECK(fs::toString(a, "d< ' - ' >") == "<1 - 2>");
		std::array<int, 0> b {};
		CHECK(fs::toString(b, "d< ' - ' >") == "<>");
		CHECK(fs::toString(b, "D< ' - ' > '<!>'") == "<!>");
	}
	
	SECTION("Parameter forwarding") {
		std::set<int> a {12, 11, 10};
		CHECK(fs::toString(a, "i:x") == "[a, b, c]");
		CHECK(fs::toString(a, "m:o") == "[\n12,\n13,\n14\n]");
		CHECK(fs::toString(a, "d{ ; }:x") == "{a;b;c}");
	}
	
	SECTION("Exception") {
		std::list<int> a{};
		CHECK_THROWS_WITH(fs::toString(a, "d<>"), Catch::Contains("Space expected"));
	}
}

TEST_CASE("MapToString", "[toString][MapToString]")
{
	SECTION("Standard usage") {
		std::map<int, char> a {{1, 'a'}, {2, 'b'}};
		CHECK(fs::toString(a) == "[1: a, 2: b]");
	}
	
	SECTION("Inline vs multiline") {
		std::map<char, int> a {{'x', 42}, {'y', 0}};
		CHECK(fs::toString(a, "i") == "[x: 42, y: 0]");
		CHECK(fs::toString(a, "m") == "[\nx: 42,\ny: 0\n]");
	}
	
	SECTION("Width formatting") {
		std::unordered_map<int, int> a {{1, 2}};
		CHECK(fs::toString(a, ">8") == "  [1: 2]");
		CHECK(fs::toString(a, ">20m") == "[\n1: 2\n]");
		std::map<int, int> b {};
		CHECK(fs::toString(b, "^5m") == "[\n]");
		CHECK(fs::toString(b, "^5i") == " []  ");
	}
	
	SECTION("Custom decorators") {
		std::map<const char*, float> a {{"pi", 3.14159}, {"e", 2.71828}};
		CHECK(fs::toString(a, "d'Constants: ' ' = ' '; ' ''") == "Constants: pi = 3.14159; e = 2.71828");
		std::map<int, int> b {};
		// Note: Not a width term, even though [[fill]align] would match.
		CHECK(fs::toString(b, "d< = ' - ' >") == "<>");
		CHECK(fs::toString(b, "D< = ' - ' > '<!>'") == "<!>");
	}
	
	SECTION("Parameter forwarding") {
		std::multimap<const char*, int> a {{"Alice", 42}, {"Bob", 12}, {"Alice", -10}};
		CHECK(fs::toString(a, "i:.<5:+") == "[Alice: +42, Alice: -10, Bob..: +12]");
		CHECK(fs::toString(a, "d{ : ; }:#1:#x") == "{A:0x2a;A:-0xa;B:0xc}");
	}
	
	SECTION("Exception") {
		std::map<int, int> a{};
		CHECK_THROWS_WITH(fs::toString(a, "d<>"), Catch::Contains("Space expected"));
	}
}
