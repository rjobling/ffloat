////////////////////////////////////////////////////////////////////////////////
// ffloat.cpp
////////////////////////////////////////////////////////////////////////////////

#include "ffloat.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static const ffloat c1 = ffloat_from_ieee( 0.9999932946f);
static const ffloat c2 = ffloat_from_ieee(-0.4999124376f);
static const ffloat c3 = ffloat_from_ieee( 0.0414877472f);
static const ffloat c4 = ffloat_from_ieee(-0.0012712095f);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static const ffloat t1 = ffloat_from_ieee(-3.167830270f);
static const ffloat t2 = ffloat_from_ieee( 0.134516124f);
static const ffloat t3 = ffloat_from_ieee(-4.033321984f);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static const ffloat range_over_two_pi = ffloat_from_ieee(2670176.75f);
static const ffloat two_pi_over_range = ffloat_from_ieee(3.74507039e-07f);
static const int range_mask = 0xffffff;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static const ffloat lp00 = ffloat_from_ieee(-1.45326486f);
static const ffloat lp01 = ffloat_from_ieee(0.951366714f);
static const ffloat lp02 = ffloat_from_ieee(0.501994886f);
static const ffloat lq00 = ffloat_from_ieee(0.352143751f);
static const ffloat lq01 = ff_one;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static const ffloat sqrt2 = ffloat_from_ieee(1.4142135623730950488f);
static const ffloat ep00 = ffloat_from_ieee(7.2152891521493f);
static const ffloat ep01 = ffloat_from_ieee(0.0576900723731f);
static const ffloat eq00 = ffloat_from_ieee(20.8189237930062f);
static const ffloat eq01 = ff_one;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static ffloat cos_52(ffloat x)
{
	ffloat x2 = x * x;

	return (c1 + x2 * (c2 + x2 * (c3 + c4 * x2)));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static ffloat tan_56(ffloat x)
{
	ffloat x2 = x * x;

	return (x * (t1 + t2 * x2) / (t3 + x2));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ffloat cos(ffloat x)
{
	x = abs(x);

	int i = (int) (x * range_over_two_pi);

	i &= range_mask;

	x = ffloat(i) * two_pi_over_range;

	int quadrant = (int) (x * ff_two_over_pi);

	switch (quadrant)
	{
		case 0: x =  cos_52(			x); break;
		case 1: x = -cos_52(	ff_pi - x); break;
		case 2: x = -cos_52(	x - ff_pi); break;
		case 3: x =  cos_52(ff_two_pi - x); break;
	}

	return x;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ffloat sin(ffloat x)
{
	return cos(ff_half_pi - x);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ffloat tan(ffloat x)
{
	int i = (int) (x * range_over_two_pi);

	i &= range_mask;

	x = ffloat(i) * two_pi_over_range;

	int octant = (int) (x * ff_four_over_pi);

	switch (octant)
	{
		case 0: x =			  tan_56(x							* ff_four_over_pi); break;
		case 1: x = ff_one /  tan_56((ff_half_pi - x)			* ff_four_over_pi); break;
		case 2: x = ff_one / -tan_56((x - ff_half_pi)			* ff_four_over_pi); break;
		case 3: x =			 -tan_56((ff_pi - x)				* ff_four_over_pi); break;
		case 4: x =			  tan_56((x - ff_pi)				* ff_four_over_pi); break;
		case 5: x = ff_one /  tan_56((ff_one_and_a_half_pi - x)	* ff_four_over_pi); break;
		case 6: x = ff_one / -tan_56((x - ff_one_and_a_half_pi)	* ff_four_over_pi); break;
		case 7: x =			 -tan_56((ff_two_pi - x)			* ff_four_over_pi); break;
	}

	return x;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ffloat log2(ffloat x)
{
	bool adjustment = false;
	if (x < ff_half)
	{
		x = ff_one / x;
		adjustment = true;
	}

	int i = (int) x;
	int two_to_n = 1;
	ffloat n = ff_zero;
	while (i != 0)
	{
		i >>= 1;
		two_to_n <<= 1;
		n += ff_one;
	}

	x /= ffloat(two_to_n);

	ffloat poly = (lp00 + x * (lp01 + x * lp02)) / (lq00 + lq01 * x);

	return (adjustment ? (-n - poly) : (poly + n));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ffloat exp2(ffloat x)
{
	bool negative = false;
	if (x < ff_zero)
	{
		x = -x;
		negative = true;
	}

	bool adjustment = false;
	int i = (int) x;
	if ((x - ffloat(i)) > ff_half)
	{
		adjustment = true;
		x -= ff_half;
	}
	x -= ffloat(i);

	ffloat two_int_a = ff_one;
	while (i != 0)
	{
		two_int_a *= ff_two;
		i--;
	}

	ffloat x2 = x * x;
	ffloat Q = eq00 + eq01 * x2;
	ffloat x_P = x * (ep00 + ep01 * x2);

	ffloat answer = ((Q + x_P) / (Q - x_P)) * two_int_a;

	if (adjustment)
	{
		answer *= sqrt2;
	}

	return (negative ? (ff_one / answer) : answer);
}