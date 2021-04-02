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


//Implementation variables.
#define K9BLOCKLEVEL	20
#define K9BLOCKBYTES ((size_t)1<<(K9BLOCKLEVEL - 1))
#define K9BLOCKMASK (K9BLOCKBYTES - 1)

#define K9_FAST_FLOAT_MATH 1
#define K9_USE_FLOAT 1
#define K9_USE_DOUBLE 1
#define K9_USE_LONG_DOUBLE 1
#define K9_IMPLEMENTATION_BITS 64

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

//The Kernel9 basic bus interface
//Contains an amount of internal memory which is used in different ways depending on mode.
//Four bits are used, as "flags"
//PLAIN MODE
//if the plain flag is set, is_function is ignored.
//We either store memory directly,  or a pointer to memory owned by other buses, depending on is_owning (bit 1)
//if the read-only flag is set, writes are ignored.
//if the "is_wrapped" flag is set, attempts to read beyond the bounds of
//the allocated memory will loop around to the beginning.
//FUNCTION MODE
//Only available if is_plain is 0.
//
constexpr bool is_wrapped(umax mode){return (mode & 16) == 1;}
constexpr bool is_read_only(umax mode){return (mode & 8) == 1;}
constexpr bool is_function(umax mode){return (mode & 4) == 1;}
constexpr bool is_plain(umax mode){return (mode & 2) == 1; }
constexpr bool is_owning(umax mode){return (mode & 1) == 1;}

template <umax StageLevel, umax mode>
class Kernel9_Bus{
	static_assert(StageLevel > 0);
	//static_assert(mode == 0 || mode == 1 || mode == 2 || mode == 3);
	public:
		/*
			static constexpr umax is_read_only = mode & 8; //Ignore writes? This is propagated.
			static constexpr umax is_function = mode & 4; //When writes occur, do we execute a function?
			static constexpr umax is_plain = mode & 2; //Boring?
			static constexpr umax is_owning = mode & 1; //Do we own our resorces?
		*/
		Kernel9_Bus<StageLevel, mode>(void* dads_data){
			//TODO: Implement
		}
		template <typename T> void copy_bus(T other){
			//TODO
		}
		void __internal_read(BYTE* 	dest, umax sz, umax where){} //TODO
		void __internal_write(BYTE* src, umax sz, umax where){ if(is_read_only) return;
		
		} //TODO
		//TODO: Implement correct .at<> functionality. That'll be fun...
		//template < (some calculation for stagelevel), (some calculation for mode)> 
		//Kernel9_Bus<,> at(){}
		//
	private:

		BYTE mem[
			(is_plain(mode))?
			(//PLAIN BUSES- Hold memory only.
				is_owning(mode)? //Owning Memory
				(
				   (StageLevel <= K9BLOCKLEVEL)?
					((umax)1<<(StageLevel-1)): //Small buffer optimization
					(((umax)1<<(StageLevel-1))/K9BLOCKBYTES) * sizeof(void*) //Block allocation
				)
				:
				(					//Pointer to memory
					(StageLevel <= K9BLOCKLEVEL)?
					sizeof(void*): //Points to less than or equal to a single block
					((((umax)1<<(StageLevel-1))/K9BLOCKBYTES) * sizeof(void*)) //Points to the same number of blocks as if we owned it.
				)	
			):(

				
			)
		];


};
static_assert(sizeof(Kernel9_Bus<5, 5>) == (1<<(5-1)));

#endif
