
#include "K9.hpp"
#include <iostream>
int main(){
	State<20> bus;
	bus.g<u32>(1) = 3;
	myfunc(bus.gs<5>(0));
}
