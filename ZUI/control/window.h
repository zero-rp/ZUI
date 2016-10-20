#ifndef __UIWINDOW_H__
#define __UIWINDOW_H__

typedef struct _ZWindow
{
	HWND m_hWnd;
	WNDPROC m_OldWndProc;
	BOOL m_bSubclassed;
	ZuiControl root;
	ZuiPaintManager m_pm;
	ZCtlProc old_call;
	void *old_udata;
}*ZuiWindow, ZWindow;
ZAPI(ZuiAny) ZuiWindowProc(ZuiInt ProcId, ZuiControl cp, ZuiWindow p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__UIWINDOW_H__