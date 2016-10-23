#include <ZUI.h>


/////////////////////////////////////////////////////////////////////////////////////
//
//

static UINT MapKeyState()
{
	UINT uState = 0;
	if (GetKeyState(VK_CONTROL) < 0) uState |= MK_CONTROL;
	if (GetKeyState(VK_RBUTTON) < 0) uState |= MK_RBUTTON;
	if (GetKeyState(VK_LBUTTON) < 0) uState |= MK_LBUTTON;
	if (GetKeyState(VK_SHIFT) < 0) uState |= MK_SHIFT;
	if (GetKeyState(VK_MENU) < 0) uState |= MK_ALT;
	return uState;
}

typedef struct tagFINDTABINFO
{
	ZuiControl pFocus;
	ZuiControl pLast;
	BOOL bForward;
	BOOL bNextIsIt;
} FINDTABINFO;

typedef struct tagFINDSHORTCUT
{
	TCHAR ch;
	BOOL bPickNext;
} FINDSHORTCUT;
//定时器结构
typedef struct tagTIMERINFO
{
	ZuiControl pSender;	//接收时钟消息的控件
	UINT nLocalID;		//时钟ID
	HWND hWnd;			//窗口句柄
	UINT uWinTimer;		
	BOOL bKilled;
} TIMERINFO;


/////////////////////////////////////////////////////////////////////////////////////

static void GetChildWndRect(HWND hWnd, HWND hChildWnd, RECT *rcChildWnd)
{
	GetWindowRect(hChildWnd, rcChildWnd);

	POINT pt;
	pt.x = rcChildWnd->left;
	pt.y = rcChildWnd->top;
	ScreenToClient(hWnd, &pt);
	rcChildWnd->left = pt.x;
	rcChildWnd->top = pt.y;

	pt.x = rcChildWnd->right;
	pt.y = rcChildWnd->bottom;
	ScreenToClient(hWnd, &pt);
	rcChildWnd->right = pt.x;
	rcChildWnd->bottom = pt.y;
}

/////////////////////////////////////////////////////////////////////////////////////

typedef BOOL(__stdcall *PFUNCUPDATELAYEREDWINDOW)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);

PFUNCUPDATELAYEREDWINDOW g_fUpdateLayeredWindow = NULL;	//UpdateLayeredWindow函数指针

HPEN m_hUpdateRectPen = NULL;


#define LAYEREDUPDATE_TIMERID	0x2000

ZuiBool ZuiPaintManagerInitialize() {
	m_aPreMessages = darray_create();
	m_hInstance = (HINSTANCE)GetModuleHandleA(NULL);
	return TRUE;
}

//创建消息管理器
ZuiPaintManager NewCPaintManagerUI(){
	ZuiPaintManager p = (ZuiPaintManager)malloc(sizeof(IZuiPaintManager));
	if (p){
		memset(p, 0, sizeof(IZuiPaintManager));

		p->m_iTooltipWidth = -1;
		p->m_iHoverTime = 1000;
		//p->m_bShowUpdateRect = TRUE;
		p->m_uTimerID = 0x1000;
		p->m_bFirstLayout = TRUE;
		p->m_bOffscreenPaint = TRUE;
		p->m_nOpacity = 0xFF;

		if (m_hUpdateRectPen == NULL) {
			m_hUpdateRectPen = CreatePen(PS_SOLID, 1, RGB(220, 0, 0));
			// Boot Windows Common Controls (for the ToolTip control)
			InitCommonControls();
			LoadLibrary(_T("msimg32.dll"));
		}

		p->m_ptLastMousePos.x = p->m_ptLastMousePos.y = -1;


		p->m_aTimers= darray_create();
		p->m_aPostPaintControls = darray_create();
		p->m_aFoundControls = darray_create();
		return p;
	}
	return NULL;
}
//销毁消息管理器
void FreeCPaintManagerUI(ZuiPaintManager p){
	// Delete the control-tree structures
	if (p->m_pRoot != NULL) 
		FreeCControlUI(p->m_pRoot);

	ZuiPaintManagerRemoveAllTimers(p);

	// Reset other parts...
	if (p->m_hwndTooltip != NULL)
	{
		DestroyWindow(p->m_hwndTooltip);
		p->m_hwndTooltip = NULL;
	}
	if (p->m_hDcOffscreen != NULL)
		ZuiDestroyGraphics(p->m_hDcOffscreen);
	if (p->m_hDcPaint != NULL)
		ReleaseDC(p->m_hWndPaint, p->m_hDcPaint);
	darray_delete(m_aPreMessages, darray_find(m_aPreMessages,p));
}
//-------------------------------------------------------------------------------------------------

// 绘图管理器的初始化(m_hWndPaint,m_hDcPaint赋值，在预处理消息中加入管理器)
ZAPI(ZuiVoid) ZuiPaintManagerInit(ZuiPaintManager p, HWND hWnd)
{
	ZuiPaintManagerRemoveAllTimers(p);

	if (p->m_hWndPaint != hWnd) {
		p->m_hWndPaint = hWnd;
		p->m_hDcPaint = GetDC(hWnd);
		darray_append(m_aPreMessages, p);
	}
}
//指定区域失效
ZAPI(ZuiVoid) ZuiPaintManagerInvalidate(ZuiPaintManager p)
{
	if (!p->m_bLayered)
		InvalidateRect(p->m_hWndPaint, NULL, FALSE);
	else {
		RECT rcClient = { 0 };
		GetClientRect(p->m_hWndPaint, &rcClient);
		UnionRect(&p->m_rcLayeredUpdate, &p->m_rcLayeredUpdate, &rcClient);
	}
}
//指定区域失效
ZAPI(ZuiVoid) ZuiPaintManagerInvalidateRect(ZuiPaintManager p, RECT *rcItem)
{
	RECT rc;
	rc.bottom = rcItem->bottom;
	rc.left = rcItem->left;
	rc.right = rcItem->right;
	rc.top = rcItem->top;
	if (rc.left < 0) rc.left = 0;
	if (rc.top < 0) rc.top = 0;
	if (rc.right <rc.left) rc.right = rc.left;
	if (rc.bottom < rc.top) rc.bottom = rc.top;
	if (!p->m_bLayered) 
		InvalidateRect(p->m_hWndPaint, &rc, FALSE);
	else 
		UnionRect(&p->m_rcLayeredUpdate, &p->m_rcLayeredUpdate, &rc);
}
//-------------------------------------------------------------------------------------------------


