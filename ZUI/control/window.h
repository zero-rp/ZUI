#ifndef __UIWINDOW_H__
#define __UIWINDOW_H__

#define	Proc_Window_SetNoBox	1001	//设置为无边框窗体
/**窗口控件结构*/
typedef struct _ZWindow
{
	ZRect m_rect;
	ZuiBool m_nobox;
	HWND m_hWnd;
	WNDPROC m_OldWndProc;
	ZuiControl root;
	ZuiPaintManager m_pm;
	ZCtlProc old_call;
	void *old_udata;
}*ZuiWindow, ZWindow;
ZEXPORT ZuiAny ZCALL ZuiWindowProc(ZuiInt ProcId, ZuiControl cp, ZuiWindow p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__UIWINDOW_H__