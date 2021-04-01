#include <cstdlib>
#include <cstdint>
#include <memory>
#include <cassert>
#include <cmath>
#include <ctgmath>
#include <type_traits>
#include <iostream>

//Implementation variables.
#define K9BLOCKSIZE ((ssize_t)1<<19)
#define K9BLOCKMASK (K9BLOCKSIZE-1)
#define K9_FAST_FLOAT_MATH 1
#define K9_USE_DOUBLE 1
#define K9_USE_LONG_DOUBLE 1
#define K9_IMPLEMENTATION_BITS 64

typedef float f32;
typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef size_t umax;
typedef ssize_t imax;
typedef u8 BYTE;

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



template <umax lev, umax size>
static inline umax fix_index(umax index){
	static const umax mask = (((umax)1<<(size-1))/((umax)1<<(umax)(lev-1)))-1;
	index &= mask;
	return index;
}

//Bus!
template <umax size> 
class k9_bus{
	protected:
	public:
		k9_bus() = default;
		void fill(BYTE val){
			for(umax i = 0; i < (umax{1} << umax(size - 1)); i++) {write(val,i);}
		}
		virtual BYTE read(umax where);
		virtual void write(BYTE byte, umax where);
		void copy(k9_bus<size>& other){
			for(umax i = 0; i < (umax{1} << umax(size - 1)); i++)
				{write(other.read(i),i);}
		}
		template<umax lev> 
		k9_bus<lev> at(umax index);
		k9_bus<size-1> high() 	{return at<size-1>(0);}
		k9_bus<size-1> low() 	{return at<size-1>(1);}
		void operator=(k9_bus<size>& other){ copy(other); }
		template <typename T>
		T to_(){
			BYTE data[sizeof(T)];
			for(umax i = 0; i < umax{sizeof(T)}; i++){
				data[i] = read(i);
			}
			T var;
			memcpy(&var, data, sizeof(T));
			return var;
		}
		template <typename T>
		void from_(T a){
			for(umax i = 0; i < sizeof(T); i++){
				write( *((BYTE*)&a), i );
			}
		}
		template <typename T>
		void fromp_(T* a){
			for(umax i = 0; i < sizeof(T); i++){
				write( *(((BYTE*)a)+i), i );
			}
		}
		virtual ~k9_bus(){}
};

template <umax size> 
class k9_ptrmem : public k9_bus<size>{
	public:
		k9_ptrmem<size>(BYTE* p) : data(p) {}
		inline BYTE read(umax where) {		  return  data[where & ((umax)1<<(size-1))];	}
		inline void write(BYTE byte, umax where) {	data[where & ((umax)1<<(size-1))] = byte;}
		template<umax lev> k9_ptrmem<lev> at(umax index){
			k9_ptrmem<lev> k(data + fix_index<lev, size>(index) );
			return k;
		}
	private:
		BYTE* data;
};

//The full address map.
//TODO
template <umax size>
class k9_addrmap : public k9_bus<size> {
	public:
	private:
};

//MY WORDS ARE BACKED WITH... real memory...
template <umax size> 
class k9_realmem : public k9_bus<size>{
	public:
		inline BYTE read(umax where) {		return  data[fix_index<1,size>(where)];		}
		inline void write(BYTE byte, umax where) {	data[fix_index<1,size>(where)] = byte;	}
		template<umax lev> k9_ptrmem<lev> at(umax index){
			k9_ptrmem<lev> k(data + fix_index<lev, size>(index) );
			return k;
		}
		operator k9_ptrmem<size>() {return k9_ptrmem<size>(data);}
	private:
		BYTE data[(umax)1<<(size-1)];
};
f32 myfunc(f32 a, f32 b) {a++; return a-b; }
u32 myfunc2(u32 a, i32 b){ b = 0; i32 q; a++;    q = 7;     a+=q;     return a%b; }
static k9_realmem<10> mymem;

void myfunc(){
	k9_realmem<11> some_memory;
	some_memory.fill(0);
	some_memory.at<3>(39).from_<u32>(0x7f3f3f7f);
	std::cout << "Value at the 39th state3, byte 1 is..." << 
		(u32)some_memory.at<3>(39).read(1) << 
	std::endl;
}
