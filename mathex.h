#ifndef mathex_h
#define mathex_h

#include <cmath>

const double EPSILON = 1.0e-8;
#define ZERO EPSILON

template <class T> inline T sqr(const T &x) { return x*x; }


template <class T> inline T limit(const T &x, const T &lower, const T &upper) {

	if (x < lower) return lower;
	if (x > upper) return upper;

	return x;

}


#endif

