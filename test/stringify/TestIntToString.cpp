// test/stringify/TestIntToString.cpp
//
// Tests the conversion of integers to strings

#include "catch2/catch.hpp"
#include "formatstring/ToString.h"


using namespace fs;

TEST_CASE("IntToString", "[toString][IntToString]")
{
	SECTION("Integral Types") {
		CHECK(toString((signed char) -12) == "-12");
		CHECK(toString((signed short) -12345) == "-12345");
		CHECK(toString((signed int) -42) == "-42");
		CHECK(toString((signed long) -9174) == "-9174");
		CHECK(toString((signed long long) -1999581) == "-1999581");
		
		CHECK(toString((unsigned char) 155) == "155");
		CHECK(toString((unsigned short) 20000) == "20000");
		CHECK(toString((unsigned int) 42) == "42");
		CHECK(toString((unsigned long) 12343) == "12343");
		CHECK(toString((unsigned long long) 918571) == "918571");
	}
	
	SECTION("Limit values") {
		CHECK(toString(0) == "0");
		
		CHECK(toString((int8_t)  127) == "127");
		CHECK(toString((int16_t) 32767) == "32767");
		CHECK(toString((int32_t) 2147483647) == "2147483647");
		CHECK(toString((int64_t) 9223372036854775807) == "9223372036854775807");
		
		CHECK(toString((int8_t)  -128) == "-128");
		CHECK(toString((int16_t) -32768) == "-32768");
		CHECK(toString((int32_t) -2147483648) == "-2147483648");
		CHECK(toString((int64_t) -9223372036854775808ull) == "-9223372036854775808");
		
		CHECK(toString((uint8_t)  255) == "255");
		CHECK(toString((uint16_t) 65535) == "65535");
		CHECK(toString((uint32_t) 4294967295) == "4294967295");
		CHECK(toString((uint64_t) 18446744073709551615ull) == "18446744073709551615");
	}
	
	SECTION("Formatting") {
		SECTION("Alignment") {
			CHECK(toString(42, "5") ==  "   42");
			CHECK(toString(42, ">5") == "   42");
			CHECK(toString(42, "<5") == "42   ");
			CHECK(toString(42, "^5") == " 42  ");
			CHECK(toString(42, "=5") == "   42");
			
			CHECK(toString(-42, ".>6") == "...-42");
			CHECK(toString(-42, ".=6") == "-...42");
			
			CHECK(toString(0, ".^5") == "..0..");
		}
		
		SECTION("Sign") {
			CHECK(toString(0, "-") == "0");
			CHECK(toString(1, "-") == "1");
			CHECK(toString(-1, "-") == "-1");
			
			CHECK(toString(0, "+") == "+0");
			CHECK(toString(1, "+") == "+1");
			CHECK(toString(-1, "+") == "-1");
			
			CHECK(toString(0, " ") == " 0");
			CHECK(toString(1, " ") == " 1");
			CHECK(toString(-1, " ") == "-1");
		}
		
		SECTION("Zero Padding") {
			CHECK(toString(-5, "05") == "-0005");
			CHECK(toString(10, "+05") == "+0010");
			CHECK(toString(0, " 03") == " 00");
		}
		
		SECTION("Bases") {
			CHECK(toString(42, "x") == "2a");
			CHECK(toString(42, "X") == "2A");
			CHECK(toString(42, "o") == "52");
			CHECK(toString(42, "b") == "101010");
			CHECK(toString(42, "d") == "42");
			
			CHECK(toString(-42, "x") == "-2a");
			CHECK(toString(-42, "o") == "-52");
			CHECK(toString(-42, "b") == "-101010");
			
			CHECK(toString(0, "x") == "0");
		}
		
		SECTION("Alternate Style") {
			CHECK(toString(12, "#d") == "12");
			CHECK(toString(12, "#b") == "0b1100");
			CHECK(toString(12, "#o") == "0o14");
			CHECK(toString(12, "#x") == "0xc");
			CHECK(toString(12, "#X") == "0xC");
			
			CHECK(toString(12, "+#b") == "+0b1100");
			CHECK(toString(12, "+#o") == "+0o14");
			CHECK(toString(12, "+#x") == "+0xc");
			CHECK(toString(12, "+#X") == "+0xC");
			
			CHECK(toString(0, " #x") == " 0x0");
		}
		
		SECTION("Exceptions") {
			CHECK_THROWS_WITH(toString(0, "+0#"), Catch::Contains("Unknown type parameter \"#\""));
			CHECK_THROWS_WITH(toString(0, "e"), Catch::Contains("Unknown type parameter \"e\""));
			CHECK_THROWS_WITH(toString(0, "--<6"), Catch::Contains("Unknown type parameter \"-<6\""));
		}
	}
}
