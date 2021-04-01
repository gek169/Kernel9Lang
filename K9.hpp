#include <cstdlib>
#include <cstdint>
#include <memory>
#include <cassert>
#include <cmath>
#include <ctgmath>
#include <type_traits>

//Implementation variables.
#define K9BLOCKSIZE ((ssize_t)1<<19)
#define K9BLOCKMASK (K9BLOCKSIZE-1)
#define K9_FAST_FLOAT_MATH 1
#define K9_USE_DOUBLE 1
#define K9_USE_LONG_DOUBLE 1
#define K9_IMPLEMENTATION_BITS 64
typedef unsigned char BYTE;



/*
template <size_t size>
class Membacker{
	public:
		virtual BYTE read(size_t where);
		virtual void write(uint8_t byte, size_t where);
		void copy(Membacker<size>& other){
			for(ssize_t i = 0; i < (1<<(size-1)); i++){write(other.read(i),i);}
		}
		virtual ~Membacker();
};

*/

template <typename T>
struct float_intern{
	private:
		T f;
		 static inline bool isfinite(T a){return std::isfinite(a);}
		 static inline bool isnormal(T a){return std::isnormal(a);}
	public:
		float_intern() = default;
		float_intern(const T other){f = other;}
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

//Signed integer implementation.
template <typename Ti, typename To>
struct int_intern{
	static_assert(sizeof(Ti) == sizeof(To));
	private:
		Ti f;
		static inline int_intern<Ti, To> nonzero(int_intern<Ti, To> a){
			if(a.__getintern() == 0)
				a.__setintern(0);
			return a;
		}
	public:	
		int_intern<Ti, To>() = default;
		int_intern<Ti, To>(To in){*this = in;}
		int_intern<Ti, To>(Ti in){*this = in;}
		void operator=(const int_intern<Ti, To> other) 		{f = other.__getintern();}
		void operator=(const To other) 						{f = (Ti)(other);}
		Ti __getintern() const 								{return 	f;}
		To __getinterno() const 							{return (To)(f);}
		void __setintern(Ti a) 								{f = a;}
		void __setinterno(To a)								{f = (Ti)(a);}
	//Actual operator implementations.
#define K9_INTERNAL_INT_SIMP_OP_NOEQ(x) \
		int_intern<Ti, To> operator x(const int_intern<Ti, To> c){\
			return __getintern() x c.__getintern();\
		}
#define K9_INTERNAL_INT_SIMP_OP(x) \
		K9_INTERNAL_INT_SIMP_OP_NOEQ(x)\
		int_intern<Ti, To> operator x##=(const int_intern<Ti, To> c){*this = *this x c; return *this;}
#define K9_INTERNAL_INT_CMP_OP_NOEQ(x) \
		bool operator x(const int_intern<Ti, To> c){\
			return __getinterno() x (c).__getinterno();\
		}
#define K9_INTERNAL_INT_CMP_OP(x) \
		K9_INTERNAL_INT_CMP_OP_NOEQ(x)\
		bool operator x##=(const int_intern<Ti, To> c){bool result = *this x c; return result;}
#define K9_INTERNAL_INT_DIV_OP(x) \
		int_intern<Ti, To> operator x(const int_intern<Ti, To> c){\
			return __getinterno() x nonzero(c).__getinterno();\
		}\
		int_intern<Ti, To> operator x##=(const int_intern<Ti, To> c){*this = *this x c; return *this;}
		
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
		//int_intern<Ti, To> operator==(int_intern<Ti, To> a){return a.__getinterno() == __getinterno();}
		int_intern<Ti, To> operator++(){this += 1; return *this;}
		int_intern<Ti, To> operator--(){this -= 1; return *this;}
		int_intern<Ti, To> operator++(int){auto temp = *this; *this += 1; return temp;}
		int_intern<Ti, To> operator--(int){auto temp = *this; *this -= 1; return temp;}
		int_intern<Ti, To> operator~(){return ~__getinterno();}
		int_intern<Ti, To> operator!(){return !__getinterno();}
		//operator int_intern<Ti, Ti>(){int_intern<Ti, Ti> a; a.__setintern(__getintern()); return a;}
};



typedef float_intern<float> f32;
typedef int_intern<uint32_t, int32_t> i32;
typedef int_intern<uint16_t, int16_t> i16;
typedef int_intern<uint8_t, int8_t> i8;

typedef int_intern<uint32_t, uint32_t> u32;
typedef int_intern<uint16_t, uint16_t> u16;
typedef int_intern<uint8_t, uint8_t>   u8;
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

#if K9_IMPLEMENTATION_BITS >= 64

#endif

template <size_t size> class state_interface {
	public:
		template<size_t lev> state_interface<lev>& at(size_t index);
		virtual BYTE read(size_t where);
		virtual void write(uint8_t byte, size_t where);
		void copy(state_interface<size>& other){
			for(ssize_t i = 0; i < ((size_t)1<<(size-1)); i++) 
				{write(other.read(i),i);}
		}
		template <size_t lvl> state_interface<lvl> get();
		state_interface<size-1> high();
		state_interface<size-1> low();
		void operator=(state_interface<size>& other){ copy(other); }
		template <typename T>
		T to_(){
			BYTE data[sizeof(T)];
			for(size_t i = 0; i < sizeof(T); i++){
				data[i] = read(i);
			}
			T var;
			memcpy(&var, data, sizeof(T));
			return var;
		}
		f32 to_f32(){return to_<f32>();}
#if K9_USE_DOUBLE
		f64 to_f64(){return to_<f64>();}
#endif
#if K9_USE_LONG_DOUBLE
		f128 to_f128(){return to_<f128>();}
#endif
		virtual ~state_interface();
};
f32 myfunc(f32 a, f32 b){  a++; a %= b; return a; }
i32 myfunc2(i32 a, i32 b){i32 q; a++; q = 7; a+=q; return a/b; }
