#ifndef K9_BASIC_INCL
#define K9_BASIC_INCL

#include <cstdlib>
#include <cstdint>
#include <memory>
#include <cassert>
#include <cmath>
#include <ctgmath>
#include <type_traits>
#include <iostream>
#include <cstring>




#define K9_FAST_FLOAT_MATH 1
#define K9_USE_FLOAT 1
#define K9_USE_DOUBLE 1
#define K9_USE_LONG_DOUBLE 1
#define K9_IMPLEMENTATION_BITS 64
#define K9_MAX_ALIGNMENT 32
//Implementation default 
#define K9BLOCKLEVEL	18
#define K9BLOCKBYTES	getwidth(K9BLOCKLEVEL)


#if K9_USE_FLOAT
typedef float f32;
static_assert(sizeof (f32) == 4);
#endif
typedef uint8_t u8;
typedef int8_t i8;
typedef u8 BYTE;
typedef uint16_t u16;
typedef int16_t i16;


#if K9_IMPLEMENTATION_BITS >= 32
typedef uint32_t u32;
typedef int32_t i32;
#endif
typedef size_t umax;
typedef ssize_t imax;


#if K9_IMPLEMENTATION_BITS >= 64

typedef uint64_t u64;
typedef int64_t i64;

#endif

#if K9_USE_DOUBLE
typedef double f64;
#endif

#if K9_USE_LONG_DOUBLE
typedef long double f128;
#endif



constexpr static inline umax getwidth(umax size)            {return (umax)1<<((umax)(size-1));}
constexpr static inline umax getbytemask(umax size)         {return getwidth(size) - 1;}
constexpr static inline umax getrelwidth(umax os, umax size){return getwidth(size) / getwidth(os);}
constexpr static inline umax getmask(umax os, umax size)    {return getrelwidth(os,size)?
																(getrelwidth(os, size)-1):
																0;}
constexpr static inline umax fixindex(umax ind, umax os, umax size) {return ind & getmask(os,size);}
constexpr static inline umax byteoff(umax ind, umax os, umax size) {return fixindex(ind, os, size) * getwidth(os);}
//Compiletime unit tests.
static_assert(fixindex(3,5,6) == 1);
static_assert(fixindex(4,3,5) == 0);
static_assert(byteoff(1,1,2) == 1);
static_assert(byteoff(1,2,3) == 2);

static_assert(fixindex(3,2,3) == 1);
static_assert(byteoff(3,2,3) == 2);

static_assert(fixindex(1,3,3) == 0);
static_assert(byteoff(1,3,3) == 0);

static_assert(fixindex(1,2,3) == 1);
static_assert(byteoff(1,2,3) == 2);

static_assert(byteoff(1,4,3) == 0);
static_assert(getwidth(1) == 1);
static_assert(getwidth(2) == 2);
static_assert(getwidth(3) == 4);
static_assert(getrelwidth(1,3)-1 == 3);
static_assert(getrelwidth(3,7)-1 == 15);

constexpr umax __k9_log2_ceil(umax n)
{
  return ( (n<2) ? 1 : 1+log2(n/2));
}

constexpr umax __k9_log2_floor(umax n)
{
  return ( (n<2) ? 0 : 1+log2(n/2));
}

constexpr umax __k9_max_p2n_inside(umax n){
	return (umax)1<<__k9_log2_floor(n);
}

//The 

//The Kernel9 state machine and Basic "Sized Bus"
//This is a 100% memory-backed, contiguous bus.
template <umax StageLevel> struct __k9_internal_mblock {BYTE mem[getwidth(StageLevel)];};

#define K9_NORMAL_BUS_ASSIGN_BEHAVIOR(stagelevel)\
		template <typename T>\
		inline void operator=(T& other){\
			if(stagelevel <= K9BLOCKLEVEL){\
		   	    st(other.template ld<__k9_internal_mblock<stagelevel>>(0),0);\
		   	} else {\
		   		for(umax i = 0; i < getwidth(stagelevel) / K9BLOCKBYTES; i++)\
		   			st(other.template ld<__k9_internal_mblock<K9BLOCKLEVEL>>(i),i);\
		   	}\
		}
