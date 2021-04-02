#include "K9.hpp"
#include <iostream>

//Calling Kernel9 code from C++.
int main(){
	state20 bus;
	bus.gs(5,0).g(f32, 0) = 47.0f;
	myfunc(bus.gs(5,0));
}
