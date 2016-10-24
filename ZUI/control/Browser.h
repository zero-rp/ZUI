#ifndef __BROWSER_H__
#define __BROWSER_H__
#include "wke.h"

#define	Proc_Browser_LoadUrl	1001
typedef struct _ZuiBrowser
{
	ZuiText url;
	wkeWebView view;
	ZuiControl cp;
	ZuiBool init;
	ZCtlProc old_call;
}*ZuiBrowser, ZBrowser;
ZAPI(ZuiAny) ZuiBrowserProc(ZuiInt ProcId, ZuiControl cp, ZuiBrowser p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__BROWSER_H__
