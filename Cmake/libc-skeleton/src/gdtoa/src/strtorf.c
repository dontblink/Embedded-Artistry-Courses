/****************************************************************

The author of this software is David M. Gay.

Copyright (C) 1998, 2000 by Lucent Technologies
All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the name of Lucent or any of its entities
not be used in advertising or publicity pertaining to
distribution of the software without specific, written prior
permission.

LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.

****************************************************************/

/* Please send bug reports to David M. Gay (dmg at acm dot org,
 * with " at " changed at "@" and " dot " changed to ".").	*/

#include "gdtoaimp.h"

void ULtof(uint32_t* L, const uint32_t* bits, int32_t exp, int k)
{
	switch(k & STRTOG_Retmask)
	{
		case STRTOG_NoNumber:
		case STRTOG_Zero:
			*L = 0;
			break;

		case STRTOG_Normal:
		case STRTOG_NaNbits:
			L[0] = (bits[0] & 0x7fffff) | (unsigned)((exp + 0x7f + 23) << 23);
			break;

		case STRTOG_Denormal:
			L[0] = bits[0];
			break;

		case STRTOG_Infinite:
			L[0] = 0x7f800000;
			break;

		case STRTOG_NaN:
			L[0] = f_QNAN;
			break;
		default:
			break;
	}
	if(k & STRTOG_Neg)
	{
		{
			L[0] |= 0x80000000L;
		}
	}
}

int strtorf(const char* s, char** sp, int rounding, float* f)
{
	static FPI fpi0 = {24, 1 - 127 - 24 + 1, 254 - 127 - 24 + 1, 1, SI};
	FPI* fpi;
	FPI fpi1;
	uint32_t bits[1];
	int32_t exp;
	int k;

	fpi = &fpi0;
	if(rounding != FPI_Round_near)
	{
		fpi1 = fpi0;
		fpi1.rounding = rounding;
		fpi = &fpi1;
	}
	k = strtodg(s, sp, fpi, &exp, bits);
	ULtof((uint32_t*)f, bits, exp, k);
	return k;
}
