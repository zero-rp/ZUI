/**
* @file     Browser.h
* @brief    游览器控件实现.
* @author   [Zero](22249030)
* @version  1.0
* @date     $date
* @par History:
*
* [2016-10-25] <Zero> v1.0
*
* + v1.0版发布.
*
*/
#ifndef __BROWSER_H__
#define __BROWSER_H__
#define Type_Browser                0xefef127a

#define Proc_Browser_LoadUrl        1001
#define Proc_Browser_LoadHtml       1002
#define Proc_Browser_LoadFile       1003
#define Proc_Browser_GetView        1004
#define Proc_Browser_RunJs          1005
#define Proc_Browser_jsToString     1006

//------Js

#define Js_Id_Browser_titlechanged    100
#define Js_Id_Browser_newwindow       101
#define Js_Id_Browser_urlchanged    102
#define Js_Id_Browser_navigation   103
#define Js_Id_Browser_url   104

#define ZuiBrowserLoadUrl(Control, ...) ZProc(Control, Browser_LoadUrl, __VA_ARGS__);

/**游览器控件结构*/
typedef struct _ZuiBrowser
{
    ZuiInt titlechanged;
    ZuiInt newwindow;
    ZuiInt urlchanged;
    ZuiInt navigation;

    ZuiText url;///当前URL
    ZuiAny view;
    ZuiControl cp;
    ZuiBool init;
    ZCtlProc old_call;
}*ZuiBrowser, ZBrowser;
ZEXPORT ZuiAny ZCALL ZuiBrowserProc(ZuiInt ProcId, ZuiControl cp, ZuiBrowser p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__BROWSER_H__
