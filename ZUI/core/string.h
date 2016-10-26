/**
* @file		string.h
* @brief	字符串相关函数.
* @author	[Zero](22249030)
* @version	1.0
* @date		$date
* @par History:
*
* [2016-10-25] <Zero> v1.0
*
* + v1.0版发布.
*
*/
#ifndef __STRING_H__
#define __STRING_H__

ZEXPORT ZuiBool ZCALL ZuiStingIsUtf8(ZuiAny str, ZuiInt length);
ZEXPORT ZuiInt ZCALL ZuiUtf8ToUnicode(ZuiAny str, ZuiInt slen, ZuiText out, ZuiInt olen);
ZEXPORT ZuiInt ZCALL ZuiAsciiToUnicode(ZuiAny str, ZuiInt slen, ZuiText out, ZuiInt olen);
ZEXPORT ZuiInt ZCALL ZuiUnicodeToAscii(ZuiText str, ZuiInt slen, ZuiAny out, ZuiInt olen);
#endif