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
#define K9BLOCKBYTES ((ssize_t)1<<19)
#define K9BLOCKLEVEL	20
#define K9BLOCKMASK (K9BLOCKBYTES1)

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
//Kernel9 internal common bus functions.
//Required for a K9 bus.

#define K9_BUS_METHODS()\
		template <typename T>\
		inline void top_(T *a){\
			constexpr umax s = __k9_max_p2n_inside(sizeof(T));\
			static_assert( s == sizeof(T) , "KERNEL9 ERROR: BAD READ SIZE" );\
			__read(    (BYTE*)a, __k9_max_p2n_inside(sizeof(T)) );\
			return;\
		}\
		template <typename T>\
		inline void fromp_(T* a){\
			constexpr umax s = __k9_max_p2n_inside(sizeof(T));\
			static_assert( s == sizeof(T), "KERNEL9 ERROR: BAD WRITE SIZE"  );\
			__write(  (BYTE*)a, __k9_max_p2n_inside(sizeof(T)) );\
			return;\
		}\
		template <typename T>\
		inline void from_(T a){fromp_<T>(&a);}\
		template <typename T>\
		inline T to_(){T a; top_(&a); return a;}\

#define 

//Contiguous memory declarations.
#define K9_INTERN_CONTIG_RW_DECL()\
inline BYTE* __getdata() {return data;}\
	inline void	__read(BYTE* dest, umax p2b){\
		memcpy(dest, data, p2b & getbytemask(size));\
	}\
	inline void __write(BYTE* src, umax p2b){\
		memcpy(data, src, p2b & getbytemask(size));\
	}\
	template <typename T> /*Copy values.*/\
	inline void copy(T& other){other.__read(data, size);}\
	template<umax lev> \
	inline __k9_intern_ptrmem<lev> at(umax index){\
		static_assert(lev <= size);\
		static_assert(lev > 0);\
		__k9_intern_ptrmem<lev> k(data + (((index) & getmask(lev, size))<<(lev-1)) );\
		return k;\
	}

template <umax size> 
class __k9_intern_ptrmem{
	public:
		__k9_intern_ptrmem<size>(BYTE* p) : data(p) {}
		K9_BUS_METHODS()
		K9_INTERN_CONTIG_RW_DECL()
		~__k9_intern_ptrmem(){}
	protected:
	private:
		BYTE* data;
};
//MY WORDS ARE BACKED WITH... real memory...
template <umax size> class k9_realmem{
	public:
		K9_BUS_METHODS()
		K9_INTERN_CONTIG_RW_DECL()
		operator __k9_intern_ptrmem<size>() {return __k9_intern_ptrmem<size>(data);}
		~k9_realmem<size>(){}
		k9_realmem<size>() =default;
	private:
		BYTE data[(umax)1<<(size-1)];
};

//constant bus.
template <umax size, typename Q>
class k9_constbus{
	public:
		k9_constbus<size,Q>(Q val) : __constant(val) {}
		static_assert( __k9_max_p2n_inside(sizeof(Q)) == sizeof(Q)  );
		K9_BUS_METHODS()
		inline void __read(BYTE* dest, umax p2b){
			for(umax i = 0; i < p2b/sizeof(Q); i ++)
				memcpy(
						dest + i * sizeof(Q), 
						&__constant, 
						p2b & getbytemask(size)
				);
		}
		inline void __write(BYTE* dest, umax p2b){(void)dest;(void)p2b;} //Do nothing.
	private:
		const Q __constant;
};


//f32 myfunc(f32 a, f32 b) {a++; return a-b; }
//u32 myfunc2(u32 a, i32 b){ b = 0; i32 q; a++;    q = 7;     a+=q;     return a%b; }
static k9_realmem<10> mymem;
static k9_constbus<10, u32> favorite_constbus(30);
typedef struct oddstruct {char data[3];} oddstruct;

void myfunc(){
	mymem.at<3>(10).from_<u32>(0x7f3f3f7f);
	k9_realmem<20> othermem;
	othermem.copy(mymem);
	printf("getmask of 3,1 is %zu" ,getmask(3,4));
	std::cout << "\n it's now..." << othermem.at<3>(10).to_<u32>()  << std::endl;
}

#endif
