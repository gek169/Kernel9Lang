//This is the file you include in your projects to use the Kernel9 Language.

#include "K9_Basic.hpp"

#define DEFKERNEL(name, n) void name (State<n>& G_BUS)
#define 


DEFKERNEL(myfunc, 5){
	G_BUS.a<3>(28582093582) = (f32)3;
	G_BUS.a<3>(28582093582) = (f32)G_BUS.a<3>(28582093582) * 3;
	printf("Value is %f" ,(f32)G_BUS.a<3>(28582093582));
}
