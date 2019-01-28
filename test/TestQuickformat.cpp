// test/TestQuickformat.cpp
//
// Tests for the QuickFormat methods

#include "catch2/catch.hpp"
#include "formatstring/QuickFormat.h"
#include "formatstring/Wrapper.h"

#include <sstream>

TEST_CASE("QuickFormat standard uses", "[QuickFormat]")
{
	CHECK(fs::format("a: {}, b: {}", 1, 2).str() == "a: 1, b: 2");
	CHECK(fs::format("{{{}}}", "hi").str() == "{hi}");
	
	std::stringstream s;
	fs::writeln(s, "Hello {}", "World");
	CHECK(s.str() == "Hello World\n");
	// Can't test cout printing...
	
	const int values[] = {1, 2, 3, 4, 5};
	const std::string text = "Once upon a time";
	
	CHECK(fs::format("{}: {:s0-4}, {}: {:s12-}", values[0], text, values[1], text).str()
	        == "1: Once, 2: time");
}

namespace {

struct CpReporter {
	int copies;
	CpReporter(): copies(0) {}
	CpReporter(const CpReporter& c): copies(c.copies + 1) {}
	CpReporter(const CpReporter&& m): copies(m.copies) {}
};

std::string str(const CpReporter& r, std::string)
{
	return fs::toString(r.copies);
}

}

TEST_CASE("QuickFormat copy elision", "[QuickFormat][CopyElision]")
{
	CHECK(fs::format("direct: {}, copy: {}",
			CpReporter(), fs::copy(CpReporter())).str() == "direct: 0, copy: 1");
}
