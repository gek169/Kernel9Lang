#include "K9.hpp"
#include <iostream>

//Calling Kernel9 code from C++.
int main(){
	State<23> mystate;
	mystate.gs<5>(381908429107).g(f32, 0) = 10.0;
	myfunc(mystate.gs<5>(381908429107));
}
