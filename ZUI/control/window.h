#ifndef __UIWINDOW_H__
#define __UIWINDOW_H__
#include <ZUI.h>
#include <platform/platform.h>
//------JSID
#define Js_Id_Window_SetWindowMin    100
#define Js_Id_Window_SetWindowMax    102
#define Js_Id_Window_SetWindowRestor 103
#define Js_Id_Window_Popup           104


/**窗口控件结构*/
typedef struct _ZWindow
{    
    ZuiOsWindow m_osWindow;    
    
    ZCtlProc old_call;
    ZuiAny old_udata;
}*ZuiWindow, ZWindow;
ZEXPORT ZuiAny ZCALL ZuiWindowProc(ZuiInt ProcId, ZuiControl cp, ZuiWindow p, ZuiAny Param1, ZuiAny Param2);
//查找窗口
ZEXPORT ZuiControl ZCALL ZuiWindowFindName(ZuiText Name);

#endif	//__UIWINDOW_H__