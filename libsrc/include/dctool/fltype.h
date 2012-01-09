#ifndef __Header_fltype__
#define __Header_fltype__

#include <float.h>

#ifdef POWINTEGEREXPONENTTYPE
#define	powi(m,e)	pow(m,(POWINTEGEREXPONENTTYPE)(e))
#else
#define	powi(m,e)	pow(m,(long)(e))
#endif

#ifdef __SC__
#define infinity()    HUGE_VAL
#define quiet_nan(x)  0
#endif /* __SC__ */

#ifdef __MWERKS__
#define infinity()    __inf()
#define quiet_nan(x)  0
#endif /* __MWERKS__ */

#ifdef USEDUMBINFINITYANDNAN
#if USEDUMBINFINITYANDNAN
#define infinity()    FLT_MAX
#define quiet_nan(x)  0
#endif
#endif

template <class T>
class IEEE_Float32_Base {
private:
	T binary;
public:
	IEEE_Float32_Base(void)
		{
			memset((char *)this,0,sizeof *this);
		}

	IEEE_Float32_Base<T>& operator=(const IEEE_Float32_Base<T>& i)
		{
			memcpy((char *)this,(const char *)&i,sizeof *this);
			return *this;
		}

	operator double() const
		{
			double value;
			Int16 sign	=(Int16)((binary&0x80000000)>>31);
			Int16 exponent	=(Int16)((binary&0x7f800000)>>23);
			Uint32 mantissa	= binary&0x007fffff;

			if (exponent == 0) {
				if (mantissa == 0)
					value=0;
				else {
					value=((double)mantissa/
						(1<<23))*powi(2.0,-126);
					value = (sign == 0) ? value : -value;
				}
			}
			else if (exponent == 255) {
				if (mantissa)
					value=quiet_nan(0);
				else
					value=infinity();
			}
			else {
				value=(1.0+(double)mantissa/
					(1<<23))*powi(2.0,exponent-127);
				value = (sign == 0) ? value : -value;
			}
			return value;
		}
};

typedef IEEE_Float32_Base<Uint32_B> IEEE_Float32_B;
typedef IEEE_Float32_Base<Uint32_L> IEEE_Float32_L;

template <class T,int isbig>
class IEEE_Float64_Base {
private:
	T binary1;
	T binary2;
public:
	IEEE_Float64_Base(void)
		{
			memset((char *)this,0,sizeof *this);
		}

	IEEE_Float64_Base<T,isbig>& operator=(const IEEE_Float64_Base<T,isbig>& i)
		{
			memcpy((char *)this,(const char *)&i,sizeof *this);
			return *this;
		}

	operator double() const
		{
			double value;
			Uint32 high = isbig ? binary1 : binary2;
			Uint32 low  = isbig ? binary2 : binary1;

			Int16 sign	=(Int16)((high&0x80000000)>>31);
			Int16 exponent	=(Int16)((high&0x7ff00000)>>20);
			Uint32 mantissahigh = high&0x000fffff;
			Uint32 mantissalow  = low;
			double mantissavalue = (double)mantissahigh * powi(2.0,32)
					     + (double)mantissalow;

			if (exponent == 0) {
				if (mantissahigh == 0 && mantissalow == 0)
					value=0;
				else {
					value=(mantissavalue/powi(2.0,52))
						*powi(2.0,-1022);
					value = (sign == 0) ? value : -value;
				}
			}
			else if (exponent == 255) {
				if (mantissahigh || mantissalow)
					value=quiet_nan(0);
				else
					value=infinity();
			}
			else {
				value=(1.0+mantissavalue/powi(2.0,52))
					*powi(2.0,exponent-1023);
				value = (sign == 0) ? value : -value;
			}
			return value;
		}
};

typedef IEEE_Float64_Base<Uint32_B,1> IEEE_Float64_B;
typedef IEEE_Float64_Base<Uint32_L,0> IEEE_Float64_L;

class Vax_Float_F {
private:
	Uint32_L binary;
public:
	Vax_Float_F(void)
		{
			memset((char *)this,0,sizeof *this);
		}

	Vax_Float_F& operator=(const Vax_Float_F& i)
		{
			memcpy((char *)this,(const char *)&i,sizeof *this);
			return *this;
		}

	operator double() const
		{
			double value;
			Uint32 mantissalow	= (binary&0xffff0000)>>16;
			Int16  sign		= (Int16)((binary&0x00008000)>>15);
			Int16  exponent		= (Int16)((binary&0x00007f80)>>7);
			Uint32 mantissahigh	= binary&0x0000007f;
			double mantissavalue    = (double)mantissahigh *
				powi(2.0,16) + (double)mantissalow;
			double mantissafraction = mantissavalue/powi(2.0,23);

			if (exponent == 0 && mantissalow == 0 && mantissahigh == 0) {
				value=0;
			}
			else {
			// Always normalized ... the 1.0 is the "hidden" bit in VaxSpeak
			// the exponent is excess 128, eg 10000010 is +2
			// but refers to a binary point to the LEFT of the hidden bit
			// hence the -129 rather than -128

				value=(1.0+mantissafraction)
					*powi(2.0,exponent-129);
				value = (sign == 0) ? value : -value;
			}
			return value;
		}
};

class DG_Float_F {
private:
	Uint32_B binary;
public:
	DG_Float_F(void)
		{
			memset((char *)this,0,sizeof *this);
		}

	DG_Float_F& operator=(const Vax_Float_F& i)
		{
			memcpy((char *)this,(const char *)&i,sizeof *this);
			return *this;
		}

	operator double() const
		{
			double value;
			
			Uint32 mantissa	=         (binary&0x00ffffff);
			Int16  exponent = (Int16)((binary&0x7f000000)>>24);
			Int16  sign		= (Int16)((binary&0x80000000)>>31);

			value = (double) mantissa / (1 << 24) * pow (16.0, (long)(exponent) - 64);
			value = (sign == 0) ? value : -value;
						
			return value;
		}
};

#endif /* __Header_fltype__ */
