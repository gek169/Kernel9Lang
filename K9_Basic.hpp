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
constexpr static inline umax getmask(umax os, umax size)    {return getrelwidth(os, size)-1;}

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

//The Kernel9 State machine
template <umax StageLevel>
class State{
	static_assert(StageLevel > 0);

	public:
		
		State<StageLevel>(){
			if(StageLevel > K9BLOCKLEVEL)
				for(size_t i = 0; i < (((umax)1<<(StageLevel-1))/ getwidth(K9BLOCKLEVEL)); i++){
					((BYTE**)mem)[i] = new BYTE[K9BLOCKLEVEL];
				}
		}
		~State<StageLevel>(){
			if(StageLevel > K9BLOCKLEVEL)
				for(size_t i = 0; i < (((umax)1<<(StageLevel-1))/ getwidth(K9BLOCKLEVEL)); i++){
					delete [] ((BYTE**)mem)[i];
				}
		}
		void operator=(State<StageLevel>& other){
			if(StageLevel <= K9BLOCKLEVEL){
				memcpy(mem, other.__internal_get_mem(), getwidth(StageLevel));
				return;
			}
			//Loop over it
			for(umax i = 0; i < getwidth(StageLevel)/ getwidth(K9BLOCKLEVEL); i++){
				at<K9BLOCKLEVEL>(i) = other.at<K9BLOCKLEVEL>(i);
			}
		}
		template <umax lvl> State<lvl>& at(umax index){
			static_assert(lvl < StageLevel);
			if(StageLevel <= K9BLOCKLEVEL){
				return *((State<lvl>*)(mem + getwidth(lvl) * (index & getwidth(StageLevel))));
			}
			//if StageLevel is greater.
		}
		BYTE* __internal_get_mem(){return mem;}
	private:
		BYTE mem[
			(
			   (StageLevel <= K9BLOCKLEVEL)?
				getwidth(StageLevel): //Small buffer optimization
				getwidth(StageLevel)/ getwidth(K9BLOCKLEVEL) * sizeof(BYTE*) //Block allocation
			)
		];


};
static_assert(sizeof(State<5>) == (1<<(5-1)));
static_assert(sizeof(State<1>) == (1<<(1-1)));
static_assert(sizeof(State<7>) == (1<<(7-1)));
static_assert(sizeof(State<21>) == 2*sizeof(void*));
State<30> mystate;

#endif
