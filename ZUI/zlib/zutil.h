
#ifndef ZUTIL_H
#define ZUTIL_H

#define ZLIB_INTERNAL

#include "zlib.h"

#if defined(STDC) && !defined(Z_SOLO)
#  if !(defined(_WIN32_WCE) && defined(_MSC_VER))
#    include <stddef.h>
#  endif
#  include <string.h>
#  include <stdlib.h>
#endif


#ifndef local
#  define local static
#endif

typedef unsigned char  uch;
typedef uch uchf;
typedef unsigned short ush;
typedef ush ushf;
typedef unsigned long  ulg;



#define zmemcpy memcpy

voidpf ZLIB_INTERNAL zcalloc OF((voidpf opaque, unsigned items, unsigned size));
void ZLIB_INTERNAL zcfree  OF((voidpf opaque, voidpf ptr));

#define ZALLOC(strm, items, size) \
           (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr)  (*((strm)->zfree))((strm)->opaque, (voidpf)(addr))

#define ZSWAP32(q) ((((q) >> 24) & 0xff) + (((q) >> 8) & 0xff00) + \
                    (((q) & 0xff00) << 8) + (((q) & 0xff) << 24))

#endif /* ZUTIL_H */