//设置窗口透明度 会开启分层窗口
ZAPI(ZuiVoid) ZuiPaintManagerSetOpacity(ZuiPaintManager p, BYTE nOpacity)
{
	p->m_nOpacity = nOpacity;
	if (p->m_hWndPaint != NULL) {
		typedef BOOL(__stdcall *PFUNCSETLAYEREDWINDOWATTR)(HWND, COLORREF, BYTE, DWORD);
		PFUNCSETLAYEREDWINDOWATTR fSetLayeredWindowAttributes = 0;

		HMODULE hUser32 = GetModuleHandle(_T("User32.dll"));
		if (hUser32)
		{
			fSetLayeredWindowAttributes =
				(PFUNCSETLAYEREDWINDOWATTR)GetProcAddress(hUser32, "SetLayeredWindowAttributes");
			if (fSetLayeredWindowAttributes == NULL) return;
		}

		DWORD dwStyle = GetWindowLong(p->m_hWndPaint, GWL_EXSTYLE);
		DWORD dwNewStyle = dwStyle;
		if (nOpacity >= 0 && nOpacity < 256) dwNewStyle |= WS_EX_LAYERED;
		else dwNewStyle &= ~WS_EX_LAYERED;
		if (dwStyle != dwNewStyle) SetWindowLong(p->m_hWndPaint, GWL_EXSTYLE, dwNewStyle);
		fSetLayeredWindowAttributes(p->m_hWndPaint, 0, nOpacity, LWA_ALPHA);
	}
}
//-------------------------------------------------------------------------------------------------

//设置分层窗口
ZAPI(ZuiVoid) ZuiPaintManagerSetLayered(ZuiPaintManager p, BOOL bLayered)
{
	if (p->m_hWndPaint != NULL && bLayered != p->m_bLayered) {
		UINT uStyle = GetWindowStyle(p->m_hWndPaint);
		if ((uStyle & WS_CHILD) != 0) return;

		if (g_fUpdateLayeredWindow == NULL) {
			HMODULE hUser32 = GetModuleHandle(_T("User32.dll"));
			if (hUser32) {
				g_fUpdateLayeredWindow =
					(PFUNCUPDATELAYEREDWINDOW)GetProcAddress(hUser32, "UpdateLayeredWindow");
				if (g_fUpdateLayeredWindow == NULL) return;
			}
		}
		DWORD dwExStyle = GetWindowLong(p->m_hWndPaint, GWL_EXSTYLE);
		DWORD dwNewExStyle = dwExStyle;
		if (bLayered) {
			dwNewExStyle |= WS_EX_LAYERED;
			SetTimer(p->m_hWndPaint, LAYEREDUPDATE_TIMERID, 10L, NULL);
		}
		else {
			dwNewExStyle &= ~WS_EX_LAYERED;
			KillTimer(p->m_hWndPaint, LAYEREDUPDATE_TIMERID);
		}
		if (dwExStyle != dwNewExStyle) SetWindowLong(p->m_hWndPaint, GWL_EXSTYLE, dwNewExStyle);
		p->m_bLayered = bLayered;
		if (p->m_pRoot != NULL) ZuiControlNeedUpdate(p->m_pRoot);
		ZuiPaintManagerInvalidate(p);
	}
}


ZAPI(ZuiVoid) ZuiPaintManagerSetLayeredInset(ZuiPaintManager p, RECT *rcLayeredInset)
{
	
	p->m_rcLayeredInset.bottom = rcLayeredInset->bottom;
	p->m_rcLayeredInset.left = rcLayeredInset->left;
	p->m_rcLayeredInset.right = rcLayeredInset->right;
	p->m_rcLayeredInset.top = rcLayeredInset->top;
	p->m_bLayeredChanged = TRUE;
	ZuiPaintManagerInvalidate(p);
}

//设置分层窗口透明度
ZAPI(ZuiVoid) ZuiPaintManagerSetLayeredOpacity(ZuiPaintManager p, BYTE nOpacity)
{
	p->m_nOpacity = nOpacity;
	p->m_bLayeredChanged = TRUE;
	ZuiPaintManagerInvalidate(p);
}
//-------------------------------------------------------------------------------------------------

//附加控件树 将控件树关联到管理器
ZAPI(ZuiBool) ZuiPaintManagerAttachDialog(ZuiPaintManager p, ZuiControl pControl)
{
	ASSERT(IsWindow(p->m_hWndPaint));
	// Reset any previous attachment
	ZuiPaintManagerSetFocus(p, NULL ,TRUE);
	p->m_pEventKey = NULL;
	p->m_pEventHover = NULL;
	p->m_pEventClick = NULL;
	// Remove the existing control-tree. We might have gotten inside this function as
	// a result of an event fired or similar, so we cannot just delete the objects and
	// pull the internal memory of the calling code. We'll delay the cleanup.
	if (p->m_pRoot != NULL) {
		darray_empty(p->m_aPostPaintControls);
	}
	// Set the dialog root element
	p->m_pRoot = pControl;
	// Go ahead...
	p->m_bUpdateNeeded = TRUE;
	p->m_bFirstLayout = TRUE;
	p->m_bFocusNeeded = TRUE;
	// Initiate all control
	return ZuiPaintManagerInitControls(p, pControl, NULL);
}
//初始化控件
ZAPI(ZuiBool) ZuiPaintManagerInitControls(ZuiPaintManager p, ZuiControl pControl, ZuiControl pParent)
{
	ASSERT(pControl);
	if (pControl == NULL) return FALSE;
	ZuiControlCall(Proc_SetManager, pControl, p, pParent != NULL ? pParent : pControl->m_pParent, (void*)TRUE);
	return TRUE;
}

ZAPI(ZuiVoid) ZuiPaintManagerReapObjects(ZuiPaintManager p, ZuiControl pControl)
{
	if (pControl == NULL) return;
	if (pControl == p->m_pEventKey) p->m_pEventKey = NULL;
	if (pControl == p->m_pEventHover) p->m_pEventHover = NULL;
	if (pControl == p->m_pEventClick) p->m_pEventClick = NULL;
	if (pControl == p->m_pFocus) p->m_pFocus = NULL;
	ZuiPaintManagerKillTimer(p, pControl);
}
//-------------------------------------------------------------------------------------------------

//设置焦点控件
ZAPI(ZuiVoid) ZuiPaintManagerSetFocus(ZuiPaintManager p, ZuiControl pControl, BOOL bFocusWnd)
{
	// Paint manager window has focus?
	HWND hFocusWnd = GetFocus();
	if (bFocusWnd && hFocusWnd != p->m_hWndPaint && pControl != p->m_pFocus) SetFocus(p->m_hWndPaint);
	// Already has focus?
	if (pControl == p->m_pFocus) return;
	// Remove focus from old control
	if (p->m_pFocus != NULL)
	{
		TEventUI event = { 0 };
		event.Type = ZEVENT_KILLFOCUS;
		event.pSender = pControl;
		event.dwTimestamp = GetTickCount();
		ZuiControlEvent(p->m_pFocus, &event);
		p->m_pFocus = NULL;
	}
	if (pControl == NULL) return;
	// Set focus to new control
	if (pControl != NULL
		&& pControl->m_pManager == p
		&& pControl->m_bVisible
		&& pControl->m_bEnabled)
	{
		p->m_pFocus = pControl;
		TEventUI event = { 0 };
		event.Type = ZEVENT_SETFOCUS;
		event.pSender = pControl;
		event.dwTimestamp = GetTickCount();
		ZuiControlEvent(p->m_pFocus, &event);
	}
}

