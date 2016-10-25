#ifndef __STRING_H__
#define __STRING_H__

ZEXPORT ZuiBool ZCALL ZuiStingIsUtf8(ZuiAny str, ZuiInt length);
ZEXPORT ZuiInt ZCALL ZuiUtf8ToUnicode(ZuiAny str, ZuiInt slen, ZuiText out, ZuiInt olen);
ZEXPORT ZuiInt ZCALL ZuiAsciiToUnicode(ZuiAny str, ZuiInt slen, ZuiText out, ZuiInt olen);
ZEXPORT ZuiInt ZCALL ZuiUnicodeToAscii(ZuiText str, ZuiInt slen, ZuiAny out, ZuiInt olen);
#endif