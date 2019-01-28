// test/TestFormatstring.cpp
//
// Tests the Formatstring class.

#include "catch2/catch.hpp"
#include "formatstring/Formatstring.h"

#include "formatstring/Wrapper.h"


using namespace fs;

TEST_CASE("Formatstring basics w/o variables", "[Formatstring]")
{
	
	SECTION("Basic Getter") {
		Formatstring f1;
		Formatstring f2("Hello World");
		
		CHECK(f1.getFormat().empty());
		CHECK(f2.getFormat() == "Hello World");
	}
	
	SECTION("Copy and Move") {
		Formatstring f1("Hello");
		Formatstring f2(f1);
		
		CHECK(f1.getFormat() == "Hello");
		CHECK(f2.getFormat() == "Hello");
		
		Formatstring f3(std::move(f2));
		
		CHECK(f2.getFormat().empty());
		CHECK(f3.getFormat() == "Hello");
		
		swap(f2, f3);
		
		CHECK(f2.getFormat() == "Hello");
		CHECK(f3.getFormat().empty());
	}
	
	SECTION("Setter") {
		Formatstring f1("Hello");
		CHECK(f1.getFormat() == "Hello");
		f1.setFormat("World");
		CHECK(f1.getFormat() == "World");
	}
	
	SECTION("Basic Outputting") {
		Formatstring f1("Hello World");
		CHECK(f1.str() == "Hello World");
		CHECK(static_cast<std::string>(f1) == "Hello World");
		std::string s1 = f1;
		CHECK(s1 == "Hello World");
		std::stringstream str;
		f1.write(str);
		CHECK(str.str() == "Hello World");
		str.str("");
		f1.writeln(str);
		CHECK(str.str() == "Hello World\n");
	}
	
	SECTION("Escaping Braces") {
		Formatstring f1("Escape {{ me }}");
		CHECK(f1.str() == "Escape { me }");
		f1.setFormat("{{{{ :-}} }}}} {{}}");
		CHECK(f1.str() == "{{ :-} }} {}");
		f1.setFormat("}}{{");
		CHECK(f1.str() == "}{");
	}
}

namespace {

struct Reporter {};

std::string str(const Reporter&, std::string format)
{
	return format;
}

} // anon namespace

TEST_CASE("Formatstring basics w/ variables", "[Formatstring]")
{
	SECTION("Counting required and provided vars") {
		Formatstring f1("One var {}");
		CHECK(f1.countRequestedVariables() == 1);
		CHECK(f1.countProvidedVariables() == 0);
		f1.arg(1);
		CHECK(f1.countProvidedVariables() == 1);
		
		Formatstring f2("{{Two}} {} vars {}");
		CHECK(f2.countRequestedVariables() == 2);
		f2.args(1, 2, 3);
		CHECK(f2.countProvidedVariables() == 3);
	}
	
	SECTION("Outputting variables in order") {
		Formatstring f1("{}, {}, {}");
		f1.args(1, 2, 3);
		CHECK(f1.str() == "1, 2, 3");
		Formatstring f2("{}, {}");
		f2.args('a', 'b');
		CHECK(f2.str() == "a, b");
	}
	
	SECTION("Reading variable positions") {
		Formatstring f1("{3}, {2}, {1}");
		f1.args(1, 2, 3);
		CHECK(f1.str() == "3, 2, 1");
		Formatstring f2("{1}, {}, {3}, {}");
		f2.args(1, 2, 3);
		CHECK(f2.str() == "1, 1, 3, 2");
	}
	
	SECTION("Passing on formatting arguments") {
		Formatstring f1("{1:argument}");
		f1.arg(Reporter());
		CHECK(f1.str() == "argument");
		Formatstring f2("{:{{braces}} }");
		f2.arg(Reporter());
		CHECK(f2.str() == "{braces} ");
		Formatstring f3("{}}}");
		f3.arg(1);
		CHECK(f3.str() == "1}");
	}
	
	SECTION("Automatically clearing variables") {
		Formatstring f1("{}");
		f1.arg(1);
		CHECK(f1.str() == "1");
		CHECK(f1.wasPrinted());
		f1.arg(2);
		CHECK(!f1.wasPrinted());
		CHECK(f1.countProvidedVariables() == 1);
		CHECK(f1.str() == "2");
	}
	
	SECTION("Wrapper") {
		Formatstring f1("{}");
		int a = 1;
		f1.arg(fs::ref(&a));
		CHECK(f1.str() == "1");
		a = 2;
		CHECK(f1.str() == "2");
		
		Formatstring f2("{:fmt}");
		f2.arg(fs::fmt(fs::copy(Reporter()), "abc"));
		CHECK(f2.str() == "abc");
	}
}

TEST_CASE("Formatstring parsing exceptions", "[Formatstring}")
{
	Formatstring f1;
	CHECK_THROWS_WITH(f1.setFormat("{0}"), Catch::Contains("Variable IDs start at 1"));
	CHECK_THROWS_WITH(f1.setFormat("{foo"), Catch::Contains("Closing brace or colon expected"));
	CHECK_THROWS_WITH(f1.setFormat("{1:"), Catch::Contains("EOF within variable specifier"));
	CHECK_THROWS_WITH(f1.setFormat("Hi :}"), Catch::Contains("Unexpected closing brace"));
}
