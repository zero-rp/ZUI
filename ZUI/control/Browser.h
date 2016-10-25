#ifndef __BROWSER_H__
#define __BROWSER_H__
#include "wke.h"

#define	Proc_Browser_LoadUrl	1001
/**游览器控件结构*/
typedef struct _ZuiBrowser
{
	ZuiText url;///当前URL
	wkeWebView view;
	ZuiControl cp;
	ZuiBool init;
	ZCtlProc old_call;
}*ZuiBrowser, ZBrowser;
ZEXPORT ZuiAny ZCALL ZuiBrowserProc(ZuiInt ProcId, ZuiControl cp, ZuiBrowser p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__BROWSER_H__
