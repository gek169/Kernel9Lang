//This is the file you include in your projects to use the Kernel9 Language.

#include "K9_Basic.hpp"


//#pragma GCC poison printf


//Basic syntax for a kernel.
//IS: Internal State
#define DEFKERNEL(name, n) void name (State<n>& IS)
#define g_(tn, addr) IS.g<tn>(addr)
#define gs_(n, addr) IS.gs<n>(addr)
//shorthand in case you wish to directly access IS.
#define g(tn, addr) g<tn>(addr)
#define gs(n, addr) gs<n>(addr)

static_assert(sizeof(State<4>) == 8);

DEFKERNEL(k_fmul_s3, 4){
	g_(f32,0) = g_(f32, 1) * g_(f32, 1);
	printf("Values inside mul: %f, %f\n" ,g_(f32, 0) ,g_(f32, 1));
}

DEFKERNEL(myfunc, 5){
	g_(f32, 0) = 3.0f;
	g_(f32, 1) = 7.0f;
	k_fmul_s3(
		IS.gs<4>(0)
	);
	printf("Values at end: %f, %f\n" ,g_(f32, 0) ,g_(f32, 1));
}
