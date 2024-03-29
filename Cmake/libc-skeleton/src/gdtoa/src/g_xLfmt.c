/****************************************************************

The author of this software is David M. Gay.

Copyright (C) 1998 by Lucent Technologies
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

#undef _0
#undef _1

#ifdef IEEE_8087
	#define _0 2
	#define _1 1
	#define _2 0
#else
	#error Something went wrong, IEEE8087 is not defined
#endif

char* g_xLfmt(char* buf, void* V, int ndig, unsigned bufsize)
{
	static FPI fpi = {64, 1 - 16383 - 64 + 1, 32766 - 16383 - 64 + 1, 1, 0};
	char* b;
	char* s;
	char* se;
	uint32_t bits[2];
	uint32_t* L;
	uint32_t sign;
	int decpt;
	int ex;
	int i;
	int mode;

	if(ndig < 0)
	{
		ndig = 0;
	}

	if(bufsize < (unsigned)ndig + 10)
	{
		return 0;
	}

	L = (uint32_t*)V;
	sign = L[_0] & 0x80000000L;
	bits[1] = L[_1];
	bits[0] = L[_2];

	if((ex = (int)((L[_0] >> 16) & 0x7fff)) != 0)
	{
		if(ex == 0x7fff)
		{
			/* Infinity or NaN */
			if(bits[0] | bits[1])
			{
				b = strcp(buf, "NaN");
			}
			else
			{
				b = buf;

				if(sign)
				{
					*b++ = '-';
				}

				b = strcp(b, "Infinity");
			}

			return b;
		}

		i = STRTOG_Normal;
	}
	else if(bits[0] | bits[1])
	{
		i = STRTOG_Denormal;
	}
	else
	{
		b = buf;
#ifndef IGNORE_ZERO_SIGN
		if(sign)
		{
			*b++ = '-';
		}
#endif
		*b++ = '0';
		*b = 0;

		return b;
	}

	ex -= 0x3fff + 63;
	mode = 2;

	if(ndig <= 0)
	{
		if(bufsize < 32)
		{
			return 0;
		}

		mode = 0;
	}

	s = gdtoa(&fpi, ex, bits, &i, mode, ndig, &decpt, &se);

	return g__fmt(buf, s, se, decpt, sign);
}
