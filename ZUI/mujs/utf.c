/*
 * The authors of this software are Rob Pike and Ken Thompson.
 *              Copyright (c) 2002 by Lucent Technologies.
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHORS NOR LUCENT TECHNOLOGIES MAKE
 * ANY REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
 * OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
 */
#include <stdlib.h>
#include <string.h>

#include "utf.h"


int
chartorune(Rune *rune, const wchar_t *str)
{
	wchar_t c;

	/*
	 * one character sequence
	 *	00000-0007F => T1
	 */
	c = *(wchar_t*)str;
	if(c == 0xFEFF) {
		c = *(wchar_t*)(str+1);
		*rune = c;
		return 2;
	}
	*rune = c;
	return 1;

bad:
	*rune = 0xFFFD;
	return 1;
}

int
runetochar(wchar_t *str, const Rune *rune)
{
	*str = *rune;
	return 1;
}
