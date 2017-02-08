/**
* @file		html.h
* @brief	游览器控件实现.
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
#ifndef __HTML_H__
#define __HTML_H__


/**游览器控件结构*/
typedef struct _ZuiHtml
{
	ZuiControl cp;
	ZuiBool init;
	ZCtlProc old_call;
}*ZuiHtml, ZHtml;
ZEXPORT ZuiAny ZCALL ZuiHtmlProc(ZuiInt ProcId, ZuiControl cp, ZuiHtml p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__BROWSER_H__
