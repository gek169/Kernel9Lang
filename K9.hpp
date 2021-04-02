//This is the file you include in your projects to use the Kernel9 Language.

#include "K9_Basic.hpp"

#define load(t, n) template ld<t>(n)
#define store(t, e, n) template st<t>(e, n)

#define DEFKERNEL(name) template <typename T>  void name(T& G_BUS)

DEFKERNEL(myfunc){
	PlainBus<5> state1;
	PlainBus<5> state2;
	//Template type deduction at its finest!
	MergeBus<6>(state2, G_BUS).store(u32, 0x1b, 0);
	AddBus<6>(state2, G_BUS).store(u32, 0x00, 0); //This one won't actually happen.
	G_BUS.store(u32, 0xff, 1);
	G_BUS.store(u32, 0xaa, 2);
	G_BUS.store(u32, 0xcc, 3);
	//TODO: Poison puts.
	puts("reached this spot.");
	state1 = G_BUS;
	puts("reached this other spot.");
	G_BUS = state1;
	for(umax i = 0; i < (umax)1<<(8-1); i++)
		printf("value is %x\n", G_BUS.load(u32, i));
}
