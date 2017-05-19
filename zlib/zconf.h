#ifndef ZCONF_H
#define ZCONF_H

#  define WINDOWS
#  define STDC
#  define z_const
#  define MAX_MEM_LEVEL 9

#  define MAX_WBITS   15 /* 32K LZ77 window */

#    define OF(args)  args
#    define Z_ARG(args)  args

#  define ZEXTERN extern
#  define ZEXPORT
#  define ZEXPORTVA

typedef unsigned char  Byte;  /* 8 bits */
typedef unsigned int   uInt;  /* 16 bits or more */
typedef unsigned long  uLong; /* 32 bits or more */


typedef Byte  Bytef;
typedef char  charf;
typedef int   intf;
typedef uInt  uIntf;
typedef uLong uLongf;

typedef void const *voidpc;
typedef void   *voidpf;
typedef void       *voidp;

#include <limits.h>
#define Z_U4 unsigned

typedef Z_U4 z_crc_t;

#    include <sys/types.h>      /* for off_t */
#    include <stdarg.h>         /* for va_list */
#    include <stddef.h>         /* for wchar_t */


#  define z_off_t long
#  define z_off64_t long long




#endif /* ZCONF_H */
