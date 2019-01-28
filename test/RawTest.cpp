#include "formatstring/QuickFormat.h"
#include <iostream>
#include <iomanip>

int main() {
	double pi = 3.14159;
	int i = 42;
	std::string message = "Hello World";
	
	std::cout << "pi = " << std::setprecision(5) << pi
	          << ", i = 0x" << std::hex << i << std::dec
	          << ", message = " << message << '\n';
	
	// vs
	
	fs::println("pi = {:.5}, i = {:#x}, message = {}", pi, i, message);
	
	fs::println("{:.5} = {1:.2-5} = {1:.-2}", 1.234);
	
	return 0;
}