#define K9_BUS_POW2_CHECK()\
			constexpr umax p2 = __k9_max_p2n_inside(sizeof(T));\
			constexpr umax p1 = __k9_log2_floor(sizeof(T)) + 1;\
			static_assert( p2  			== sizeof(T),"KERNEL9: MUST ST POWER OF TWO BYTES");\
			static_assert( getwidth(p1)	== sizeof(T),"KERNEL9: MUST ST POWER OF TWO BYTES");\

template <umax StageLevel>
class PlainBus{
	static_assert(StageLevel > 0);
	public:
		PlainBus<StageLevel>() = default;
		~PlainBus<StageLevel>() = default;
		template <typename T> inline T ld(umax index) const {
			K9_BUS_POW2_CHECK()
			index &= getmask(p1, StageLevel);
            if(sizeof(T) <= getwidth(StageLevel)){
				T var;
				memcpy(&var,
						mem  + byteoff(index, p1, StageLevel),
						sizeof(T)
					);
				return var;
			} else { //Repeat ourselves multiple times into the destination.
				T var;
				for(umax i = 0; i < getrelwidth(StageLevel,p1); i++){
					((__k9_internal_mblock<StageLevel>*)&var)[i] = 
					ld<__k9_internal_mblock<StageLevel>>(0);
				}
				return var;
			}
		}
		template <typename T> inline void st(T e, umax index){
			K9_BUS_POW2_CHECK()
			index &= getmask( p1, StageLevel );
            if(sizeof(T) <= getwidth(StageLevel)){
				memcpy(mem + byteoff(index, p1, StageLevel), &e, 	sizeof(T));
			} else {
			//store the *last* portion of e- behave as if everything were written byte-by-byte.
				memcpy(mem,
						((BYTE*)&e) + (getrelwidth(StageLevel,p1) - 1),
						getwidth(StageLevel)
					);
				
			}
			return;
		}
		K9_NORMAL_BUS_ASSIGN_BEHAVIOR(StageLevel)
		BYTE* __internal_get_mem(){return mem;}
	private:
		alignas(
			(getwidth(StageLevel)>K9_MAX_ALIGNMENT)?
			K9_MAX_ALIGNMENT:
			getwidth(StageLevel)
		) BYTE mem[ getwidth(StageLevel) ];
};


//Combination bus.
//Allows you to pass two buses.
//The resulting address space is getwidth(StageLevel)
//T1 and T2 should logically be buses of 
template <umax StageLevel, typename T1, typename T2>
class DBus{
	public:
		DBus<StageLevel, T1, T2>(T1& _u, T2& _l): upper(_u), lower(_l){};
		template <typename T> inline T ld(umax index){
			K9_BUS_POW2_CHECK()
			if(getwidth(StageLevel) & sizeof(T)){
				return upper.template ld<T>(index);
			}
			return lower.template ld<T>(index);
		}
		template <typename T> inline void st(T e, umax index){
			K9_BUS_POW2_CHECK()
			if(getwidth(StageLevel) & sizeof(T)){
				upper.template st<T>(e, index);
				return;
			}
			lower.template st<T>(e, index);
			return;
		}
		K9_NORMAL_BUS_ASSIGN_BEHAVIOR(StageLevel);
	private:
		T1& upper;
		T2& lower;
};

template <umax StageLevel, typename T1, typename T2>
DBus<StageLevel, T1, T2> AddBus(T1& a, T2& b){
	return DBus<StageLevel, T1, T2>(a, b);
}
static_assert(sizeof(PlainBus<5>) == (1<<(5-1)));
static_assert(sizeof(PlainBus<1>) == (1<<(1-1)));
static_assert(sizeof(PlainBus<7>) == (1<<(7-1)));
static_assert(sizeof(PlainBus<21>) == (1<<(21-1)));
PlainBus<23> mystate;

void myfunc(){
	PlainBus<5> state1;
	PlainBus<5> state2;
	//Template type deduction at its finest!
	AddBus<6>(state2, mystate).st((u32)0x1b, 0);

	
	mystate.st((u32)0xfff, 		1);
	mystate.st((u32)0xe, 		2);
	mystate.st((u32)0xaa, 		3);
	puts("reached this spot.");
	state1 = mystate;
	puts("reached this other spot.");
	mystate = state1;
	for(umax i = 0; i < (umax)1<<(8-1); i++)
		printf("value is %x\n", mystate.ld<u32>(i));
}

#endif
