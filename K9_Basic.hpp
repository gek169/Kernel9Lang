#ifndef K9_BASIC_INCL
#define K9_BASIC_INCL

#include <cstdlib>
#include <cstdint>
#include <memory>
#include <cassert>
#include <cmath>
#include <ctgmath>
#include <iostream>
#include <cstring>

#define loop(v, e)\
static_assert(e >= 0, "end must be constant expression >= 0");\
for(unsigned long long v = 0, __k9_internal_##v = 0; __k9_internal_##v < e; __k9_internal_##v++, v = __k9_internal_##v)

#define loople(v, e)\
static_assert(e >= 0, "end must be constant expression >= 0");\
for(unsigned long long v = 0, __k9_internal_##v = 0; __k9_internal_##v <= e; __k9_internal_##v++, v = __k9_internal_##v)


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

constexpr umax __k9_ceil_p2n(umax n){
	return (umax)1<<__k9_log2_ceil(n);
}

constexpr umax __k9_floor_p2n(umax n){
	return (umax)1<<__k9_log2_floor(n);
}

template <umax StageLevel> struct __k9_internal_mblock {BYTE mem[getwidth(StageLevel)];};


template <umax StageLevel>
class State{
	static_assert(StageLevel > 0);
	public:
		constexpr State<StageLevel>() = default;
		~State<StageLevel>() = default;
		template <typename T>
		constexpr inline void operator=(const T& other){
			static_assert(sizeof(T) <= getwidth(StageLevel), "Location not large enough to store...");
			memcpy(mem, &other, sizeof(T));
		}
		template <umax lvl>
		constexpr inline State<lvl>& gs(const umax ind){
			static_assert(lvl <= StageLevel, "Location not large enough to read...");
			//static_assert(ind < getrelwidth(lvl, StageLevel), "ERROR: Index out of bounds");
			return *((State<lvl>*)(mem + byteoff(ind, lvl, StageLevel)));
		}
		template <typename T>
		constexpr inline T& g(const umax ind){
			constexpr umax p2n = __k9_log2_ceil(sizeof(T))+1;
			//static_assert(ind < getrelwidth(p2n, StageLevel), "ERROR: Index out of bounds");
			static_assert(p2n <= (StageLevel), "Location not large enough to store...");
			return *((T*)(mem + byteoff(ind, p2n, StageLevel)));
		}
	private:
		//This memory is always aligned.
		alignas(
			(getwidth(StageLevel)  >  K9_MAX_ALIGNMENT)* 
			K9_MAX_ALIGNMENT +
			(!(getwidth(StageLevel)  >  K9_MAX_ALIGNMENT)) * getwidth(StageLevel)
		) BYTE mem[ getwidth(StageLevel) ];
};
//Compiletime unit tests.
static_assert(sizeof(State<5>) == (umax)1<<(5-1));
static_assert(sizeof(State<12>) == (umax)1<<(12-1));

//typedefs
#define K9_INTERNAL_DEFSTATE(n) typedef State<n> state##n;

K9_INTERNAL_DEFSTATE(1);
K9_INTERNAL_DEFSTATE(2);
K9_INTERNAL_DEFSTATE(3);
K9_INTERNAL_DEFSTATE(4);
K9_INTERNAL_DEFSTATE(5);
K9_INTERNAL_DEFSTATE(6);
K9_INTERNAL_DEFSTATE(7);
K9_INTERNAL_DEFSTATE(8);
K9_INTERNAL_DEFSTATE(9);
K9_INTERNAL_DEFSTATE(10);
K9_INTERNAL_DEFSTATE(11); //1 KB
K9_INTERNAL_DEFSTATE(12);
K9_INTERNAL_DEFSTATE(13);
K9_INTERNAL_DEFSTATE(14);
K9_INTERNAL_DEFSTATE(15);
K9_INTERNAL_DEFSTATE(16);
#if K9_IMPLEMENTATION_BITS > 16
K9_INTERNAL_DEFSTATE(17); //64M
K9_INTERNAL_DEFSTATE(18); //128M
K9_INTERNAL_DEFSTATE(19); //256M
K9_INTERNAL_DEFSTATE(20); //512M
K9_INTERNAL_DEFSTATE(21); //1MB
K9_INTERNAL_DEFSTATE(22);
K9_INTERNAL_DEFSTATE(23);
K9_INTERNAL_DEFSTATE(24);
K9_INTERNAL_DEFSTATE(25);
K9_INTERNAL_DEFSTATE(26);
K9_INTERNAL_DEFSTATE(27);
K9_INTERNAL_DEFSTATE(28);
K9_INTERNAL_DEFSTATE(29);
K9_INTERNAL_DEFSTATE(30);
K9_INTERNAL_DEFSTATE(31); //1GB
K9_INTERNAL_DEFSTATE(32);
#if K9_IMPLEMENTATION_BITS > 32
K9_INTERNAL_DEFSTATE(33);
K9_INTERNAL_DEFSTATE(34);
K9_INTERNAL_DEFSTATE(35);
K9_INTERNAL_DEFSTATE(36);
K9_INTERNAL_DEFSTATE(37);
K9_INTERNAL_DEFSTATE(38);
K9_INTERNAL_DEFSTATE(39);
K9_INTERNAL_DEFSTATE(40);
K9_INTERNAL_DEFSTATE(41);
K9_INTERNAL_DEFSTATE(42);
K9_INTERNAL_DEFSTATE(43);
K9_INTERNAL_DEFSTATE(44);
K9_INTERNAL_DEFSTATE(45);
K9_INTERNAL_DEFSTATE(46);
K9_INTERNAL_DEFSTATE(47);
K9_INTERNAL_DEFSTATE(48);
#endif
#endif

#endif
