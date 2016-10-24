#ifndef __STRING_H__
#define __STRING_H__

ZAPI(ZuiBool) ZuiStingIsUtf8(ZuiAny str, ZuiInt length);
ZAPI(ZuiInt) ZuiUtf8ToUnicode(ZuiAny str, ZuiInt slen, ZuiText out, ZuiInt olen);
ZAPI(ZuiInt) ZuiAsciiToUnicode(ZuiAny str, ZuiInt slen, ZuiText out, ZuiInt olen);

#endif