ZAPI(ZuiVoid) ZuiPaintManagerSetFocusNeeded(ZuiPaintManager p, ZuiControl pControl)
{
	SetFocus(p->m_hWndPaint);
	if (pControl == NULL) return;
	if (p->m_pFocus != NULL) {
		TEventUI event = { 0 };
		event.Type = ZEVENT_KILLFOCUS;
		event.pSender = pControl;
		event.dwTimestamp = GetTickCount();
		ZuiControlEvent(p->m_pFocus, &event);
		p->m_pFocus = NULL;
	}
	FINDTABINFO info = { 0 };
	info.pFocus = pControl;
	info.bForward = FALSE;
	p->m_pFocus = (ZuiControl)ZuiControlCall(Proc_FindControl, p->m_pRoot, __FindControlFromTab, &info, (void *)(ZFIND_VISIBLE | ZFIND_ENABLED | ZFIND_ME_FIRST));
	p->m_bFocusNeeded = TRUE;
	if (p->m_pRoot != NULL)
		ZuiControlNeedUpdate(p->m_pRoot);
}
//-------------------------------------------------------------------------------------------------

//创建时钟
ZAPI(ZuiBool) ZuiPaintManagerSetTimer(ZuiControl pControl, UINT nTimerID, UINT uElapse)
{
	ASSERT(pControl != NULL);
	ASSERT(uElapse > 0);
	if (pControl->m_pManager) {
		for (int i = 0; i < darray_len(pControl->m_pManager->m_aTimers); i++) {
			TIMERINFO* pTimer = (TIMERINFO*)(pControl->m_pManager->m_aTimers->data[i]);
			if (pTimer->pSender == pControl
				&& pTimer->hWnd == pControl->m_pManager->m_hWndPaint
				&& pTimer->nLocalID == nTimerID) {
				if (pTimer->bKilled == TRUE) {
					if (SetTimer(pControl->m_pManager->m_hWndPaint, pTimer->uWinTimer, uElapse, NULL)) {
						pTimer->bKilled = FALSE;
						return TRUE;
					}
					return FALSE;
				}
				return FALSE;
			}
		}

		pControl->m_pManager->m_uTimerID = (++pControl->m_pManager->m_uTimerID) % 0xFF;
		if (!SetTimer(pControl->m_pManager->m_hWndPaint, pControl->m_pManager->m_uTimerID, uElapse, NULL)) return FALSE;
		TIMERINFO* pTimer = malloc(sizeof(TIMERINFO));
		memset(pTimer, 0, sizeof(TIMERINFO));
		if (pTimer == NULL) return FALSE;
		pTimer->hWnd = pControl->m_pManager->m_hWndPaint;
		pTimer->pSender = pControl;
		pTimer->nLocalID = nTimerID;
		pTimer->uWinTimer = pControl->m_pManager->m_uTimerID;
		pTimer->bKilled = FALSE;
		return darray_append(pControl->m_pManager->m_aTimers, pTimer);
	}
	return FALSE;
}
//销毁时钟
ZAPI(ZuiBool) ZuiPaintManagerKillTimer_Id(ZuiControl pControl, UINT nTimerID)
{
	ASSERT(pControl != NULL);
	if (pControl->m_pManager) {
		for (int i = 0; i < darray_len(pControl->m_pManager->m_aTimers); i++) {
			TIMERINFO* pTimer = (TIMERINFO*)(pControl->m_pManager->m_aTimers->data[i]);
			if (pTimer->pSender == pControl
				&& pTimer->hWnd == pControl->m_pManager->m_hWndPaint
				&& pTimer->nLocalID == nTimerID)
			{
				if (pTimer->bKilled == FALSE) {
					if (IsWindow(pControl->m_pManager->m_hWndPaint))
						KillTimer(pTimer->hWnd, pTimer->uWinTimer);
					pTimer->bKilled = TRUE;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
//销毁时钟
ZAPI(ZuiVoid) ZuiPaintManagerKillTimer(ZuiControl pControl)
{
	ASSERT(pControl != NULL);
	if (pControl->m_pManager) {
		int count = darray_len(pControl->m_pManager->m_aTimers);
		for (int i = 0, j = 0; i < count; i++) {
			TIMERINFO* pTimer = (TIMERINFO*)(pControl->m_pManager->m_aTimers->data[i - j]);
			if (pTimer->pSender == pControl && pTimer->hWnd == pControl->m_pManager->m_hWndPaint) {
				if (pTimer->bKilled == FALSE)
					KillTimer(pTimer->hWnd, pTimer->uWinTimer);
				free(pTimer);
				darray_delete(pControl->m_pManager->m_aTimers, i - j);
				j++;
			}
		}
	}
}
//销毁全部时钟
ZAPI(ZuiVoid) ZuiPaintManagerRemoveAllTimers(ZuiPaintManager p)
{
	for (int i = 0; i < darray_len(p->m_aTimers); i++) {
		TIMERINFO* pTimer = (TIMERINFO*)(p->m_aTimers->data[i]);
		if (pTimer->hWnd == p->m_hWndPaint) {
			if (pTimer->bKilled == FALSE) {
				if (IsWindow(p->m_hWndPaint)) KillTimer(p->m_hWndPaint, pTimer->uWinTimer);
			}
			free(pTimer);
		}
	}

	darray_empty(p->m_aTimers);
}
//-------------------------------------------------------------------------------------------------

//俘获输入
ZAPI(ZuiVoid) ZuiPaintManagerSetCapture(ZuiPaintManager p)
{
	SetCapture(p->m_hWndPaint);
	p->m_bMouseCapture = TRUE;
}
//释放俘获
ZAPI(ZuiVoid) ZuiPaintManagerReleaseCapture(ZuiPaintManager p)
{
	ReleaseCapture();
	p->m_bMouseCapture = FALSE;
}

//-------------------------------------------------------------------------------------------------

ZAPI(ZuiBool) ZuiPaintManagerSetNextTabControl(ZuiPaintManager p, BOOL bForward)
{
	// If we're in the process of restructuring the layout we can delay the
	// focus calulation until the next repaint.
	if (p->m_bUpdateNeeded && bForward) {
		p->m_bFocusNeeded = TRUE;
		InvalidateRect(p->m_hWndPaint, NULL, FALSE);
		return TRUE;
	}
	// Find next/previous tabbable control
	FINDTABINFO info1 = { 0 };
	info1.pFocus = p->m_pFocus;
	info1.bForward = bForward;
	ZuiControl pControl = (ZuiControl)ZuiControlCall(Proc_FindControl, p->m_pRoot, __FindControlFromTab, &info1, (void *)(ZFIND_VISIBLE | ZFIND_ENABLED | ZFIND_ME_FIRST));
	if (pControl == NULL) {
		if (bForward) {
			// Wrap around
			FINDTABINFO info2 = { 0 };
			info2.pFocus = bForward ? NULL : info1.pLast;
			info2.bForward = bForward;
			pControl = (ZuiControl)ZuiControlCall(Proc_FindControl, p->m_pRoot, __FindControlFromTab, &info2, (void *)(ZFIND_VISIBLE | ZFIND_ENABLED | ZFIND_ME_FIRST));
		}
		else {
			pControl = info1.pLast;
		}
	}
	if (pControl != NULL) ZuiPaintManagerSetFocus(p, pControl,TRUE);
	p->m_bFocusNeeded = FALSE;
	return TRUE;
}
//-------------------------------------------------------------------------------------------------

//发送绘制请求控件的数量
int CPaintManagerUI_GetPostPaintCount(ZuiPaintManager p)
{
	return darray_len(p->m_aPostPaintControls);
}
//添加
ZAPI(ZuiBool) ZuiPaintManagerAddPostPaint(ZuiPaintManager p, ZuiControl pControl)
{
	ASSERT(darray_find(p->m_aPostPaintControls,pControl) < 0);
	return darray_append(p->m_aPostPaintControls,pControl);
}

ZAPI(ZuiBool) ZuiPaintManagerRemovePostPaint(ZuiPaintManager p, ZuiControl pControl)
{
	for (int i = 0; i < darray_len(p->m_aPostPaintControls); i++) {
		if ((ZuiControl)(p->m_aPostPaintControls->data[i]) == pControl) {
			return darray_delete(p->m_aPostPaintControls,i);
		}
	}
	return FALSE;
}

ZAPI(ZuiBool) ZuiPaintManagerSetPostPaintIndex(ZuiPaintManager p, ZuiControl pControl, int iIndex)
{
	ZuiPaintManagerRemovePostPaint(p, pControl);
	return darray_insert(p->m_aPostPaintControls,iIndex, pControl);
}


ZAPI(ZuiControl) ZuiPaintManagerFindControl(ZuiPaintManager p, POINT pt)
{
	return (ZuiControl)ZuiControlCall(Proc_FindControl, p->m_pRoot, __FindControlFromPoint, &pt, (void *)(ZFIND_VISIBLE | ZFIND_HITTEST | ZFIND_TOP_FIRST));
}


ZAPI(ZuiControl) ZuiPaintManagerFindSubControlByPoint(ZuiPaintManager p, ZuiControl pParent, POINT pt)
{
	if (pParent == NULL) pParent = p->m_pRoot;
	ASSERT(pParent);
	return (ZuiControl)ZuiControlCall(Proc_FindControl, pParent, __FindControlFromPoint, &pt, (void *)(ZFIND_VISIBLE | ZFIND_HITTEST | ZFIND_TOP_FIRST));
}

//-------------------------------------------------------------------------------------------------
//控件查找函数

ZuiControl CALLBACK __FindControlFromCount(ZuiControl pThis, LPVOID pData)
{
	int* pnCount = (int*)(pData);
	(*pnCount)++;
	return NULL;  // Count all controls
}

ZuiControl CALLBACK __FindControlFromPoint(ZuiControl pThis, LPVOID pData)
{
	LPPOINT pPoint = (LPPOINT)(pData);
	return PtInRect((RECT *)ZuiControlCall(Proc_GetPos, pThis, NULL, NULL, NULL), *pPoint) ? pThis : NULL;
}

ZuiControl CALLBACK __FindControlFromTab(ZuiControl pThis, LPVOID pData)
{
	FINDTABINFO* pInfo = (FINDTABINFO*)(pData);
	if (pInfo->pFocus == pThis) {
		if (pInfo->bForward) pInfo->bNextIsIt = TRUE;
		return pInfo->bForward ? NULL : pInfo->pLast;
	}
	if (((int)ZuiControlCall(Proc_GetControlFlags, pThis, NULL, NULL, NULL) & ZFLAG_TABSTOP) == 0)
		return NULL;
	pInfo->pLast = pThis;
	if (pInfo->bNextIsIt) return pThis;
	if (pInfo->pFocus == NULL) return pThis;
	return NULL;  // Examine all controls
}

ZuiControl CALLBACK __FindControlFromShortcut(ZuiControl pThis, LPVOID pData)
{
	if (!pThis->m_bVisible) return NULL;
	FINDSHORTCUT* pFS = (FINDSHORTCUT*)(pData);
	if (pFS->ch == toupper(pThis->m_chShortcut)) pFS->bPickNext = TRUE;
	return pFS->bPickNext ? pThis : NULL;
}

ZuiControl CALLBACK __FindControlsFromUpdate(ZuiControl pThis, LPVOID pData)
{
	if (pThis->m_bUpdateNeeded) {
		darray_append(pThis->m_pManager->m_aFoundControls,(LPVOID)pThis);
		return pThis;
	}
	return NULL;
}


//消息处理函数
ZAPI(ZuiBool) ZuiPaintManagerMessageHandler(ZuiPaintManager p, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *lRes)
{
	if (p->m_hWndPaint == NULL) return FALSE;

	// 事件的自定义处理
	switch (uMsg) {
	case WM_CLOSE:	//关闭窗口
	{
						// Make sure all matching "closing" events are sent
						TEventUI event = { 0 };
						event.ptMouse = p->m_ptLastMousePos;
						event.wKeyState = MapKeyState();
						event.dwTimestamp = GetTickCount();
						if (p->m_pEventHover != NULL) {
							event.Type = ZEVENT_MOUSELEAVE;
							event.pSender = p->m_pEventHover;
							ZuiControlEvent(p->m_pEventHover, &event);
						}
						if (p->m_pEventClick != NULL) {
							event.Type = ZEVENT_BUTTONUP;
							event.pSender = p->m_pEventClick;
							ZuiControlEvent(p->m_pEventClick, &event);
						}

						SetFocus(NULL);

						if (GetActiveWindow() == p->m_hWndPaint) {
							HWND hwndParent = GetWindowOwner(p->m_hWndPaint);
							if (hwndParent != NULL) SetFocus(hwndParent);
						}
						if (p->m_hwndTooltip != NULL) //by jiangdong 修改当父窗体以成员变量形式在窗口类中存在时候,当点击父窗体关闭按钮的时候
							//提示框内容还停留在页面中，没有销毁。
						{
							DestroyWindow(p->m_hwndTooltip);
							p->m_hwndTooltip = NULL;
						}
	}
		break;
	case WM_ERASEBKGND:
	{
						  //不允许背景擦除,防止闪屏
						  *lRes = 1;
	}
		return TRUE;
	case WM_PAINT:	//绘制
	{
						if (p->m_pRoot == NULL) {	//没有控件树
							PAINTSTRUCT ps = { 0 };
							BeginPaint(p->m_hWndPaint, &ps);
							//CRenderEngine::DrawColor(p->m_hDcPaint, ps.rcPaint, 0xFFFF0000);
							EndPaint(p->m_hWndPaint, &ps);
							return TRUE;
						}

						RECT rcClient = { 0 };
						GetClientRect(p->m_hWndPaint, &rcClient);

						RECT rcPaint = { 0 };
						if (p->m_bLayered) {
							p->m_bOffscreenPaint = TRUE;
							rcPaint = p->m_rcLayeredUpdate;
							if (IsRectEmpty(&p->m_rcLayeredUpdate)) {
								PAINTSTRUCT ps = { 0 };
								BeginPaint(p->m_hWndPaint, &ps);
								EndPaint(p->m_hWndPaint, &ps);
								return TRUE;
							}
							if (rcPaint.right > rcClient.right) rcPaint.right = rcClient.right;
							if (rcPaint.bottom > rcClient.bottom) rcPaint.bottom = rcClient.bottom;
							memset(&p->m_rcLayeredUpdate, 0, sizeof(p->m_rcLayeredUpdate));
						}
						else {
							if (!GetUpdateRect(p->m_hWndPaint, &rcPaint, FALSE)) return TRUE;
						}

						p->m_bIsPainting = TRUE;
						//是否需要更新控件布局
						if (p->m_bUpdateNeeded) {	//更新控件布局
							p->m_bUpdateNeeded = FALSE;
							if (!IsRectEmpty(&rcClient)) {
								if (p->m_pRoot->m_bUpdateNeeded) {
									RECT rcRoot = rcClient;
									if (p->m_hDcOffscreen != NULL)
										ZuiDestroyGraphics(p->m_hDcOffscreen);
									p->m_hDcOffscreen = NULL;
									if (p->m_bLayered) {
										rcRoot.left += p->m_rcLayeredInset.left;
										rcRoot.top += p->m_rcLayeredInset.top;
										rcRoot.right -= p->m_rcLayeredInset.right;
										rcRoot.bottom -= p->m_rcLayeredInset.bottom;
									}
									ZuiControlCall(Proc_SetPos, p->m_pRoot, &rcRoot, (void *)TRUE, 0);
								}
								else {
									ZuiControl pControl = NULL;
									darray_empty(p->m_aFoundControls);
									ZuiControlCall(Proc_FindControl, p->m_pRoot, __FindControlsFromUpdate, NULL, (void *)(ZFIND_VISIBLE | ZFIND_ME_FIRST | ZFIND_UPDATETEST));
									for (int it = 0; it < darray_len(p->m_aFoundControls); it++) {
										pControl = (ZuiControl)(p->m_aFoundControls->data[it]);
										if (!pControl->m_bFloat)
											ZuiControlCall(Proc_SetPos, pControl, (RECT *)ZuiControlCall(Proc_GetPos, pControl, NULL, NULL, NULL), (void *)TRUE, 0);
										else
											ZuiControlCall(Proc_SetPos, pControl, ZuiControlCall(Proc_GetRelativePos, pControl, NULL, NULL, NULL), (void *)TRUE, 0);
									}
								}
								// We'll want to notify the window when it is first initialized
								// with the correct layout. The window form would take the time
								// to submit swipes/animations.
								if (p->m_bFirstLayout) {
									p->m_bFirstLayout = FALSE;
									//第一次更新布局完成 相当于窗口初始化完成
									if (p->m_bLayered && p->m_bLayeredChanged) {
										ZuiPaintManagerInvalidate(p);
										p->m_bIsPainting = FALSE;
										return TRUE;
									}
								}
							}
						}
						else if (p->m_bLayered && p->m_bLayeredChanged) {
							RECT rcRoot = rcClient;
							if (p->m_pOffscreenBits)
								memset(p->m_pOffscreenBits, 0, (rcRoot.right - rcRoot.left) * (rcRoot.bottom - rcRoot.top) * 4);
							rcRoot.left += p->m_rcLayeredInset.left;
							rcRoot.top += p->m_rcLayeredInset.top;
							rcRoot.right -= p->m_rcLayeredInset.right;
							rcRoot.bottom -= p->m_rcLayeredInset.bottom;
							ZuiControlCall(Proc_SetPos, p->m_pRoot, &rcRoot, (void *)TRUE, 0);
						}
						// Set focus to first control?
						if (p->m_bFocusNeeded) {
							ZuiPaintManagerSetNextTabControl(p, TRUE);
						}
						//
						// 渲染屏幕
						//
						// 创建缓存图形
						if (p->m_bOffscreenPaint && p->m_hDcOffscreen == NULL)
						{
							p->m_hDcOffscreen = ZuiCreateGraphicsInMemory(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
							p->m_pOffscreenBits = p->m_hDcOffscreen->Bits;
						}
						// 开始窗口绘制
						PAINTSTRUCT ps = { 0 };
						BeginPaint(p->m_hWndPaint, &ps);
						//是否双缓存绘图
						if (p->m_bOffscreenPaint)
						{
							HBITMAP hOldBitmap = (HBITMAP) SelectObject(p->m_hDcOffscreen->hdc, p->m_hDcOffscreen->HBitmap);
							int iSaveDC = SaveDC(p->m_hDcOffscreen->hdc);
							//if (p->m_bLayered && p->m_diLayered.pImageInfo == NULL) {
							//擦除绘制区域背景
							COLORREF* pOffscreenBits = NULL;
							for (LONG y = rcClient.bottom - rcPaint.bottom; y < rcClient.bottom - rcPaint.top; ++y) {
								for (LONG x = rcPaint.left; x < rcPaint.right; ++x) {
									pOffscreenBits = p->m_pOffscreenBits + y*(rcClient.right - rcClient.left) + x;
									*pOffscreenBits = 0;
								}
							}
							
							ZuiControlCall(Proc_OnPaint, p->m_pRoot, p->m_hDcOffscreen, &rcPaint, NULL);

							for (int i = 0; i < darray_len(p->m_aPostPaintControls); i++) {
								ZuiControl pPostPaintControl = (ZuiControl)(p->m_aPostPaintControls->data[i]);
								ZuiControlCall(Proc_OnPostPaint, pPostPaintControl, p->m_hDcOffscreen, &rcPaint, NULL);
							}
							RestoreDC(p->m_hDcOffscreen->hdc, iSaveDC);
							if (p->m_bLayered) {
								RECT rcWnd = { 0 };
								GetWindowRect(p->m_hWndPaint, &rcWnd);
								DWORD dwWidth = rcClient.right - rcClient.left;
								DWORD dwHeight = rcClient.bottom - rcClient.top;
								RECT rcLayeredClient = rcClient;
								rcLayeredClient.left += p->m_rcLayeredInset.left;
								rcLayeredClient.top += p->m_rcLayeredInset.top;
								rcLayeredClient.right -= p->m_rcLayeredInset.right;
								rcLayeredClient.bottom -= p->m_rcLayeredInset.bottom;


								BLENDFUNCTION bf = { AC_SRC_OVER, 0, p->m_nOpacity, AC_SRC_ALPHA };
								POINT ptPos = { rcWnd.left, rcWnd.top };
								SIZE sizeWnd = { dwWidth, dwHeight };
								POINT ptSrc = { 0, 0 };
								g_fUpdateLayeredWindow(p->m_hWndPaint, p->m_hDcPaint, &ptPos, &sizeWnd, p->m_hDcOffscreen->hdc, &ptSrc, 0, &bf, ULW_ALPHA);
							}
							else
								BitBlt(p->m_hDcPaint, rcPaint.left, rcPaint.top, rcPaint.right - rcPaint.left, rcPaint.bottom - rcPaint.top, p->m_hDcOffscreen->hdc, rcPaint.left, rcPaint.top, SRCCOPY);
							SelectObject(p->m_hDcOffscreen->hdc, hOldBitmap);

							if (p->m_bShowUpdateRect && !p->m_bLayered) { //绘制更新矩形
								HPEN hOldPen = (HPEN)SelectObject(p->m_hDcPaint, m_hUpdateRectPen);
								SelectObject(p->m_hDcPaint, GetStockObject(HOLLOW_BRUSH));
								Rectangle(p->m_hDcPaint, rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
								SelectObject(p->m_hDcPaint, hOldPen);
							}
						}
						else
						{
							// A standard paint job
							int iSaveDC = SaveDC(p->m_hDcPaint);
							ZuiControlCall(Proc_OnPaint, p->m_pRoot, p->m_hDcPaint, &rcPaint, NULL);
							RestoreDC(p->m_hDcPaint, iSaveDC);
						}
						// All Done!
						EndPaint(p->m_hWndPaint, &ps);

						p->m_bIsPainting = FALSE;
						p->m_bLayeredChanged = FALSE;
						if (p->m_bUpdateNeeded) ZuiPaintManagerInvalidate(p);
	}
		return TRUE;
	case WM_PRINTCLIENT:
	{
						   if (p->m_pRoot == NULL) break;
						   RECT rcClient;
						   GetClientRect(p->m_hWndPaint, &rcClient);
						   HDC hDC = (HDC)wParam;
						   int save = SaveDC(hDC);
						   ZuiControlCall(Proc_OnPaint, p->m_pRoot, hDC, &rcClient, NULL);
						   // Check for traversing children. The crux is that WM_PRINT will assume
						   // that the DC is positioned at frame coordinates and will paint the child
						   // control at the wrong position. We'll simulate the entire thing instead.
						   if ((lParam & PRF_CHILDREN) != 0) {
							   HWND hWndChild = GetWindow(p->m_hWndPaint, GW_CHILD);
							   while (hWndChild != NULL) {
								   RECT rcPos = { 0 };
								   GetWindowRect(hWndChild, &rcPos);
								   MapWindowPoints(HWND_DESKTOP, p->m_hWndPaint, (LPPOINT)(&rcPos), 2);
								   SetWindowOrgEx(hDC, -rcPos.left, -rcPos.top, NULL);
								   // NOTE: We use WM_PRINT here rather than the expected WM_PRINTCLIENT
								   //       since the latter will not print the nonclient correctly for
								   //       EDIT controls.
								   SendMessage(hWndChild, WM_PRINT, wParam, lParam | PRF_NONCLIENT);
								   hWndChild = GetWindow(hWndChild, GW_HWNDNEXT);
							   }
						   }
						   RestoreDC(hDC, save);
	}
		break;
	case WM_GETMINMAXINFO:	//取窗口最大最小位置
	{
								LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
								if (p->m_szMinWindow.cx > 0) lpMMI->ptMinTrackSize.x = p->m_szMinWindow.cx;
								if (p->m_szMinWindow.cy > 0) lpMMI->ptMinTrackSize.y = p->m_szMinWindow.cy;
								if (p->m_szMaxWindow.cx > 0) lpMMI->ptMaxTrackSize.x = p->m_szMaxWindow.cx;
								if (p->m_szMaxWindow.cy > 0) lpMMI->ptMaxTrackSize.y = p->m_szMaxWindow.cy;
	}
		break;
	case WM_SIZE:	//大小被改变
	{
						if (p->m_pFocus != NULL) {
							TEventUI event = { 0 };
							event.Type = ZEVENT_WINDOWSIZE;
							event.pSender = p->m_pFocus;
							event.wParam = wParam;
							event.lParam = lParam;
							event.dwTimestamp = GetTickCount();
							event.ptMouse = p->m_ptLastMousePos;
							event.wKeyState = MapKeyState();
							ZuiControlEvent(p->m_pFocus, &event);
						}
						if (p->m_pRoot != NULL) ZuiControlNeedUpdate(p->m_pRoot);
						if (p->m_bLayered) ZuiPaintManagerInvalidate(p);
	}
		return TRUE;
	case WM_TIMER:	//时钟事件
	{
						if (LOWORD(wParam) == LAYEREDUPDATE_TIMERID) {
							if (p->m_bLayered && !IsRectEmpty(&p->m_rcLayeredUpdate)) {
								LRESULT lRes = 0;
								if (!IsIconic(p->m_hWndPaint)) 
									ZuiPaintManagerMessageHandler(p, WM_PAINT, 0, 0L, &lRes);
								break;
							}
						}
						for (int i = 0; i < darray_len(p->m_aTimers); i++) {
							const TIMERINFO* pTimer = (TIMERINFO*)(p->m_aTimers->data[i]);
							if (pTimer->hWnd == p->m_hWndPaint && pTimer->uWinTimer == LOWORD(wParam) && pTimer->bKilled == FALSE) {
								TEventUI event = { 0 };
								event.Type = ZEVENT_TIMER;
								event.pSender = pTimer->pSender;
								event.dwTimestamp = GetTickCount();
								event.ptMouse = p->m_ptLastMousePos;
								event.wKeyState = MapKeyState();
								event.wParam = pTimer->nLocalID;
								event.lParam = lParam;
								ZuiControlEvent(pTimer->pSender, &event);
								break;
							}
						}
	}
		break;
	case WM_MOUSEHOVER:	//鼠标悬停
	{
							if (p->m_pRoot == NULL) break;
							p->m_bMouseTracking = FALSE;
							POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
							ZuiControl pHover = ZuiPaintManagerFindControl(p, pt);
							if (pHover == NULL) break;
							// Generate mouse hover event
							if (p->m_pEventHover != NULL) {
								TEventUI event = { 0 };
								event.Type = ZEVENT_MOUSEHOVER;
								event.pSender = p->m_pEventHover;
								event.wParam = wParam;
								event.lParam = lParam;
								event.dwTimestamp = GetTickCount();
								event.ptMouse = pt;
								event.wKeyState = MapKeyState();
								ZuiControlEvent(p->m_pEventHover, &event);
							}
							// Create tooltip information
							if (!pHover->m_sToolTip)
								return TRUE;
							memset(&p->m_ToolTip, 0, sizeof(TOOLINFO));
							p->m_ToolTip.cbSize = sizeof(TOOLINFO);
							p->m_ToolTip.uFlags = TTF_IDISHWND;
							p->m_ToolTip.hwnd = p->m_hWndPaint;
							p->m_ToolTip.uId = (UINT_PTR)p->m_hWndPaint;
							p->m_ToolTip.hinst = m_hInstance;
							p->m_ToolTip.lpszText = (LPWSTR)pHover->m_sToolTip;
							memcpy(&p->m_ToolTip.rect, (RECT *)ZuiControlCall(Proc_GetPos, pHover, NULL, NULL, NULL), sizeof(RECT));
							if (p->m_hwndTooltip == NULL) {
								p->m_hwndTooltip = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, p->m_hWndPaint, NULL, m_hInstance, NULL);
								if (p->m_hwndTooltip != NULL && p->m_iTooltipWidth >= 0) {
									p->m_iTooltipWidth = (int)SendMessage(p->m_hwndTooltip, TTM_SETMAXTIPWIDTH, 0, p->m_iTooltipWidth);
								}
								SendMessage(p->m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM)&p->m_ToolTip);
							}
							SendMessage(p->m_hwndTooltip, TTM_SETMAXTIPWIDTH, 0, pHover->m_nTooltipWidth);
							SendMessage(p->m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&p->m_ToolTip);
							SendMessage(p->m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&p->m_ToolTip);
	}
		return TRUE;
	case WM_MOUSELEAVE:	//鼠标离开
	{
							if (p->m_pRoot == NULL) break;
							if (p->m_hwndTooltip != NULL) SendMessage(p->m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&p->m_ToolTip);
							if (p->m_bMouseTracking) {
								POINT pt = { 0 };
								RECT rcWnd = { 0 };
								GetCursorPos(&pt);
								GetWindowRect(p->m_hWndPaint, &rcWnd);
								if (!IsIconic(p->m_hWndPaint) && GetActiveWindow() == p->m_hWndPaint && PtInRect(&rcWnd, pt)) {
									if (SendMessage(p->m_hWndPaint, WM_NCHITTEST, 0, MAKELPARAM(pt.x, pt.y)) == HTCLIENT) {
										ScreenToClient(p->m_hWndPaint, &pt);
										SendMessage(p->m_hWndPaint, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
									}
									else
										SendMessage(p->m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM)-1);
								}
								else
									SendMessage(p->m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM)-1);
							}
							p->m_bMouseTracking = FALSE;
	}
		break;
	case WM_MOUSEMOVE:	//鼠标移动
	{
							if (p->m_pRoot == NULL) break;
							// Start tracking this entire window again...
							if (!p->m_bMouseTracking) {
								TRACKMOUSEEVENT tme = { 0 };
								tme.cbSize = sizeof(TRACKMOUSEEVENT);
								tme.dwFlags = TME_HOVER | TME_LEAVE;
								tme.hwndTrack = p->m_hWndPaint;
								tme.dwHoverTime = p->m_hwndTooltip == NULL ? p->m_iHoverTime : (DWORD) SendMessage(p->m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
								_TrackMouseEvent(&tme);
								p->m_bMouseTracking = TRUE;
							}
							// Generate the appropriate mouse messages
							POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
							p->m_ptLastMousePos = pt;
							ZuiControl pNewHover = ZuiPaintManagerFindControl(p, pt);
							if (pNewHover != NULL && pNewHover->m_pManager != p) break;
							TEventUI event = { 0 };
							event.ptMouse = pt;
							event.wParam = wParam;
							event.lParam = lParam;
							event.dwTimestamp = GetTickCount();
							event.wKeyState = MapKeyState();
							if (!p->m_bMouseCapture) {
								pNewHover = ZuiPaintManagerFindControl(p, pt);
								if (pNewHover != NULL && pNewHover->m_pManager != p) break;
								if (pNewHover != p->m_pEventHover && p->m_pEventHover != NULL) {
									event.Type = ZEVENT_MOUSELEAVE;
									event.pSender = p->m_pEventHover;
									ZuiControlEvent(p->m_pEventHover, &event);
									p->m_pEventHover = NULL;
									if (p->m_hwndTooltip != NULL) SendMessage(p->m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&p->m_ToolTip);
								}
								if (pNewHover != p->m_pEventHover && pNewHover != NULL) {
									event.Type = ZEVENT_MOUSEENTER;
									event.pSender = pNewHover;
									ZuiControlEvent(pNewHover, &event);
									p->m_pEventHover = pNewHover;
								}
							}
							if (p->m_pEventClick != NULL) {
								event.Type = ZEVENT_MOUSEMOVE;
								event.pSender = p->m_pEventClick;
								ZuiControlEvent(p->m_pEventClick, &event);
							}
							else if (pNewHover != NULL) {
								event.Type = ZEVENT_MOUSEMOVE;
								event.pSender = pNewHover;
								ZuiControlEvent(pNewHover, &event);
							}
	}
		break;
	case WM_LBUTTONDOWN://鼠标左键按下
	{
							// We alway set focus back to our app (this helps
							// when Win32 child windows are placed on the dialog
							// and we need to remove them on focus change).
							SetFocus(p->m_hWndPaint);
							if (p->m_pRoot == NULL) break;
							POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
							p->m_ptLastMousePos = pt;
							ZuiControl pControl = ZuiPaintManagerFindControl(p, pt);
							if (pControl == NULL) break;
							if (pControl->m_pManager != p) break;
							p->m_pEventClick = pControl;
							ZuiControlCall(Proc_SetFocus, pControl, 0, 0, 0);
							ZuiPaintManagerSetCapture(p);
							TEventUI event = { 0 };
							event.Type = ZEVENT_BUTTONDOWN;
							event.pSender = pControl;
							event.wParam = wParam;
							event.lParam = lParam;
							event.ptMouse = pt;
							event.wKeyState = (WORD)wParam;
							event.dwTimestamp = GetTickCount();
							ZuiControlEvent(pControl, &event);
	}
		break;
	case WM_LBUTTONDBLCLK://鼠标左键单击
	{
							  SetFocus(p->m_hWndPaint);
							  POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
							  p->m_ptLastMousePos = pt;
							  ZuiControl pControl = ZuiPaintManagerFindControl(p, pt);
							  if (pControl == NULL) break;
							  if (pControl->m_pManager != p) break;
							  ZuiPaintManagerSetCapture(p);
							  TEventUI event = { 0 };
							  event.Type = ZEVENT_DBLCLICK;
							  event.pSender = pControl;
							  event.ptMouse = pt;
							  event.wKeyState = (WORD)wParam;
							  event.dwTimestamp = GetTickCount();
							  ZuiControlEvent(pControl, &event);
							  p->m_pEventClick = pControl;
	}
		break;
	case WM_LBUTTONUP://鼠标左键弹起
	{
						  POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
						  p->m_ptLastMousePos = pt;
						  if (p->m_pEventClick == NULL) break;
						  ZuiPaintManagerReleaseCapture(p);
						  TEventUI event = { 0 };
						  event.Type = ZEVENT_BUTTONUP;
						  event.pSender = p->m_pEventClick;
						  event.wParam = wParam;
						  event.lParam = lParam;
						  event.ptMouse = pt;
						  event.wKeyState = (WORD)wParam;
						  event.dwTimestamp = GetTickCount();
						  // By daviyang35 at 2015-6-5 16:10:13
						  // 在Click事件中弹出了模态对话框，退出阶段窗口实例可能已经删除
						  // this成员属性赋值将会导致heap错误
						  // this成员函数调用将会导致野指针异常
						  // 使用栈上的成员来调用响应，提前清空成员
						  // 当阻塞的模态窗口返回时，回栈阶段不访问任何类实例方法或属性
						  // 将不会触发异常
						  ZuiControl pClick = p->m_pEventClick;
						  p->m_pEventClick = NULL;
						  ZuiControlEvent(pClick, &event);
	}
		break;
	case WM_RBUTTONDOWN://鼠标右键按下
	{
							SetFocus(p->m_hWndPaint);
							POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
							p->m_ptLastMousePos = pt;
							ZuiControl pControl = ZuiPaintManagerFindControl(p, pt);
							if (pControl == NULL) break;
							if (pControl->m_pManager != p) break;
							ZuiControlCall(Proc_SetFocus, pControl, 0, 0, 0);
							TEventUI event = { 0 };
							event.Type = ZEVENT_RBUTTONDOWN;
							event.pSender = pControl;
							event.wParam = wParam;
							event.lParam = lParam;
							event.ptMouse = pt;
							event.wKeyState = (WORD)wParam;
							event.dwTimestamp = GetTickCount();
							ZuiControlEvent(pControl, &event);
							p->m_pEventClick = pControl;
	}
		break;
	case WM_CONTEXTMENU:
	{
						   if (p->m_pRoot == NULL) break;
						   if (p->m_bMouseCapture) break;
						   POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
						   ScreenToClient(p->m_hWndPaint, &pt);
						   p->m_ptLastMousePos = pt;
						   if (p->m_pEventClick == NULL) break;
						   TEventUI event = { 0 };
						   event.Type = ZEVENT_CONTEXTMENU;
						   event.pSender = p->m_pEventClick;
						   event.ptMouse = pt;
						   event.wKeyState = (WORD)wParam;
						   event.lParam = (LPARAM)p->m_pEventClick;
						   event.dwTimestamp = GetTickCount();
						   ZuiControlEvent(p->m_pEventClick, &event);
						   p->m_pEventClick = NULL;
	}
		break;
	case WM_MOUSEWHEEL:	//鼠标滚动
	{
							if (p->m_pRoot == NULL) break;
							POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
							ScreenToClient(p->m_hWndPaint, &pt);
							p->m_ptLastMousePos = pt;
							ZuiControl pControl = ZuiPaintManagerFindControl(p, pt);
							if (pControl == NULL) break;
							if (pControl->m_pManager != p) break;
							int zDelta = (int)(short)HIWORD(wParam);
							TEventUI event = { 0 };
							event.Type = ZEVENT_SCROLLWHEEL;
							event.pSender = pControl;
							event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
							event.lParam = lParam;
							event.wKeyState = MapKeyState();
							event.dwTimestamp = GetTickCount();
							ZuiControlEvent(pControl, &event);

							// Let's make sure that the scroll item below the cursor is the same as before...
							SendMessage(p->m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM)MAKELPARAM(p->m_ptLastMousePos.x, p->m_ptLastMousePos.y));
	}
		break;
	case WM_CHAR:
	{
					if (p->m_pRoot == NULL) break;
					if (p->m_pFocus == NULL) break;
					TEventUI event = { 0 };
					event.Type = ZEVENT_CHAR;
					event.pSender = p->m_pFocus;
					event.wParam = wParam;
					event.lParam = lParam;
					event.chKey = (TCHAR)wParam;
					event.ptMouse = p->m_ptLastMousePos;
					event.wKeyState = MapKeyState();
					event.dwTimestamp = GetTickCount();
					ZuiControlEvent(p->m_pFocus, &event);
	}
		break;
	case WM_KEYDOWN://按下某键
	{
						if (p->m_pRoot == NULL) break;
						if (p->m_pFocus == NULL) break;
						TEventUI event = { 0 };
						event.Type = ZEVENT_KEYDOWN;
						event.pSender = p->m_pFocus;
						event.wParam = wParam;
						event.lParam = lParam;
						event.chKey = (TCHAR)wParam;
						event.ptMouse = p->m_ptLastMousePos;
						event.wKeyState = MapKeyState();
						event.dwTimestamp = GetTickCount();
						ZuiControlEvent(p->m_pFocus, &event);
						p->m_pEventKey = p->m_pFocus;
	}
		break;
	case WM_KEYUP:	//弹起某键
	{
						if (p->m_pRoot == NULL) break;
						if (p->m_pEventKey == NULL) break;
						TEventUI event = { 0 };
						event.Type = ZEVENT_KEYUP;
						event.pSender = p->m_pEventKey;
						event.wParam = wParam;
						event.lParam = lParam;
						event.chKey = (TCHAR)wParam;
						event.ptMouse = p->m_ptLastMousePos;
						event.wKeyState = MapKeyState();
						event.dwTimestamp = GetTickCount();
						ZuiControlEvent(p->m_pEventKey, &event);
						p->m_pEventKey = NULL;
	}
		break;
	case WM_SETCURSOR:
	{
						 if (p->m_pRoot == NULL) break;
						 if (LOWORD(lParam) != HTCLIENT) break;
						 if (p->m_bMouseCapture) return TRUE;

						 POINT pt = { 0 };
						 GetCursorPos(&pt);
						 ScreenToClient(p->m_hWndPaint, &pt);
						 ZuiControl pControl = ZuiPaintManagerFindControl(p, pt);
						 if (pControl == NULL) break;
						 if (((int)ZuiControlCall(Proc_GetControlFlags, pControl, NULL, NULL, NULL) & ZFLAG_SETCURSOR) == 0) break;
						 TEventUI event = { 0 };
						 event.Type = ZEVENT_SETCURSOR;
						 event.pSender = pControl;
						 event.wParam = wParam;
						 event.lParam = lParam;
						 event.ptMouse = pt;
						 event.wKeyState = MapKeyState();
						 event.dwTimestamp = GetTickCount();
						 ZuiControlEvent(pControl, &event);
	}
		return TRUE;
	case WM_KILLFOCUS://失去焦点
	{
	}
		break;
	case WM_NOTIFY:
	{
					  LPNMHDR lpNMHDR = (LPNMHDR)lParam;
					  if (lpNMHDR != NULL) *lRes = SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
					  return TRUE;
	}
		break;
	case WM_COMMAND:
	{
					   if (lParam == 0) break;
					   HWND hWndChild = (HWND)lParam;
					   *lRes = SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
					   return TRUE;
	}
		break;
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORSTATIC:
	{
							  // Refer To: http://msdn.microsoft.com/en-us/library/bb761691(v=vs.85).aspx
							  // Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; instead, they send the WM_CTLCOLORSTATIC message.
							  if (lParam == 0) break;
							  HWND hWndChild = (HWND)lParam;
							  *lRes = SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
							  return TRUE;
	}
		break;
	default:
		break;
	}
	return FALSE;
}




