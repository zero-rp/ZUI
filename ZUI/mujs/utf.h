#ifndef js_utf_h
#define js_utf_h
#include <wchar.h>

typedef unsigned short Rune;	/* 16 bits */

#define chartorune	jsU_chartorune
#define runetochar	jsU_runetochar
#define runelen		wcslen
#define utflen		wcslen

#define isalpharune	jsU_isalpharune
#define islowerrune	jsU_islowerrune
#define isspacerune	jsU_isspacerune
#define istitlerune	jsU_istitlerune
#define isupperrune	jsU_isupperrune
#define tolowerrune	jsU_tolowerrune
#define totitlerune	jsU_totitlerune
#define toupperrune	jsU_toupperrune

enum
{
	UTFmax		= 3,		/* maximum bytes per rune */
	Runesync	= 0x80,		/* cannot represent part of a UTF sequence (<) */
	Runeself	= 0x80,		/* rune and UTF sequences are the same (<) */
	Runeerror	= 0xFFFD,	/* decoding error in UTF */
};

int	chartorune(Rune *rune, const wchar_t *str);
int	runetochar(char *str, const Rune *rune);
size_t	runelen(int c);
size_t	utflen(const wchar_t *s);

int		isalpharune(Rune c);
int		islowerrune(Rune c);
int		isspacerune(Rune c);
int		istitlerune(Rune c);
int		isupperrune(Rune c);
Rune		tolowerrune(Rune c);
Rune		totitlerune(Rune c);
Rune		toupperrune(Rune c);

#endif
