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




//Signed integer implementation.
template <typename Ti, typename To>
struct int_intern{

	private:
		Ti f;
		static_assert(sizeof(Ti) == sizeof(To), "Bad integer types");	
		static inline int_intern<Ti, To> nonzero(int_intern<Ti, To> a){
			if(a.__getintern() == 0)
				a.__setintern(0);
			return a;
		}
	public:	

#define K9_INTERNAL_INT_CONSTRUCTOR(x)\
		inline int_intern<Ti, To>(x a) {f = a;}\
		inline int_intern<Ti, To> operator=(x a) {f = a; return *this;}
		//operator std::size_t() const {return f;}
		inline int_intern<Ti, To>() = default;
		inline int_intern<Ti, To> operator=(const int_intern<Ti, To> other) 		
			{f = other.__getintern(); return *this;}
		inline Ti __getintern() const
			{return 	f;}
		inline To __getinterno() const
			{return (To)(f);}
		void __setintern(Ti a)
			{f = a;}
		void __setinterno(To a)
			{f = (Ti)(a);}
	//Actual operator implementations.
#define K9_INTERNAL_INT_SIMP_OP_NOEQ(x) \
		int_intern<Ti, To> operator x(const int_intern<Ti, To> c) const {\
			return __getintern() x c.__getintern();\
		}
#define K9_INTERNAL_INT_SIMP_OP(x) \
		K9_INTERNAL_INT_SIMP_OP_NOEQ(x)\
		int_intern<Ti, To> operator x##=(const int_intern<Ti, To> c){*this = *this x c; return *this;}
#define K9_INTERNAL_INT_CMP_OP_NOEQ(x) \
		bool operator x(const int_intern<Ti, To> c) const {\
			return __getinterno() x (c).__getinterno();\
		}
#define K9_INTERNAL_INT_CMP_OP(x) \
		K9_INTERNAL_INT_CMP_OP_NOEQ(x)\
		bool operator x##=(const int_intern<Ti, To> c){bool result = *this x c; return result;}
#define K9_INTERNAL_INT_DIV_OP(x) \
		int_intern<Ti, To> operator x(const int_intern<Ti, To> c) const {\
			return __getinterno() x nonzero(c).__getinterno();\
		}\
		int_intern<Ti, To> operator x##=(const int_intern<Ti, To> c) {*this = *this x c; return *this;}
		K9_INTERNAL_INT_SIMP_OP(+)
		K9_INTERNAL_INT_SIMP_OP(-)
		K9_INTERNAL_INT_SIMP_OP(*)
		K9_INTERNAL_INT_SIMP_OP(&)
		K9_INTERNAL_INT_SIMP_OP(|)
		K9_INTERNAL_INT_SIMP_OP(^)
		K9_INTERNAL_INT_SIMP_OP(<<)
		K9_INTERNAL_INT_SIMP_OP(>>)
		K9_INTERNAL_INT_SIMP_OP_NOEQ(||)
		K9_INTERNAL_INT_SIMP_OP_NOEQ(&&)
		K9_INTERNAL_INT_CMP_OP_NOEQ(==)
		K9_INTERNAL_INT_CMP_OP_NOEQ(!=)
		K9_INTERNAL_INT_CMP_OP(>)
		K9_INTERNAL_INT_CMP_OP(<)
		K9_INTERNAL_INT_DIV_OP(/)
		K9_INTERNAL_INT_DIV_OP(%)
		int_intern<Ti, To> operator++(){this += 1; return *this;}
		int_intern<Ti, To> operator--(){this -= 1; return *this;}
		int_intern<Ti, To> operator++(int){int_intern<Ti, To> temp = *this; *this += 1; return temp;}
		int_intern<Ti, To> operator--(int){int_intern<Ti, To> temp = *this; *this -= 1; return temp;}
		int_intern<Ti, To> operator~(){return ~__getinterno();}
		int_intern<Ti, To> operator!(){return !__getinterno();}
#define K9_INTERNAL_INT_CONV(ti, to)\
		operator int_intern<ti, to>(){\
			return int_intern<ti, to>(__getintern());\
		}
#define K9_INTERNAL_LITERAL_INT_CONV(ti)\
		operator ti(){\
			return (ti)(__getintern());\
		}

K9_INTERNAL_INT_CONSTRUCTOR(uint8_t)
K9_INTERNAL_INT_CONSTRUCTOR(uint16_t)
K9_INTERNAL_INT_CONSTRUCTOR(uint32_t)
K9_INTERNAL_INT_CONSTRUCTOR(int8_t)
K9_INTERNAL_INT_CONSTRUCTOR(int16_t)
K9_INTERNAL_INT_CONSTRUCTOR(int32_t)
#if K9_IMPLEMENTATION_BITS >= 64
		K9_INTERNAL_INT_CONSTRUCTOR(size_t);
		K9_INTERNAL_INT_CONSTRUCTOR(ssize_t);
#endif
#if K9_IMPLEMENTATION_BITS > 64
		K9_INTERNAL_INT_CONSTRUCTOR(uint64_t)
		K9_INTERNAL_INT_CONSTRUCTOR(int64_t)
#endif
};


