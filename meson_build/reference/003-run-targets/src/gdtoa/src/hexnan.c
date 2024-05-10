/****************************************************************

The author of this software is David M. Gay.

Copyright (C) 2000 by Lucent Technologies
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

static void L_shift(uint32_t* x, const uint32_t* x1, int i)
{
	int j;

	i = 8 - i;
	i <<= 2;
	j = ULbits - i;

	do
	{
		*x |= x[1] << j;
		x[1] >>= i;
	} while(++x < x1);
}

int hexnan(const char** sp, FPI* fpi, uint32_t* x0)
{
	uint32_t c;
	uint32_t h;
	uint32_t* x;
	uint32_t* x1;
	uint32_t* xe;
	const char* s;
	int havedig;
	int hd0;
	int i;
	int nbits;

	if(!hexdig['0'])
	{
		hexdig_init();
	}

	nbits = fpi->nbits;
	x = x0 + (nbits >> kshift);

	if(nbits & kmask)
	{
		x++;
	}

	*--x = 0;
	x1 = xe = x;
	havedig = hd0 = i = 0;
	s = *sp;

	/* allow optional initial 0x or 0X */
	while((c = *(const unsigned char*)(s + 1)) && c <= ' ')
	{
		++s;
	}

	if(s[1] == '0' && (s[2] == 'x' || s[2] == 'X') && *(const unsigned char*)(s + 3) > ' ')
	{
		s += 2;
	}

	while((c = *(const unsigned char*)++s))
	{
		if(!(h = hexdig[c]))
		{
			if(c <= ' ')
			{
				if(hd0 < havedig)
				{
					if(x < x1 && i < 8)
					{
						L_shift(x, x1, i);
					}

					if(x <= x0)
					{
						i = 8;
						continue;
					}

					hd0 = havedig;
					*--x = 0;
					x1 = x;
					i = 0;
				}

				while(*(const unsigned char*)(s + 1) <= ' ')
				{
					++s;
				}

				if(s[1] == '0' && (s[2] == 'x' || s[2] == 'X') &&
				   *(const unsigned char*)(s + 3) > ' ')
				{
					s += 2;
				}

				continue;
			}

			if(/*(*/ c == ')' && havedig)
			{
				*sp = s + 1;
				break;
			}

#ifndef GDTOA_NON_PEDANTIC_NANCHECK
			do
			{
				if(/*(*/ c == ')')
				{
					*sp = s + 1;
					break;
				}
			} while((c = (uint32_t) * ++s));
#endif

			return STRTOG_NaN;
		}

		havedig++;

		if(++i > 8)
		{
			if(x <= x0)
			{
				continue;
			}

			i = 1;
			*--x = 0;
		}

		*x = (*x << 4) | (h & 0xf);
	}

	if(!havedig)
	{
		return STRTOG_NaN;
	}

	if(x < x1 && i < 8)
	{
		L_shift(x, x1, i);
	}

	if(x > x0)
	{
		x1 = x0;
		do
		{
			*x1++ = *x++;
		} while(x <= xe);

		do
		{
			*x1++ = 0;
		} while(x1 <= xe);
	}
	else
	{
		/* truncate high-order word if necessary */
		if((i = nbits & (ULbits - 1)) != 0)
		{
			*xe &= ((uint32_t)0xffffffff) >> (ULbits - i);
		}
	}

	for(x1 = xe;; --x1)
	{
		if(*x1 != 0)
		{
			break;
		}

		if(x1 == x0)
		{
			*x1 = 1;
			break;
		}
	}

	return STRTOG_NaNbits;
}