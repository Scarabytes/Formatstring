// test/TestVariables.cpp
//
// Tests the variable holder classes.

#include "catch2/catch.hpp"
#include "formatstring/detail/Variable.h"

using namespace fs;

namespace {

struct CpReporter {
	int copies;
	CpReporter(): copies(0) {}
	CpReporter(const CpReporter& c): copies(c.copies + 1) {}
	CpReporter(const CpReporter&& m): copies(m.copies) {}
};

std::string str(const CpReporter& r, std::string)
{
	return toString(r.copies);
}

struct FmtReporter{};

std::string str(const FmtReporter&, std::string format)
{
	return format;
}

} // anon namespace

TEST_CASE("VariableCopy", "[Variable][VariableCopy]")
{
	CpReporter c;
	U<Variable> v1 = mkU<VariableCopy<CpReporter>>(c);
	CHECK(v1->toString({}) == "1");
	U<Variable> v2 = v1->clone();
	CHECK(v2->toString({}) == "2");
	CHECK(c.copies == 0);
}

TEST_CASE("VariableReference", "[Variable][VariableReference]")
{
	U<Variable> v1;
	{
		S<CpReporter> s1 = mkS<CpReporter>();
		v1 = mkU<VariableReference<CpReporter>>(s1);
		CHECK(v1->toString({}) == "0");
		U<Variable> v2 = v1->clone();
		CHECK(v1->toString({}) == "0");
	}
	CHECK(v1->toString({}) == "nullptr");
	S<CpReporter> s2;
	v1 = mkU<VariableReference<CpReporter>>(s2);
	CHECK(v1->toString({}) == "nullptr");
	S<int> s3 = mkS<int>(1);
	v1 = mkU<VariableReference<int>>(s3);
	CHECK(v1->toString({}) == "1");
	*s3 = 42;
	CHECK(v1->toString({}) == "42");
}

TEST_CASE("VariableRawReference", "[Variable][VariableRawReference]")
{
	int i = 1;
	U<Variable> v1 = mkU<VariableRawReference<int>>(&i);
	CHECK(v1->toString({}) == "1");
	i = 42;
	CHECK(v1->toString({}) == "42");
	v1 = mkU<VariableRawReference<int>>(nullptr);
	CHECK(v1->toString({}) == "nullptr");
}

TEST_CASE("VariableFormat", "[Variable][VariableFormat]")
{
	U<Variable> v1 = mkU<VariableCopy<FmtReporter>>(FmtReporter());
	CHECK(v1->toString("myfmt") == "myfmt");
	U<Variable> v2 = mkU<VariableFormat>(std::move(v1), "fmt123");
	CHECK(v2->toString("myfmt") == "fmt123");
}