typedef int_intern<uint32_t, int32_t> i32;
typedef int_intern<uint16_t, int16_t> i16;
typedef int_intern<uint8_t, int8_t> i8;
typedef int_intern<size_t, ssize_t> imax;
typedef int_intern<uint32_t, uint32_t> u32;
typedef int_intern<uint16_t, uint16_t> u16;
typedef int_intern<uint8_t, uint8_t>   u8;
typedef int_intern<size_t, size_t> umax;
static_assert(sizeof(umax) == sizeof(size_t), "Bad integer types");
static_assert(sizeof(int8_t) == sizeof(i8), "Bad integer types");
static_assert(sizeof(uint8_t) == sizeof(u8), "Bad integer types");

#if K9_IMPLEMENTATION_BITS >= 64
typedef int_intern<uint64_t, int64_t> i64;
typedef int_intern<uint64_t, uint64_t> u64;
#endif

template <typename T>
struct float_intern{
	private:
		T f;
		 static inline bool isfinite(T a){return std::isfinite(a);}
		 static inline bool isnormal(T a){return std::isnormal(a);}
	public:
		float_intern() = default;
		float_intern(const T other) {f = other;}
		void operator=(const float_intern<T> other) {f = other.__getintern();}
		void operator=(const T other) {f = other;}
		T __getintern() const {return f;}
		void __setintern(T a) {f = a;}
#define K9_INTERINAL_FLOAT_SIMP_OP(x) \
		float_intern<T> operator x(const float_intern<T> c){\
			return validate(*this).__getintern() x validate(c).__getintern();\
		}\
		float_intern<T> operator x##=(const float_intern<T> c){*this = *this x c; return *this;}
#define K9_INTERINAL_FLOAT_DIV_OP(x) \
		float_intern<T> operator x(const float_intern<T> c){\
			return validate(*this).__getintern() x nonzero(c).__getintern();\
		}\
		float_intern<T> operator x##=(const float_intern<T> c){*this = *this x c; return *this;}

