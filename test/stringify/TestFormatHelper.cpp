// test/stringify/TestFormatHelper.cpp
//
// Tests the FormatHelper header.

#include "catch2/catch.hpp"
#include "formatstring/stringify/FormatHelper.h"


using namespace fs;

TEST_CASE("FormatHelper Numberformat", "[Helper][Numformat]")
{
	Numformat nf{};
	
	SECTION("Defaults")
	{
		CHECK(nf.fill == ' ');
		CHECK(nf.align == '\0');
		CHECK(nf.sign == '-');
		CHECK(!nf.alternate);
		CHECK(!nf.zero);
		CHECK(nf.min_precision == -1);
		CHECK(nf.max_precision == -1);
		CHECK(nf.width == -1);
		CHECK(nf.type.empty());
		CHECK(nf.parsed_until == 0);
	}
	
	SECTION("Parsing")
	{
		nf = parseNumformat(".= #020.6X");
		CHECK(nf.fill == '.');
		CHECK(nf.align == '=');
		CHECK(nf.sign == ' ');
		CHECK(nf.alternate);
		CHECK(nf.zero);
		CHECK(nf.min_precision == 6);
		CHECK(nf.max_precision == 6);
		CHECK(nf.width == 20);
		CHECK(nf.type == "X");
		CHECK(nf.parsed_until == 10);
		
		nf = parseNumformat(".5-10");
		CHECK(nf.min_precision == 5);
		CHECK(nf.max_precision == 10);
		
		nf = parseNumformat(".-5");
		CHECK(nf.max_precision == 5);
		CHECK(nf.min_precision == 0);
		
		CHECK_THROWS_WITH(parseNumformat(".3-"), Catch::Contains("Maximum precision expected"));
		CHECK_THROWS_WITH(parseNumformat(".3-2"), Catch::Contains("Maximum precision less than minimum"));
		
		nf = parseNumformat("test:abc");
		CHECK(nf.type == "test");
		CHECK(nf.parsed_until == 4);
	}
}
