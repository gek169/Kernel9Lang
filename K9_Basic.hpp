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
//Implementation block size- How much memory are you willing to store on the stack?
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


template <umax StageLevel>
class State{
	static_assert(StageLevel > 0);
	public:
		State<StageLevel>() = default;
		~State<StageLevel>() = default;
		template <umax lvl>
		inline State<lvl>& a(umax ind){
			static_assert(lvl <= StageLevel);
			return *((State<lvl>*)(mem + byteoff(ind, lvl, StageLevel)));
		}
		template <typename T>
		inline void operator=(const T& other){
			static_assert(sizeof(T) <= getwidth(StageLevel), "Location not large enough to store...");
			memcpy(mem, &other, sizeof(T));
		}
		template <typename T>
		inline operator T(){
			T x;
			static_assert(sizeof(T) <= getwidth(StageLevel), "Location not large enough to store...");
			memcpy(&x, mem, sizeof(T));
			return x;
		}
	private:
		alignas(
			(getwidth(StageLevel)>K9_MAX_ALIGNMENT)?
			K9_MAX_ALIGNMENT:
			getwidth(StageLevel)
		) BYTE mem[ getwidth(StageLevel) ];
};
//Compiletime unit tests.
static_assert(sizeof(State<5>) == (umax)1<<(5-1));
static_assert(sizeof(State<12>) == (umax)1<<(12-1));


#endif