		K9_INTERINAL_FLOAT_SIMP_OP(+)
		K9_INTERINAL_FLOAT_SIMP_OP(-)
		K9_INTERINAL_FLOAT_SIMP_OP(*)
		K9_INTERINAL_FLOAT_DIV_OP(/)
		float_intern<T> operator%(const float_intern<T> c){
			if((sizeof(T) == 4))
				return (fmodf(validate(*this).__getintern() , nonzero(c).__getintern()));
			if((sizeof(T) == 8))
				return (fmod(validate(*this).__getintern() , nonzero(c).__getintern()));
		}
		 void operator%=(const float_intern<T> c){*this = *this % c;}
		 void operator++(){this += 1;}
		 void operator--(){this -= 1;}
		 void operator++(int){*this += 1;}
		 void operator--(int){*this -= 1;}
		 inline float_intern<T> fabsf(){ return fabsf(validate(*this).__getintern()); }
		 inline float_intern<T> fabs(){ return fabs(validate(*this).__getintern()); }
		 inline float_intern<T> fabsl(){ return fabsl(validate(*this).__getintern()); }
		 static inline float_intern<T> validate(float_intern<T> a){
			if(K9_FAST_FLOAT_MATH) return a;
			if( !isfinite(a.__getintern()) )
				a.__setintern(0.0);
			return a;
		}
		static inline float_intern<T> nonzero(float_intern<T> a){
			if(K9_FAST_FLOAT_MATH) return a;
			if(!isnormal(a.__getintern()))
				a.__setintern(1.0);
			return a;
		}
};



typedef float_intern<float> f32;



typedef u8 BYTE;
static_assert(sizeof(float) == 4);
static_assert(sizeof(f32) == 4);

#if K9_USE_DOUBLE
typedef float_intern<double> f64;
static_assert(sizeof(double) == 8);
static_assert(sizeof(f64) == 8);
#endif

#if K9_USE_LONG_DOUBLE
typedef float_intern<long double> f128;
static_assert(sizeof(long double) == 16);
static_assert(sizeof(f128) == 16);
#endif



template <size_t lev, size_t size>
static inline umax fix_index(umax index){
	static const umax mask = (((umax)1<<(size-1))/((umax)1<<(umax)(lev-1)))-1;
	index &= mask;
	return index;
}

//Bus!
template <size_t size> 
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
		template<size_t lev> 
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
			for(size_t i = 0; i < sizeof(T); i++){
				write( *((BYTE*)&a), i );
			}
		}
		template <typename T>
		void fromp_(T* a){
			for(size_t i = 0; i < sizeof(T); i++){
				write( *((BYTE*)a), i );
			}
		}
		virtual ~k9_bus(){}
};

template <size_t size> 
class k9_ptrmem : public k9_bus<size>{
	public:
		k9_ptrmem<size>(BYTE* p) : data(p) {}
		inline BYTE read(umax where) {		  return  data[where & ((umax)1<<(size-1))];	}
		inline void write(BYTE byte, umax where) {	data[where & ((umax)1<<(size-1))] = byte;}
		template<size_t lev> k9_ptrmem<lev> at(umax index){
			k9_ptrmem<lev> k(data + fix_index<lev, size>(index) );
			return k;
		}
	private:
		BYTE* data;
};

//MY WORDS ARE BACKED WITH... real memory...
template <size_t size> 
class k9_realmem : public k9_bus<size>{
	public:
		inline BYTE read(size_t where) {		return  data[fix_index<1,size>(where)];		}
		inline void write(BYTE byte, size_t where) {	data[fix_index<1,size>(where)] = byte;	}
		template<size_t lev> k9_ptrmem<lev> at(umax index){
			k9_ptrmem<lev> k(data + fix_index<lev, size>(index) );
			return k;
		}
		operator k9_ptrmem<size>() {return k9_ptrmem<size>(data);}
	private:
		BYTE data[(size_t)1<<(size-1)];
};
f32 myfunc(f32 a, f32 b) {a++; a %= b; return a; }
u32 myfunc2(u32 a, i32 b){ b = 0; i32 q; a++;    q = 7;     a+=q;     return a%b; }
static k9_realmem<10> mymem;

void myfunc(){
	k9_realmem<11> some_memory;
	some_memory.fill(0);
	some_memory.at<3>(39).from_<u32>(0x7f3f3f7f);
	std::cout << "Value at the 39th state3, byte 0 is..." << 
		(uint8_t)some_memory.at<3>(39).read(0) << 
	std::endl;
}
