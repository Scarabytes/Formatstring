// formatstring/stringify/Grisu2.cpp
//
// This file contains an implementation of the Grisu algorithm by
// Florian Loitsch, which is used to turn floating point numbers into decimal
// digits.

#include "formatstring/stringify/FloatToString.h"

#include <cmath>
#include <formatstring/QuickFormat.h>

#include "formatstring/util/Assert.h"


namespace fs {
namespace detail {

namespace {

const uint64_t POW10_SIGNIFICANDS[] = {
		0xfa8fd5a0081c0288, 0xbaaee17fa23ebf76, 0x8b16fb203055ac76,
		0xcf42894a5dce35ea, 0x9a6bb0aa55653b2d, 0xe61acf033d1a45df,
		0xab70fe17c79ac6ca, 0xff77b1fcbebcdc4f, 0xbe5691ef416bd60c,
		0x8dd01fad907ffc3c, 0xd3515c2831559a83, 0x9d71ac8fada6c9b5,
		0xea9c227723ee8bcb, 0xaecc49914078536d, 0x823c12795db6ce57,
		0xc21094364dfb5637, 0x9096ea6f3848984f, 0xd77485cb25823ac7,
		0xa086cfcd97bf97f4, 0xef340a98172aace5, 0xb23867fb2a35b28e,
		0x84c8d4dfd2c63f3b, 0xc5dd44271ad3cdba, 0x936b9fcebb25c996,
		0xdbac6c247d62a584, 0xa3ab66580d5fdaf6, 0xf3e2f893dec3f126,
		0xb5b5ada8aaff80b8, 0x87625f056c7c4a8b, 0xc9bcff6034c13053,
		0x964e858c91ba2655, 0xdff9772470297ebd, 0xa6dfbd9fb8e5b88f,
		0xf8a95fcf88747d94, 0xb94470938fa89bcf, 0x8a08f0f8bf0f156b,
		0xcdb02555653131b6, 0x993fe2c6d07b7fac, 0xe45c10c42a2b3b06,
		0xaa242499697392d3, 0xfd87b5f28300ca0e, 0xbce5086492111aeb,
		0x8cbccc096f5088cc, 0xd1b71758e219652c, 0x9c40000000000000,
		0xe8d4a51000000000, 0xad78ebc5ac620000, 0x813f3978f8940984,
		0xc097ce7bc90715b3, 0x8f7e32ce7bea5c70, 0xd5d238a4abe98068,
		0x9f4f2726179a2245, 0xed63a231d4c4fb27, 0xb0de65388cc8ada8,
		0x83c7088e1aab65db, 0xc45d1df942711d9a, 0x924d692ca61be758,
		0xda01ee641a708dea, 0xa26da3999aef774a, 0xf209787bb47d6b85,
		0xb454e4a179dd1877, 0x865b86925b9bc5c2, 0xc83553c5c8965d3d,
		0x952ab45cfa97a0b3, 0xde469fbd99a05fe3, 0xa59bc234db398c25,
		0xf6c69a72a3989f5c, 0xb7dcbf5354e9bece, 0x88fcf317f22241e2,
		0xcc20ce9bd35c78a5, 0x98165af37b2153df, 0xe2a0b5dc971f303a,
		0xa8d9d1535ce3b396, 0xfb9b7cd9a4a7443c, 0xbb764c4ca7a44410,
		0x8bab8eefb6409c1a, 0xd01fef10a657842c, 0x9b10a4e5e9913129,
		0xe7109bfba19c0c9d, 0xac2820d9623bf429, 0x80444b5e7aa7cf85,
		0xbf21e44003acdd2d, 0x8e679c2f5e44ff8f, 0xd433179d9c8cb841,
		0x9e19db92b4e31ba9, 0xeb96bf6ebadf77d9, 0xaf87023b9bf0ee6b
};

// Binary exponents of pow(10, k), for k = -348, -340, ..., 340, corresponding
// to significands above.
const int16_t POW10_EXPONENTS[] = {
		-1220, -1193, -1166, -1140, -1113, -1087, -1060, -1034, -1007,  -980,
		-954,   -927,  -901,  -874,  -847,  -821,  -794,  -768,  -741,  -715,
		-688,   -661,  -635,  -608,  -582,  -555,  -529,  -502,  -475,  -449,
		-422,   -396,  -369,  -343,  -316,  -289,  -263,  -236,  -210,  -183,
		-157,   -130,  -103,   -77,   -50,   -24,     3,    30,    56,    83,
		109,     136,   162,   189,   216,   242,   269,   295,   322,   348,
		375,     402,   428,   455,   481,   508,	534,   561,   588,   614,
		641,     667,   694,   720,   747,   774,   800,   827,   853,   880,
		907,     933,   960,   986,  1013,  1039,  1066,
};
}

/**
 * Calculates lhs * rhs / 2^64 rounded to nearest, up on tie. Result may be
 * denormalized.
 */
fp mul(const fp& lhs, const fp& rhs)
{
	uint64_t mask32 = 0xffffffff;
	// Dividing the significands into two 32bit segments
	uint64_t a = lhs.f >> 32;
	uint64_t b = lhs.f & mask32;
	uint64_t c = rhs.f >> 32;
	uint64_t d = rhs.f & mask32;
	// Multiply the segments
	uint64_t ac = a * c;
	uint64_t ad = a * d;
	uint64_t bc = b * c;
	uint64_t bd = b * d;
	// Add the less significant bits together to get the rounding behaviour
	uint64_t rnd = (bd >> 32) + (ad & mask32) + (bc & mask32);
	rnd += (1ull << 31); // effectively adding 0.5...
	// Sum everything up
	return {ac + (ad >> 32) + (bc >> 32) + (rnd >> 32), lhs.e + rhs.e + 64};
}

/**
 * Normalizes the given value so that the most significant bit (64) is not zero.
 * Expects a value that has no higher bit than the 54th set.
 */
fp normalize_boundary(const fp& v)
{
	fp r = v;
	while (!(r.f & (1ull << 53))) {
		r.f <<= 1;
		--r.e;
	}
	// Shift to beginning of the 64 bit uint
	r.f <<= 64 - 54;
	r.e -= 64 - 54;
	return r;
}

/** Calculates the lower and upper boundaries m^- and m^+ of the given number.*/
void compute_boundaries(const fp& v, fp& upper, fp& lower)
{
	uint64_t msb = (1ull << 52);
	upper = {(v.f << 1) + 1ull, v.e - 1};
	if (v.f == msb)
		// If the significand is the most significant bit only, its predecessor
		// is {(f<<1)-1, e-1}, thus the lower boundary needs to be shifted one
		// bit further to get the mean of v and its predecessor.
		lower = {(v.f << 2) - 1ull, v.e - 2};
	else
		lower = {(v.f << 1) - 1ull, v.e - 1};
	
	upper = normalize_boundary(upper);
	if (lower.e - upper.e > 0)
		lower.f <<= lower.e - upper.e;
	else
		lower.f >>= upper.e - lower.e;
	lower.e = upper.e;
}

/**
 * Finds a cached power of 10 that has at least the binary exponent e_min.
 * Stores the decimal exponent of that number in k and returns the number
 * itself.
 */
fp cached_power(int e_min, int& k)
{
	const double inverse_ld_10 = 0.30102999566398114; // = 1 / log_2(10)
	int k_optimal = static_cast<int>(ceil((e_min + 64 - 1) * inverse_ld_10));
	
	const int first_cached_exp = -348;
	
	int index = (k_optimal - first_cached_exp - 1) / 8 + 1;
	k = index * 8 + first_cached_exp;
	assertmsg(POW10_EXPONENTS[index] >= e_min,
			"e_c=" << POW10_EXPONENTS[index] << ", e_min=" << e_min);
	
	return {POW10_SIGNIFICANDS[index], POW10_EXPONENTS[index]};
}

decimal generate_digits(fp M_up, uint64_t delta, int k)
{
	fp one = {1ull << -M_up.e, M_up.e};
	uint32_t part1 = static_cast<uint32_t>(M_up.f >> -M_up.e); // effectively div 2^-e_M_up
	uint64_t part2 = M_up.f & (one.f - 1ull); // effectively mod 2^-e_M_up
	std::string out;
	
	int kappa = 10;
	uint32_t div = 1000000000;
	
	while (kappa > 0) {
		int32_t d = part1 / div;
		if (d > 0 || !out.empty())
			out += static_cast<char>('0' + d);
		part1 %= div;
		div /= 10;
		--kappa;
		if ((static_cast<uint64_t>(part1) << -one.e) + part2 <= delta)
			return {out, kappa - k};
	}
	
	do {
		part2 *= 10;
		int64_t d = part2 >> -one.e;
		if (d > 0 || !out.empty())
			out += static_cast<char>('0' + d);
		part2 &= (one.f - 1ull);
		delta *= 10;
		--kappa;
		
	} while (part2 > delta);
	
	return {out, kappa - k};
}

decimal grisu2(fp v)
{
	if (v.f == 0)
		return {"0", 0};
	
	fp m_up, m_low;
	compute_boundaries(v, m_up, m_low);
	
	int k;
	fp c_k = cached_power(-59 - (m_up.e + 64), k);
	
	fp M_up = mul(m_up, c_k);
	--M_up.f; // - 1 ulp
	fp M_low = mul(m_low, c_k);
	++M_low.f; // + 1 ulp
	uint64_t delta = M_up.f - M_low.f;
	
	return generate_digits(M_up, delta, k);
}

} // namespace detail
} // namespace fs
