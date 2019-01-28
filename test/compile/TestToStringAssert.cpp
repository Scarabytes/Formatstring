// test/compile/TestToStringAssert.cpp
//
// Tests the static assertion of toString(), if the passed object does not
// provide any of the required methods to stringify.

#include "formatstring/ToString.h"

#include <iostream>

struct NoToString{};

int main() {
	
	std::cout << fs::toString(NoToString());
	
	return 0;
}
