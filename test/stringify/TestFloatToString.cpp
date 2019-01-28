// test/stringify/TestFloatToString.cpp
//
// Tests the conversion of floats to string

#include "catch2/catch.hpp"
#include "formatstring/ToString.h"


using namespace fs;

TEST_CASE("FloatToString", "[toString][FloatToString]")
{
	SECTION("Floating point types") {
		CHECK(toString(3.14159) == "3.14159");
		CHECK(toString(42.42f) == "42.42");
	}
	
	SECTION("Values at the end of the range") {
		CHECK(toString(1.537e308) == "1.537e308");
		CHECK(toString(3.445e-308) == "3.445e-308");
		CHECK(toString(-0.) == "-0");
	}
	
	SECTION("Special values") {
		CHECK(toString(1.0 / 0.0) == "Inf");
		CHECK(toString(-1.0 / 0.0) == "-Inf");
		CHECK(toString(0.0 / 0.0) == "NaN");
	}
	
	SECTION("Formatting") {
		SECTION("Alignment") {
			CHECK(toString(3.14, "7") ==  "   3.14");
			CHECK(toString(3.14, ">7") == "   3.14");
			CHECK(toString(3.14, "<7") == "3.14   ");
			CHECK(toString(3.14, "^7") == " 3.14  ");
			CHECK(toString(3.14, "=7") == "   3.14");
			
			CHECK(toString(-3.14, ".>8") == "...-3.14");
			CHECK(toString(-3.14, ".=8") == "-...3.14");
			
			CHECK(toString(0.0, ".^5.1f") == ".0.0.");
		}
		
		SECTION("Sign") {
			CHECK(toString(0., "-") == "0");
			CHECK(toString(1., "-") == "1");
			CHECK(toString(-1., "-") == "-1");
			CHECK(toString(1./0., "-") == "Inf");
			
			CHECK(toString(0., "+") == "+0");
			CHECK(toString(1., "+") == "+1");
			CHECK(toString(-1., "+") == "-1");
			CHECK(toString(1./0., "+") == "+Inf");
			
			CHECK(toString(0., " ") == " 0");
			CHECK(toString(1., " ") == " 1");
			CHECK(toString(-1., " ") == "-1");
			CHECK(toString(1./0., " ") == " Inf");
		}
		
		SECTION("Zero Padding") {
			CHECK(toString(-42.5, "07") == "-0042.5");
			CHECK(toString(1.23, "+07") == "+001.23");
		}
		
		SECTION("Alternate Style") {
			CHECK(toString(12.) == "12");
			CHECK(toString(12., "#") == "12.");
		}
		
		SECTION("Precision") {
			CHECK(toString(1.23456789) == "1.23456789");
			CHECK(toString(1.23456789, ".5") == "1.2346");
			CHECK(toString(1.23456789, ".5f") == "1.23457");
			CHECK(toString(1.23, ".5") == "1.2300");
			CHECK(toString(1.23, ".2-4") == "1.23");
			CHECK(toString(1.23, ".4-6") == "1.230");
			CHECK(toString(1.23, ".1-2") == "1.2");
			CHECK(toString(12345., ".3") == "12300");
 			CHECK(toString(0.999, ".2f") == "1.00");
		}
		
		SECTION("Precision readjustment after rounding") {
			CHECK(toString(0.999, ".0-2f") == "1");
			CHECK(toString(0.001, ".0-2f") == "0");
			CHECK(toString(9.9, ".0f") == "10");
			CHECK(toString(9.99, ".1-2e") == "1e1");
		}
		
		SECTION("Formatting modes") {
			CHECK(toString(12.3, "e") == "1.23e1");
			CHECK(toString(12.3, "E") == "1.23E1");
			CHECK(toString(1.23e4, "ee") == "12.3e3");
			CHECK(toString(1.23e-4, "EE") == "123E-6");
			CHECK(toString(0.375, "si") == "375m");
			CHECK(toString(1024., "si") == "1.024k");
			CHECK(toString(1.234e10, "f") == "12340000000");
		}
		
		SECTION("Automatic scientific mode") {
			CHECK(toString(9999999999.) == "9999999999");
			CHECK(toString(10000000000.) == "1e10");
			CHECK(toString(10000000000., "G") == "1E10");
			CHECK(toString(0.001) == "0.001");
			CHECK(toString(0.00099999) == "9.9999e-4");
		}
	}
	
	SECTION("Exceptions") {
		CHECK_THROWS_WITH(toString(0., "+0#"), Catch::Contains("Unknown type parameter \"#\""));
		CHECK_THROWS_WITH(toString(0., "x"), Catch::Contains("Unknown type parameter \"x\""));
		CHECK_THROWS_WITH(toString(0., "--<6"), Catch::Contains("Unknown type parameter \"-<6\""));
		CHECK_THROWS_WITH(toString(0., ".-"), Catch::Contains("Maximum precision expected after '-'"));
		CHECK_THROWS_WITH(toString(0., ".4-2"), Catch::Contains("Maximum precision less than minimum"));
	}
}
