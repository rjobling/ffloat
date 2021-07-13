////////////////////////////////////////////////////////////////////////////////
// ffloat.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <proto/mathffp.h>
#include "ffpieee.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct ffloat
{
	float value;

	ffloat() {}
	ffloat(const ffloat& f) : value(f.value) {}
	explicit ffloat(int i) : value(SPFlt(i)) {}

	explicit operator int() const { return SPFix(value); }
	explicit operator char() const { return ((char) SPFix(value)); }
	explicit operator short() const { return ((short) SPFix(value)); }
	explicit operator unsigned int() const { return ((unsigned int) SPFix(value)); }
	explicit operator unsigned char() const { return ((unsigned char) SPFix(value)); }
	explicit operator unsigned short() const { return ((unsigned short) SPFix(value)); }

	ffloat& operator=(ffloat f) { value = f.value; return *this; }
	ffloat& operator+=(ffloat f) { value = SPAdd(f.value, value); return *this; }
	ffloat& operator-=(ffloat f) { value = SPSub(f.value, value); return *this; }
	ffloat& operator*=(ffloat f) { value = SPMul(f.value, value); return *this; }
	ffloat& operator/=(ffloat f) { value = SPDiv(f.value, value); return *this; }

	ffloat operator-() const { ffloat r; r.value = value; asm("eor.b #0x80,%[r]" : [r] "+d" (r.value) : : "cc"); return r; }
	ffloat operator+(const ffloat& f) const { ffloat r; r.value = SPAdd(f.value, value); return r; }
	ffloat operator-(const ffloat& f) const { ffloat r; r.value = SPSub(f.value, value); return r; }
	ffloat operator*(const ffloat& f) const { ffloat r; r.value = SPMul(f.value, value); return r; }
	ffloat operator/(const ffloat& f) const { ffloat r; r.value = SPDiv(f.value, value); return r; }

	bool operator==(ffloat f) const { bool r; asm("cmp.l %[a],%[b]\n seq.b %[r]\n neg.b %[r]" : [r] "=d" (r) : [a] "d" (value), [b] "d" (f.value) : "cc"); return r; }
	bool operator!=(ffloat f) const { bool r; asm("cmp.l %[a],%[b]\n sne.b %[r]\n neg.b %[r]" : [r] "=d" (r) : [a] "d" (value), [b] "d" (f.value) : "cc"); return r; }
	bool operator>(ffloat f) const { return (SPCmp(value, f.value) > 0); }
	bool operator<(ffloat f) const { return (SPCmp(value, f.value) < 0); }
	bool operator>=(ffloat f) const { return (SPCmp(value, f.value) >= 0); }
	bool operator<=(ffloat f) const { return (SPCmp(value, f.value) <= 0); }

private:
	ffloat(float f);
};

////////////////////////////////////////////////////////////////////////////////
// Conversions.
////////////////////////////////////////////////////////////////////////////////
inline ffloat ffloat_from_ieee(float f) { ffloat r; r.value = ffp_from_ieee(f); return r; }
inline float ieee_from_ffloat(ffloat f) { return ieee_from_ffp(f.value); }

////////////////////////////////////////////////////////////////////////////////
// Consts.
////////////////////////////////////////////////////////////////////////////////
const ffloat ff_zero			  = ffloat_from_ieee(0.0f);
const ffloat ff_one				  = ffloat_from_ieee(1.0f);
const ffloat ff_two				  = ffloat_from_ieee(2.0f);
const ffloat ff_half			  = ffloat_from_ieee(0.5f);
const ffloat ff_pi				  = ffloat_from_ieee(Pi);
const ffloat ff_one_over_pi		  = ffloat_from_ieee(1.0f / Pi);
const ffloat ff_two_pi			  = ffloat_from_ieee(2.0f * Pi);
const ffloat ff_two_over_pi		  = ffloat_from_ieee(2.0f / Pi);
const ffloat ff_half_pi			  = ffloat_from_ieee(0.5f * Pi);
const ffloat ff_one_and_a_half_pi = ffloat_from_ieee(1.5f * Pi);
const ffloat ff_four_over_pi	  = ffloat_from_ieee(4.0f / Pi);
const ffloat ff_pi_over_oneeighty = ffloat_from_ieee(Pi / 180.0f);
const ffloat ff_oneeighty_over_pi = ffloat_from_ieee(180.0f / Pi);

////////////////////////////////////////////////////////////////////////////////
// Math.
////////////////////////////////////////////////////////////////////////////////
inline ffloat abs(ffloat f) { asm("and.b #0x7f,%[f]" : [f] "+d" (f.value) : : "cc"); return f; }
inline ffloat sign(ffloat f) { ffloat r; asm("move.l #0x80000041,%[r]\n and.b #0x80,%[f]\n or.b %[f],%[r]" : [r] "=d" (r.value), [f] "+d" (f.value) : : "cc"); return r; }
inline ffloat floor(ffloat f) { ffloat r; r.value = SPFloor(f.value); return r; }
inline ffloat ceil(ffloat f) { ffloat r; r.value = SPCeil(f.value); return r; }
inline ffloat round(ffloat f) { return ((SPTst(f.value) < 0) ? ceil(f - ff_half) : floor(f + ff_half)); }

////////////////////////////////////////////////////////////////////////////////
// Trig.
////////////////////////////////////////////////////////////////////////////////
inline ffloat degrees_to_radians(ffloat degrees) { return (degrees * ff_pi_over_oneeighty); }
inline ffloat radians_to_degrees(ffloat radians) { return (radians * ff_oneeighty_over_pi); }
ffloat cos(ffloat x);
ffloat sin(ffloat x);
ffloat tan(ffloat x);
ffloat log2(ffloat x);
ffloat exp2(ffloat x);
