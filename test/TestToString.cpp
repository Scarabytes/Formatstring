// test/TestToString.cpp
//
// Tests the toString method

#include "catch2/catch.hpp"
#include "formatstring/ToString.h"

namespace {
struct A {};
std::ostream& operator <<(std::ostream& s, A) { return s << "A"; }

struct B {
	operator std::string() const { return "B"; }
};
std::ostream& operator <<(std::ostream& s, B) { return s << "A"; }

struct C {
	std::string str() const { return "C"; }
	operator std::string() const { return "B"; }
};
std::ostream& operator <<(std::ostream& s, C) { return s << "A"; }

struct D {
	std::string str(std::string) const { return "D"; }
	std::string str() const { return "C"; }
	operator std::string() const { return "B"; }
};
std::ostream& operator <<(std::ostream& s, D) { return s << "A"; }

struct E {
	std::string str(std::string) const { return "D"; }
	std::string str() const { return "C"; }
	operator std::string() const { return "B"; }
};
std::ostream& operator <<(std::ostream& s, E) { return s << "A"; }
std::string str(E) { return "E"; }

struct F {
	std::string str(std::string) const { return "D"; }
	std::string str() const { return "C"; }
	operator std::string() const { return "B"; }
};
std::ostream& operator <<(std::ostream& s, F) { return s << "A"; }
std::string str(F) { return "E"; }
std::string str(F, std::string) { return "F"; }

} // anon namespace

TEST_CASE("toString() overload selection", "[toString]")
{
	CHECK(fs::toString(A()) == "A");
	CHECK(fs::toString(B()) == "B");
	CHECK(fs::toString(C()) == "C");
	CHECK(fs::toString(D()) == "D");
	CHECK(fs::toString(E()) == "E");
	CHECK(fs::toString(F()) == "F");
	
	std::string a("a");
	CHECK(fs::toString(a) == "a");
	CHECK(fs::toString(std::string("b")) == "b");
}
