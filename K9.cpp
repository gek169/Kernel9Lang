
#include "K9.hpp"
#include <iostream>
int main(){
	State<20> bus;
	bus.a<6>(1) = 3;
	myfunc(bus.a<5>(0));
}
