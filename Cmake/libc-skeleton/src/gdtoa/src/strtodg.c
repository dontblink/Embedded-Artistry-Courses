/****************************************************************

The author of this software is David M. Gay.

Copyright (C) 1998-2001 by Lucent Technologies
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
#include <stdint.h>

#ifdef USE_LOCALE
	#include "locale.h"
#endif

static const int fivesbits[] = {0,
								3,
								5,
								7,
								10,
								12,
								14,
								17,
								19,
								21,
								24,
								26,
								28,
								31,
								33,
								35,
								38,
								40,
								42,
								45,
								47,
								49,
								52
#ifdef VAX
								,
								54,
								56
#endif
};

Bigint* increment(Bigint* b)
{
	uint32_t* x;
	uint32_t* xe;
	Bigint* b1;
#ifdef Pack_16
	uint32_t carry = 1;
	uint32_t y;
#endif

	x = b->x;
	xe = x + b->wds;
#ifdef Pack_32
	do
	{
		if(*x < (uint32_t)0xffffffffL)
		{
			++*x;
			return b;
		}
		*x++ = 0;
	} while(x < xe);
#else
	do
	{
		y = *x + carry;
		carry = y >> 16;
		*x++ = y & 0xffff;
		if(!carry)
			return b;
	} while(x < xe);
	if(carry)
#endif
	{
		if(b->wds >= b->maxwds)
		{
			b1 = Balloc(b->k + 1);
			Bcopy(b1, b);
			Bfree(b);
			b = b1;
		}
		b->x[b->wds++] = 1;
	}
	return b;
}

int decrement(Bigint* b)
{
	uint32_t* x;
	uint32_t* xe;
#ifdef Pack_16
	uint32_t borrow = 1;
	uint32_t y;
#endif

	x = b->x;
	xe = x + b->wds;
#ifdef Pack_32
	do
	{
		if(*x)
		{
			--*x;
			break;
		}
		*x++ = 0xffffffffL;
	} while(x < xe);
#else
	do
	{
		y = *x - borrow;
		borrow = (y & 0x10000) >> 16;
		*x++ = y & 0xffff;
	} while(borrow && x < xe);
#endif
	return STRTOG_Inexlo;
}

static int all_on(Bigint* b, int n)
{
	uint32_t* x;
	uint32_t* xe;

	x = b->x;
	xe = x + (n >> kshift);
	while(x < xe)
	{
		{
			if((*x++ & ALL_ON) != ALL_ON)
			{
				{
					return 0;
				}
			}
		}
	}
	if(n &= kmask)
	{
		{
			return ((*x | (ALL_ON << n)) & ALL_ON) == ALL_ON;
		}
	}
	return 1;
}

Bigint* set_ones(Bigint* b, int n)
{
	int k;
	uint32_t* x;
	uint32_t* xe;

	k = (n + ((1 << kshift) - 1)) >> kshift;
	if(b->k < k)
	{
		Bfree(b);
		b = Balloc(k);
	}
	k = n >> kshift;
	if(n &= kmask)
	{
		{
			k++;
		}
	}
	b->wds = k;
	x = b->x;
	xe = x + k;
	while(x < xe)
	{
		{
			*x++ = ALL_ON;
		}
	}
	if(n)
	{
		{
			x[-1] >>= ULbits - n;
		}
	}
	return b;
}

static int rvOK(double d, FPI* fpi, int32_t* exp, uint32_t* bits, int exact, int rd, int* irv)
{
	Bigint* b;
	uint32_t carry = 0;
	uint32_t inex;
	uint32_t lostbits;
	int bdif;
	int e;
	int j;
	int k;
	int k1;
	int nb;
	int rv = 0;

	b = d2b(d, &e, &bdif);
	bdif -= nb = fpi->nbits;
	e += bdif;
	if(bdif <= 0)
	{
		if(exact)
		{
			{
				goto trunc;
			}
		}
		goto ret;
	}
	if(P == nb)
	{
		if(
#ifndef IMPRECISE_INEXACT
			exact &&
#endif
			fpi->rounding ==
#ifdef RND_PRODQUOT
				FPI_Round_near
#else
			Flt_Rounds
#endif
		)
			goto trunc;
		goto ret;
	}
	switch(rd)
	{
		case 1:
			goto trunc;
		case 2:
			break;
		default: /* round near */
			k = bdif - 1;
			if(k < 0)
			{
				goto trunc;
			}
			if(!k)
			{
				if(!exact)
				{
					goto ret;
				}
				if(b->x[0] & 2)
				{
					break;
				}
				goto trunc;
			}
			if(b->x[k >> kshift] & ((uint32_t)1 << (k & kmask)))
			{
				break;
			}
			goto trunc;
	}
	/* "break" cases: round up 1 bit, then truncate; bdif > 0 */
	carry = 1;
