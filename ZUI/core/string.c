#include <ZUI.h>

ZEXPORT ZuiBool ZCALL ZuiStingIsUtf8(ZuiAny str, ZuiInt length)
{
	int i;
	//UFT8可用1-6个字节编码,ASCII用一个字节
	int nBytes = 0;
	unsigned char chr;
	//如果全部都是ASCII, 说明不是UTF-8
	ZuiBool bAllAscii = TRUE;
	for (i = 0; i<length; i++)
	{
		chr = *((char *)str + i);
		// 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
		if ((chr & 0x80) != 0)
		{
			bAllAscii = FALSE;
		}
		//如果不是ASCII码,应该是多字节符,计算字节数
		if (nBytes == 0)
		{
			if (chr >= 0x80)
			{
				if (chr >= 0xFC && chr <= 0xFD)
				{
					nBytes = 6;
				}
				else if (chr >= 0xF8)
				{
					nBytes = 5;
				}
				else if (chr >= 0xF0)
				{
					nBytes = 4;
				}
				else if (chr >= 0xE0)
				{
					nBytes = 3;
				}
				else if (chr >= 0xC0)
				{
					nBytes = 2;
				}
				else
				{
					return FALSE;
				}
				nBytes--;
			}
		}
		//多字节符的非首字节,应为 10xxxxxx
		else
		{
			if ((chr & 0xC0) != 0x80)
			{
				return FALSE;
			}
			nBytes--;
		}
	}
	//违返规则
	if (nBytes > 0)
	{
		return FALSE;
	}
	//如果全部都是ASCII, 也是字符串
	if (bAllAscii)
	{
		return TRUE;
	}
	return TRUE;
}


ZEXPORT ZuiInt ZCALL ZuiUtf8ToUnicode(ZuiAny str, ZuiInt slen, ZuiText out, ZuiInt olen)
{
	return MultiByteToWideChar(CP_UTF8, 0, str, slen, out, olen);
}

ZEXPORT ZuiInt ZCALL ZuiAsciiToUnicode(ZuiAny str, ZuiInt slen, ZuiText out, ZuiInt olen)
{
	return MultiByteToWideChar(CP_ACP, 0, str, slen, out, olen);
}

