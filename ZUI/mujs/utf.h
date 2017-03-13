#ifndef js_utf_h
#define js_utf_h
#include <wchar.h>

typedef unsigned short Rune; /* 16 bits */


#define runelen wcslen
#define utflen wcslen

enum
{
    UTFmax = 3, /* maximum bytes per rune */
    Runesync = 0x80, /* cannot represent part of a UTF sequence (<) */
    Runeself = 0x80, /* rune and UTF sequences are the same (<) */
    Runeerror = 0xFFFD, /* decoding error in UTF */
};

int chartorune(Rune *rune, const wchar_t *str);
int runetochar(wchar_t *str, const Rune *rune);


int isalpharune(Rune c);
int islowerrune(Rune c);
int isspacerune(Rune c);
int istitlerune(Rune c);
int isupperrune(Rune c);
Rune tolowerrune(Rune c);
Rune totitlerune(Rune c);
Rune toupperrune(Rune c);

#endif