trunc:
	inex = lostbits = 0;
	if(bdif > 0)
	{
		if((lostbits = any_on(b, bdif)) != 0)
		{
			{
				inex = STRTOG_Inexlo;
			}
		}
		rshift(b, bdif);
		if(carry)
		{
			inex = STRTOG_Inexhi;
			b = increment(b);
			if((j = nb & kmask) != 0)
			{
				j = ULbits - j;
			}
			if(hi0bits(b->x[b->wds - 1]) != j)
			{
				if(!lostbits)
				{
					lostbits = b->x[0] & 1;
				}
				rshift(b, 1);
				e++;
			}
		}
	}
	else if(bdif < 0)
	{
		b = lshift(b, -bdif);
	}
	if(e < fpi->emin)
	{
		k = fpi->emin - e;
		e = fpi->emin;
		if(k > nb || fpi->sudden_underflow)
		{
			b->wds = 0;
			inex = 0;
			*irv = STRTOG_Underflow | STRTOG_Inexlo;
		}
		else
		{
			k1 = k - 1;
			if(k1 > 0 && !lostbits)
			{
				lostbits = any_on(b, k1);
			}
			if(!lostbits && !exact)
			{
				goto ret;
			}
			carry = (uint32_t)(b->x[k1 >> kshift]) & (uint32_t)(1 << (k1 & kmask));
			lostbits |= carry;
			rshift(b, k);
			*irv = STRTOG_Denormal;
			if(carry)
			{
				b = increment(b);
				inex = STRTOG_Inexhi | STRTOG_Underflow;
			}
			else if(lostbits)
			{
				inex = STRTOG_Inexlo | STRTOG_Underflow;
			}
		}
	}
	else if(e > fpi->emax)
	{
		e = fpi->emax + 1;
		*irv = STRTOG_Infinite | STRTOG_Overflow | STRTOG_Inexhi;
#ifndef NO_ERRNO
		errno = ERANGE;
#endif
		b->wds = 0;
		inex = 0;
	}
	*exp = e;
	copybits(bits, nb, b);
	*irv |= (int)inex;
	rv = 1;
ret:
	Bfree(b);
	return rv;
}

static int mantbits(double d)
{
	uint32_t L;
#ifdef VAX
	L = word1(d) << 16 | word1(d) >> 16;
	if(L)
#else
	if((L = word1(d)) != 0)
#endif
	{
		return P - lo0bits(&L);
	}
#ifdef VAX
	L = word0(d) << 16 | word0(d) >> 16 | Exp_msk11;
#else
	L = word0(d) | Exp_msk1;
#endif
	return P - 32 - lo0bits(&L);
}

