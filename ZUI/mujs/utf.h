#ifndef js_utf_h
#define js_utf_h
#include <wchar.h>

typedef wchar_t Rune; /* 16 bits */


#define runelen wcslen
#define utflen wcslen

enum
{
    Runesync = 0x80, /* cannot represent part of a UTF sequence (<) */
    Runeself = 0x80, /* rune and UTF sequences are the same (<) */
    Runeerror = 0xFFFD, /* decoding error in UTF */
};
#define isalpharune iswalpha
#define islowerrune iswlower
#define isspacerune iswspace
#define isupperrune iswupper
#define tolowerrune towlower
#define totitlerune towtitle
#define toupperrune towupper
#define istitlerune(c) (isupperrune((c)) && islowerrune((c)))

#endif
