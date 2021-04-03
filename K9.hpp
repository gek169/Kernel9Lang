//This is the file you include in your projects to use the Kernel9 Language.

#include "K9_Basic.hpp"



//Basic syntax for a kernel.
//IS: Internal State
#define DEFKERNEL(name, n) void name (State<n>& IS)
#define g_(tn, addr) IS.g<tn>(addr)
#define gs_(n, addr) IS.gs<n>(addr)
//shorthand in case you wish to directly access IS, or declare new states.
#define g(tn, addr) g<tn>(addr)
#define gs(n, addr) gs<n>(addr)
//Variable declaration
#define var(tn, name, addr)\
tn& name = g_(tn, addr);\



static_assert(sizeof(State<4>) == 8);

DEFKERNEL(k_fmul_s3, 4){
	var(f32, a, 0)
	var(f32, b, 1)
	a = a * b;
	printf("Values inside mul: %f, %f\n" ,a ,b);
}

DEFKERNEL(myfunc, 5){
	var(f32, a, 0)
	var(f32, b, 1)
	var(state4, c, 0)
	//a = 3.0f;
	b = 7.0f;
	printf("Values are: %f, %f\n" ,a ,b);
	k_fmul_s3(c);
	printf("Values at end: %f, %f\n" ,a ,b);
}