int strtodg(const char* s00, char** se, FPI* fpi, int32_t* exp, uint32_t* bits)
{
	int abe;
	int abits;
	int asub;
	int bb0;
	int bb2;
	int bb5;
	int bbe;
	int bd2;
	int bd5;
	int bbbits;
	int bs2;
	int c;
	int denorm;
	int __attribute__((unused)) decpt; // maybe unused if not compiling with IFNAN checks
	int dsign;
	int e;
	int e1;
	int e2;
	int emin;
	int esign;
	int finished;
	int i;
	int inex;
	int irv;
	int j;
	int k;
	int nbits;
	int nd;
	int nd0;
	int nf;
	int nz;
	int nz0;
	int rd;
	int rvbits;
	int rve;
	int rve1;
	int sign;
	int sudden_underflow = 0;
	const char* s;
	const char* s0;
	const char* s1;
	double adj;
	double adj0;
	double rv;
	double tol;
	int32_t L;
	uint32_t y;
	uint32_t z;
	Bigint* ab;
	Bigint* bb;
	Bigint* bb1;
	Bigint* bd;
	Bigint* bd0;
	Bigint* bs;
	Bigint* delta;
	Bigint* rvb;
	Bigint* rvb0;

	irv = STRTOG_Zero;
	denorm = sign = nz0 = nz = 0;
	dval(rv) = 0.;
	rvb = 0;
	nbits = fpi->nbits;
	for(s = s00;; s++)
	{
		{
			switch(*s)
			{
				case '-':
					sign = 1;
					/* fall through */
				case '+':
					if(*++s)
					{
						goto break2;
					}
					/* fall through */
				case 0:
					sign = 0;
					irv = STRTOG_NoNumber;
					s = s00;
					goto ret;
				case '\t':
				case '\n':
				case '\v':
				case '\f':
				case '\r':
				case ' ':
					continue;
				default:
					goto break2;
			}
		}
	}
break2:
	if(*s == '0')
	{
#ifndef NO_HEX_FP
		switch(s[1])
		{
			case 'x':
			case 'X':
				irv = gethex(&s, fpi, exp, &rvb, sign);
				if(irv == STRTOG_NoNumber)
				{
					s = s00;
					sign = 0;
				}
				goto ret;
			default:
				break;
		}
#endif
		nz0 = 1;
		while(*++s == '0')
		{
			;
		}
		if(!*s)
		{
			goto ret;
		}
	}
	sudden_underflow = fpi->sudden_underflow;
	s0 = s;
	y = z = 0;
	for(decpt = nd = nf = 0; (c = *s) >= '0' && c <= '9'; nd++, s++)
	{
		{
			if(nd < 9)
			{
				y = 10 * y + (unsigned)c - '0';
			}
			else if(nd < 16)
			{
				z = 10 * z + (unsigned)c - '0';
			}
		}
	}
	nd0 = nd;
#ifdef USE_LOCALE
	if(c == *localeconv()->decimal_point)
#else
	if(c == '.')
#endif
	{
		decpt = 1;
		c = *++s;
		if(!nd)
		{
			for(; c == '0'; c = *++s)
			{
				nz++;
			}
			if(c > '0' && c <= '9')
			{
				s0 = s;
				nf += nz;
				nz = 0;
				goto have_dig;
			}
			goto dig_done;
		}
		for(; c >= '0' && c <= '9'; c = *++s)
		{
		have_dig:
			nz++;
			if(c -= '0')
			{
				nf += nz;
				for(i = 1; i < nz; i++)
				{
					if(nd++ < 9)
					{
						y *= 10;
					}
					else if(nd <= DBL_DIG + 1)
					{
						z *= 10;
					}
				}
				if(nd++ < 9)
				{
					y = 10 * y + (unsigned)c;
				}
				else if(nd <= DBL_DIG + 1)
				{
					z = 10 * z + (unsigned)c;
				}
				nz = 0;
			}
		}
	}
dig_done:
	e = 0;
	if(c == 'e' || c == 'E')
	{
		if(!nd && !nz && !nz0)
		{
			irv = STRTOG_NoNumber;
			s = s00;
			goto ret;
		}
		s00 = s;
		esign = 0;
		switch(c = *++s)
		{
			case '-':
				esign = 1;
				/* fall through */
			case '+':
				c = *++s;
			default:
				break;
		}
		if(c >= '0' && c <= '9')
		{
			while(c == '0')
			{
				c = *++s;
			}
			if(c > '0' && c <= '9')
			{
				L = c - '0';
				s1 = s;
				while((c = *++s) >= '0' && c <= '9')
				{
					L = 10 * L + c - '0';
				}
				if(s - s1 > 8 || L > 19999)
				{
					/* Avoid confusion from exponents
					 * so large that e might overflow.
					 */
					e = 19999; /* safe for 16 bit ints */
				}
				else
				{
					e = (int)L;
				}
				if(esign)
				{
					e = -e;
				}
			}
			else
			{
				e = 0;
			}
		}
		else
		{
			s = s00;
		}
	}
	if(!nd)
	{
		if(!nz && !nz0)
		{
#ifdef INFNAN_CHECK
			/* Check for Nan and Infinity */
			if(!decpt)
				switch(c)
				{
					case 'i':
					case 'I':
						if(match(&s, "nf"))
						{
							--s;
							if(!match(&s, "inity"))
							{
								++s;
							}
							irv = STRTOG_Infinite;
							goto infnanexp;
						}
						break;
					case 'n':
					case 'N':
						if(match(&s, "an"))
						{
							irv = STRTOG_NaN;
							*exp = fpi->emax + 1;
	#ifndef No_Hex_NaN
							if(*s == '(') /*)*/
							{
								irv = hexnan(&s, fpi, bits);
							}
	#endif
							goto infnanexp;
						}
				}
#endif /* INFNAN_CHECK */
			irv = STRTOG_NoNumber;
			s = s00;
		}
		goto ret;
	}

	irv = STRTOG_Normal;
	e1 = e -= nf;
	rd = 0;
	switch(fpi->rounding & 3)
	{
		case FPI_Round_up:
			rd = 2 - sign;
			break;
		case FPI_Round_zero:
			rd = 1;
			break;
		case FPI_Round_down:
			rd = 1 + sign;
			break;
		default:
			break;
	}

	/* Now we have nd0 digits, starting at s0, followed by a
	 * decimal point, followed by nd-nd0 digits.  The number we're
	 * after is the integer represented by those digits times
	 * 10**e */

	if(!nd0)
	{
		nd0 = nd;
	}
	k = nd < DBL_DIG + 1 ? nd : DBL_DIG + 1;
	dval(rv) = y;
	if(k > 9)
	{
		dval(rv) = tens[k - 9] * dval(rv) + z;
	}
	bd0 = 0;
	if(nbits <= P && nd <= DBL_DIG)
	{
		if(!e)
		{
			if(rvOK(dval(rv), fpi, exp, bits, 1, rd, &irv))
			{
				goto ret;
			}
		}
		else if(e > 0)
		{
			if(e <= Ten_pmax)
			{
#ifdef VAX
				goto vax_ovfl_check;
#else
				i = fivesbits[e] + mantbits(dval(rv)) <= P;
				/* rv = */ rounded_product(dval(rv), tens[e]);
				if(rvOK(dval(rv), fpi, exp, bits, i, rd, &irv))
				{
					goto ret;
				}
				e1 -= e;
				goto rv_notOK;
#endif
			}
			i = DBL_DIG - nd;
			if(e <= Ten_pmax + i)
			{
				/* A fancier test would sometimes let us do
				 * this for larger i values.
				 */
				e2 = e - i;
				e1 -= i;
				dval(rv) *= tens[i];
#ifdef VAX
				/* VAX exponent range is so narrow we must
				 * worry about overflow here...
				 */
			vax_ovfl_check:
				dval(adj) = dval(rv);
				word0(adj) -= P * Exp_msk1;
				/* adj = */ rounded_product(dval(adj), tens[e2]);
				if((word0(adj) & Exp_mask) > Exp_msk1 * (DBL_MAX_EXP + Bias - 1 - P))
					goto rv_notOK;
				word0(adj) += P * Exp_msk1;
				dval(rv) = dval(adj);
#else
				/* rv = */ rounded_product(dval(rv), tens[e2]);
#endif
				if(rvOK(dval(rv), fpi, exp, bits, 0, rd, &irv))
				{
					goto ret;
				}
				e1 -= e2;
			}
		}
#ifndef Inaccurate_Divide
		else if(e >= -Ten_pmax)
		{
			/* rv = */ rounded_quotient(dval(rv), tens[-e]);
			if(rvOK(dval(rv), fpi, exp, bits, 0, rd, &irv))
			{
				goto ret;
			}
			e1 -= e;
		}
#endif
	}
rv_notOK:
	e1 += nd - k;

	/* Get starting approximation = rv * 10**e1 */

	e2 = 0;
	if(e1 > 0)
	{
		if((i = e1 & 15) != 0)
		{
			dval(rv) *= tens[i];
		}
		if(e1 &= ~15)
		{
			e1 >>= 4;
			while(e1 >= (1 << (n_bigtens - 1)))
			{
				e2 += (int)(((word0(rv) & Exp_mask) >> Exp_shift1) - Bias);
				word0(rv) &= (unsigned)~Exp_mask;
				word0(rv) |= Bias << Exp_shift1;
				dval(rv) *= bigtens[n_bigtens - 1];
				e1 -= 1 << (n_bigtens - 1);
			}
			e2 += (int)(((word0(rv) & Exp_mask) >> Exp_shift1) - Bias);
			word0(rv) &= (unsigned)~Exp_mask;
			word0(rv) |= Bias << Exp_shift1;
			for(j = 0; e1 > 0; j++, e1 >>= 1)
			{
				if(e1 & 1)
				{
					dval(rv) *= bigtens[j];
				}
			}
		}
	}
	else if(e1 < 0)
	{
		e1 = -e1;
		if((i = e1 & 15) != 0)
		{
			dval(rv) /= tens[i];
		}
		if(e1 &= ~15)
		{
			e1 >>= 4;
			while(e1 >= (1 << (n_bigtens - 1)))
			{
				e2 += (int)(((word0(rv) & Exp_mask) >> Exp_shift1) - Bias);
				word0(rv) &= (unsigned)~Exp_mask;
				word0(rv) |= Bias << Exp_shift1;
				dval(rv) *= tinytens[n_bigtens - 1];
				e1 -= 1 << (n_bigtens - 1);
			}
			e2 += (int)(((word0(rv) & Exp_mask) >> Exp_shift1) - Bias);
			word0(rv) &= (unsigned)~Exp_mask;
			word0(rv) |= Bias << Exp_shift1;
			for(j = 0; e1 > 0; j++, e1 >>= 1)
			{
				if(e1 & 1)
				{
					dval(rv) *= tinytens[j];
				}
			}
		}
	}
#ifdef IBM
	/* e2 is a correction to the (base 2) exponent of the return
	 * value, reflecting adjustments above to avoid overflow in the
	 * native arithmetic.  For native IBM (base 16) arithmetic, we
	 * must multiply e2 by 4 to change from base 16 to 2.
	 */
	e2 <<= 2;
#endif
	rvb = d2b(dval(rv), &rve, &rvbits); /* rv = rvb * 2^rve */
	rve += e2;
	if((j = rvbits - nbits) > 0)
	{
		rshift(rvb, j);
		rvbits = nbits;
		rve += j;
	}
	bb0 = 0; /* trailing zero bits in rvb */
	e2 = rve + rvbits - nbits;
	if(e2 > fpi->emax + 1)
	{
		goto huge;
	}
	rve1 = rve + rvbits - nbits;
	if(e2 < (emin = fpi->emin))
	{
		denorm = 1;
		j = rve - emin;
		if(j > 0)
		{
			rvb = lshift(rvb, j);
			rvbits += j;
		}
		else if(j < 0)
		{
			rvbits += j;
			if(rvbits <= 0)
			{
				if(rvbits < -1)
				{
				ufl:
					rvb->wds = 0;
					rvb->x[0] = 0;
					*exp = emin;
					irv = STRTOG_Underflow | STRTOG_Inexlo;
					goto ret;
				}
				rvb->x[0] = 1;
				rvb->wds = 1;
				rvbits = 1;
			}
			else
			{
				rshift(rvb, -j);
			}
		}
		rve = rve1 = emin;
		if(sudden_underflow && e2 + 1 < emin)
		{
			goto ufl;
		}
	}

	/* Now the hard part -- adjusting rv to the correct value.*/

	/* Put digits into bd: true value = bd * 10^e */

	bd0 = s2b(s0, nd0, nd, y);

	for(;;)
	{
		bd = Balloc(bd0->k);
		Bcopy(bd, bd0);
		bb = Balloc(rvb->k);
		Bcopy(bb, rvb);
		bbbits = rvbits - bb0;
		bbe = rve + bb0;
		bs = i2b(1);

		if(e >= 0)
		{
			bb2 = bb5 = 0;
			bd2 = bd5 = e;
		}
		else
		{
			bb2 = bb5 = -e;
			bd2 = bd5 = 0;
		}
		if(bbe >= 0)
		{
			bb2 += bbe;
		}
		else
		{
			bd2 -= bbe;
		}
		bs2 = bb2;
		j = nbits + 1 - bbbits;
		i = bbe + bbbits - nbits;
		if(i < emin)
		{
			/* denormal */
			j += i - emin;
		}
		bb2 += j;
		bd2 += j;
		i = bb2 < bd2 ? bb2 : bd2;
		if(i > bs2)
		{
			i = bs2;
		}
		if(i > 0)
		{
			bb2 -= i;
			bd2 -= i;
			bs2 -= i;
		}
		if(bb5 > 0)
		{
			bs = pow5mult(bs, bb5);
			bb1 = mult(bs, bb);
			Bfree(bb);
			bb = bb1;
		}
		bb2 -= bb0;
		if(bb2 > 0)
		{
			bb = lshift(bb, bb2);
		}
		else if(bb2 < 0)
		{
			rshift(bb, -bb2);
		}
		if(bd5 > 0)
		{
			bd = pow5mult(bd, bd5);
		}
		if(bd2 > 0)
		{
			bd = lshift(bd, bd2);
		}
		if(bs2 > 0)
		{
			bs = lshift(bs, bs2);
		}
		asub = 1;
		inex = STRTOG_Inexhi;
		delta = diff(bb, bd);
		if(delta->wds <= 1 && !delta->x[0])
		{
			break;
		}
		dsign = delta->sign;
		delta->sign = finished = 0;
		L = 0;
		i = cmp(delta, bs);
		if(rd && i <= 0)
		{
			irv = STRTOG_Normal;
			if((finished = dsign ^ (rd & 1)) != 0)
			{
				if(dsign != 0)
				{
					irv |= STRTOG_Inexhi;
					goto adj1;
				}
				irv |= STRTOG_Inexlo;
				if(rve1 == emin)
				{
					goto adj1;
				}
				for(i = 0, j = nbits; j >= ULbits; i++, j -= ULbits)
				{
					if(rvb->x[i] & ALL_ON)
					{
						goto adj1;
					}
				}
				if(j > 1 && lo0bits(rvb->x + i) < j - 1)
				{
					goto adj1;
				}
				rve = rve1 - 1;
				rvb = set_ones(rvb, rvbits = nbits);
				break;
			}
			irv |= dsign ? STRTOG_Inexlo : STRTOG_Inexhi;
			break;
		}
		if(i < 0)
		{
			/* Error is less than half an ulp -- check for
			 * special case of mantissa a power of two.
			 */
			irv = dsign ? STRTOG_Normal | STRTOG_Inexlo : STRTOG_Normal | STRTOG_Inexhi;
			if(dsign || bbbits > 1 || denorm || rve1 == emin)
			{
				break;
			}
			delta = lshift(delta, 1);
			if(cmp(delta, bs) > 0)
			{
				irv = STRTOG_Normal | STRTOG_Inexlo;
				goto drop_down;
			}
			break;
		}
		if(i == 0)
		{
			/* exactly half-way between */
			if(dsign)
			{
				if(denorm && all_on(rvb, rvbits))
				{
					/*boundary case -- increment exponent*/
					rvb->wds = 1;
					rvb->x[0] = 1;
					rve = emin + nbits - (rvbits = 1);
					irv = STRTOG_Normal | STRTOG_Inexhi;
					denorm = 0;
					break;
				}
				irv = STRTOG_Normal | STRTOG_Inexlo;
			}
			else if(bbbits == 1)
			{
				irv = STRTOG_Normal;
			drop_down:
				/* boundary case -- decrement exponent */
				if(rve1 == emin)
				{
					irv = STRTOG_Normal | STRTOG_Inexhi;
					if(rvb->wds == 1 && rvb->x[0] == 1)
					{
						sudden_underflow = 1;
					}
					break;
				}
				rve -= nbits;
				rvb = set_ones(rvb, rvbits = nbits);
				break;
			}
			else
			{
				irv = STRTOG_Normal | STRTOG_Inexhi;
			}
			if(((bbbits < nbits) && !denorm) || !(rvb->x[0] & 1))
			{
				break;
			}
			if(dsign)
			{
				rvb = increment(rvb);
				j = kmask & (ULbits - (rvbits & kmask));
				if(hi0bits(rvb->x[rvb->wds - 1]) != j)
				{
					rvbits++;
				}
				irv = STRTOG_Normal | STRTOG_Inexhi;
			}
			else
			{
				if(bbbits == 1)
				{
					goto undfl;
				}
				decrement(rvb);
				irv = STRTOG_Normal | STRTOG_Inexlo;
			}
			break;
		}
		if((dval(adj) = ratio(delta, bs)) <= 2.)
		{
		adj1:
			inex = STRTOG_Inexlo;
			if(dsign)
			{
				asub = 0;
				inex = STRTOG_Inexhi;
			}
			else if(denorm && bbbits <= 1)
			{
			undfl:
				rvb->wds = 0;
				rve = emin;
				irv = STRTOG_Underflow | STRTOG_Inexlo;
				break;
			}
			adj0 = dval(adj) = 1.;
		}
		else
		{
			adj0 = dval(adj) *= 0.5;
			if(dsign)
			{
				asub = 0;
				inex = STRTOG_Inexlo;
			}
			if(dval(adj) < 2147483647.)
			{
				L = (int)adj0;
				adj0 -= L;
				switch(rd)
				{
					case 0:
						if(adj0 >= .5)
						{
							goto inc_L;
						}
						break;
					case 1:
						if(asub && adj0 > 0.)
						{
							goto inc_L;
						}
						break;
					case 2:
						if(!asub && adj0 > 0.)
						{
						inc_L:
							L++;
							inex = STRTOG_Inexact - inex;
						}
						break;
					default:
						break;
				}
				dval(adj) = L;
			}
		}
		y = (uint32_t)(rve + rvbits);

		/* adj *= ulp(dval(rv)); */
		/* if (asub) rv -= adj; else rv += adj; */

		if(!denorm && rvbits < nbits)
		{
			rvb = lshift(rvb, j = nbits - rvbits);
			rve -= j;
			rvbits = nbits;
		}
		ab = d2b(dval(adj), &abe, &abits);
		if(abe < 0)
		{
			rshift(ab, -abe);
		}
		else if(abe > 0)
		{
			ab = lshift(ab, abe);
		}
		rvb0 = rvb;
		if(asub)
		{
			/* rv -= adj; */
			j = hi0bits(rvb->x[rvb->wds - 1]);
			rvb = diff(rvb, ab);
			k = rvb0->wds - 1;
			if(denorm)
			{
				/* do nothing */;
			}
			else if(rvb->wds <= k || hi0bits(rvb->x[k]) > hi0bits(rvb0->x[k]))
			{
				/* unlikely; can only have lost 1 high bit */
				if(rve1 == emin)
				{
					--rvbits;
					denorm = 1;
				}
				else
				{
					rvb = lshift(rvb, 1);
					--rve;
					--rve1;
					L = finished = 0;
				}
			}
		}
		else
		{
			rvb = sum(rvb, ab);
			k = rvb->wds - 1;
			if(k >= rvb0->wds || hi0bits(rvb->x[k]) < hi0bits(rvb0->x[k]))
			{
				if(denorm)
				{
					if(++rvbits == nbits)
					{
						denorm = 0;
					}
				}
				else
				{
					rshift(rvb, 1);
					rve++;
					rve1++;
					L = 0;
				}
			}
		}
		Bfree(ab);
		Bfree(rvb0);
		if(finished)
		{
			break;
		}

		z = (uint32_t)(rve + rvbits);
		if(y == z && L)
		{
			/* Can we stop now? */
			tol = dval(adj) * 5e-16; /* > max rel error */
			dval(adj) = adj0 - .5;
			if(dval(adj) < -tol)
			{
				if(adj0 > tol)
				{
					irv |= inex;
					break;
				}
			}
			else if(dval(adj) > tol && adj0 < 1. - tol)
			{
				irv |= inex;
				break;
			}
		}
		bb0 = denorm ? 0 : trailz(rvb);
		Bfree(bb);
		Bfree(bd);
		Bfree(bs);
		Bfree(delta);
	}
	if(!denorm && (j = nbits - rvbits))
	{
		if(j > 0)
		{
			rvb = lshift(rvb, j);
		}
		else
		{
			rshift(rvb, -j);
		}
		rve -= j;
	}
	*exp = rve;
	Bfree(bb);
	Bfree(bd);
	Bfree(bs);
	Bfree(bd0);
	Bfree(delta);
	if(rve > fpi->emax)
	{
	huge:
		rvb->wds = 0;
		irv = STRTOG_Infinite | STRTOG_Overflow | STRTOG_Inexhi;
#ifndef NO_ERRNO
		errno = ERANGE;
#endif
#ifdef INFNAN_CHECK
	infnanexp:
		*exp = fpi->emax + 1;
#endif
	}
ret:
	if(denorm)
	{
		if(sudden_underflow)
		{
			rvb->wds = 0;
			irv = STRTOG_Underflow | STRTOG_Inexlo;
		}
		else
		{
			irv = (irv & ~STRTOG_Retmask) | (rvb->wds > 0 ? STRTOG_Denormal : STRTOG_Zero);
			if(irv & STRTOG_Inexact)
			{
				irv |= STRTOG_Underflow;
			}
		}
	}
	if(se)
	{
		*se = (char*)(uintptr_t)s;
	}
	if(sign)
	{
		irv |= STRTOG_Neg;
	}
	if(rvb)
	{
		copybits(bits, nbits, rvb);
		Bfree(rvb);
	}
	return irv;
}
