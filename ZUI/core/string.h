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
/** 此函数用作分割文本.
* @param src 将被分割的字符串,需要内存可写
* @param pSeparator 用作分割的文本
* @param dest 目标文本数组
* @param num 目标数组大小
* @return 此函数没有返回值.
*/
ZEXPORT ZuiVoid ZCALL ZuiStingSplit(ZuiText src, ZuiText pSeparator, ZuiText *dest, ZuiInt *num);
ZEXPORT ZuiBool ZCALL ZuiStingIsUtf8(ZuiAny str, ZuiInt length);
ZEXPORT ZuiInt ZCALL ZuiUtf8ToUnicode(ZuiAny str, ZuiInt slen, ZuiText out, ZuiInt olen);
ZEXPORT ZuiInt ZCALL ZuiAsciiToUnicode(ZuiAny str, ZuiInt slen, ZuiText out, ZuiInt olen);
ZEXPORT ZuiInt ZCALL ZuiUnicodeToAscii(ZuiText str, ZuiInt slen, ZuiAny out, ZuiInt olen);
#endif