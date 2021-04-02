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
#define K9BLOCKLEVEL	20
#define K9BLOCKBYTES	(umax)1<<(K9BLOCKLEVEL-1)


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
template <umax StageLevel>
class PlainBus{
	static_assert(StageLevel > 0);
	public:
		PlainBus<StageLevel>() = default;
		~PlainBus<StageLevel>() = default;
		template <typename T>
		inline PlainBus<StageLevel>& operator=(T other){
			{
				st<T>(other, 0);
				return *this;
			}
		}
		template <typename T> inline T ld(umax index){
			constexpr umax p2 = __k9_max_p2n_inside(sizeof(T));
			constexpr umax p1 = __k9_log2_floor(sizeof(T)) + 1;
			static_assert( p2  			== sizeof(T), "KERNEL9: MUST LD POWER OF TWO BYTES");
			static_assert( getwidth(p1) == sizeof(T), "KERNEL9: MUST LD POWER OF TWO BYTES");
			index &= getmask(p1, StageLevel);
			if(sizeof(T) == 1){
				//Handle the stupid case
				T var; 
				memcpy(&var, mem + index, sizeof(T));
				return var;
			} else if(sizeof(T) < getwidth(StageLevel)){
				T var;
				memcpy(&var,
						mem  + byteoff(index, p1, StageLevel),
						sizeof(T)
					);
				return var;
			} else if(sizeof(T) == getwidth(StageLevel)){
				T var;
				memcpy(&var, mem,sizeof(T));
				return var;
			} else { //Repeat ourselves multiple times into the destination.
				T var;
				for(umax i = 0; i < getrelwidth(StageLevel,p1); i++){
					((PlainBus<StageLevel>*)&var)[i] = ld<PlainBus<StageLevel>>(0);
				}
				return var;
			}
		}
		template <typename T> inline void st(T e, umax index){
			constexpr umax p2 = __k9_max_p2n_inside(sizeof(T));
			constexpr umax p1 = __k9_log2_floor(sizeof(T)) + 1;
			static_assert( p2  			== sizeof(T),"KERNEL9: MUST ST POWER OF TWO BYTES");
			static_assert( getwidth(p1)	== sizeof(T),"KERNEL9: MUST ST POWER OF TWO BYTES");
			index &= getmask( p1, StageLevel );
			if(sizeof(T) == 1){
				memcpy(mem + (index & getbytemask(StageLevel)), &e, sizeof(T));
				return;
			} else if(sizeof(T) < getwidth(StageLevel)){
				memcpy(mem + byteoff(index, p1, StageLevel), &e, 	sizeof(T));
				return;
			}else if (sizeof(T) == getwidth(StageLevel)){
				memcpy(mem, &e, sizeof(T));
				return;
			} else { //store the *last* portion.
				memcpy(mem,
						((BYTE*)&e) + (getrelwidth(StageLevel,p1) - 1),
						getwidth(StageLevel)
					);
			}
		}
		BYTE* __internal_get_mem(){return mem;}
	private:
		alignas(
			(getwidth(StageLevel)>K9_MAX_ALIGNMENT)?
			K9_MAX_ALIGNMENT:
			getwidth(StageLevel)
		) BYTE mem[ getwidth(StageLevel) ];
};
static_assert(sizeof(PlainBus<5>) == (1<<(5-1)));
static_assert(sizeof(PlainBus<1>) == (1<<(1-1)));
static_assert(sizeof(PlainBus<7>) == (1<<(7-1)));
static_assert(sizeof(PlainBus<21>) == (1<<(21-1)));
PlainBus<25> mystate;
void myfunc(){
	mystate = (u32)0x1b00004d;
	printf("value is %x", mystate.ld<PlainBus<4>>(0).ld<u32>(0));
}

#endif
