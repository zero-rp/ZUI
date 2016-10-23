#ifndef __BROWSER_H__
#define __BROWSER_H__
#include "wke.h"
typedef struct _ZuiBrowser
{
	wkeWebView view;
	int type;
	ZCtlProc old_call;
}*ZuiBrowser, ZBrowser;
ZAPI(ZuiAny) ZuiBrowserProc(ZuiInt ProcId, ZuiControl cp, ZuiBrowser p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__BROWSER_H__
