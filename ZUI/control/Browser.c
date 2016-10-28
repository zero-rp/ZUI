#include <ZUI.h>
#pragma comment(lib, "libcef.lib")
void _staticOnPaintUpdated(wkeWebView webView, void* param, const HDC hdc, int x, int y, int cx, int cy)
{
	ZuiBrowser pthis = (ZuiBrowser)param;
	pthis->init = TRUE;
	if (pthis->cp->m_bVisible)
	{
		ZuiControlInvalidate(pthis->cp);
	}
}
// 回调：页面标题改变
void _staticOnTitleChanged(wkeWebView webWindow, void* param, const wkeString title)
{
	//wkeSetWindowTitleW(webWindow, wkeGetStringW(title));
	ZuiControlNotify(L"titlechanged", ((ZuiBrowser)param)->cp, wkeGetStringW(title), NULL, NULL);
}

// 回调：创建新的页面，比如说调用了 window.open 或者点击了 <a target="_blank" .../>
wkeWebView _staticOnCreateView(wkeWebView webWindow, void* param, wkeNavigationType navType, const wkeString url, const wkeWindowFeatures* features)
{
	//wkeWebView newWindow = wkeCreateWebWindow(WKE_WINDOW_TYPE_POPUP, NULL, features->x, features->y, features->width, features->height);
	//wkeShowWindow(newWindow, true);
	ZuiBrowser bro = ZuiControlNotify(L"newwindow", ((ZuiBrowser)param)->cp, (void *)navType, wkeGetStringW(url), features);
	if (bro)
		return ZuiControlCall(Proc_Browser_GetView, bro, NULL, NULL, NULL);
	return 0;
}
ZEXPORT ZuiAny ZCALL ZuiBrowserProc(ZuiInt ProcId, ZuiControl cp, ZuiBrowser p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	switch (ProcId)
	{
	case Proc_CoreInit: {
		wkeInitialize();
		return 0;
		break;
	}
	case Proc_OnCreate: {
		p = (ZuiBrowser)malloc(sizeof(ZBrowser));
		memset(p, 0, sizeof(ZBrowser));
		//保存原来的回调地址,创建成功后回调地址指向当前函数
		p->old_call = cp->call;
		p->cp = cp;
		p->view = wkeCreateWebView();
		wkeOnPaintUpdated(p->view, _staticOnPaintUpdated, p);//界面刷新
		wkeOnTitleChanged(p->view, _staticOnTitleChanged, p);
		wkeOnCreateView(p->view, _staticOnCreateView, p);
		return p;
	}
		break;
	case Proc_OnSize: {
		wkeResize(p->view, Param1, Param2);
		if (cp->m_bVisible) {
			wkeRepaintIfNeeded(p->view);
		}
	}
		break;
	case Proc_OnEvent: {
		TEventUI *event= (TEventUI *)Param1;
		switch (event->Type)
		{
		case ZEVENT_TIMER: {
			if (cp->m_bVisible) {
				wkeRepaintIfNeeded(p->view);
			}
		}
			break;
		case ZEVENT_SETFOCUS:
			if (p->view) wkeSetFocus(p->view); break;
		case ZEVENT_KILLFOCUS:
 			if (p->view) wkeKillFocus(p->view); break;
		case ZEVENT_CHAR: {
			unsigned int charCode = event->wParam;
			unsigned int flags = 0;
			if (HIWORD(event->lParam) & KF_REPEAT)
				flags |= WKE_REPEAT;
			if (HIWORD(event->lParam) & KF_EXTENDED)
				flags |= WKE_EXTENDED;

			if (wkeFireKeyPressEvent(p->view, charCode, flags, false))
				return 0;
		}
			break;
		case ZEVENT_KEYUP: {
			unsigned int virtualKeyCode = event->wParam;
			unsigned int flags = 0;
			if (HIWORD(event->lParam) & KF_REPEAT)
				flags |= WKE_REPEAT;
			if (HIWORD(event->lParam) & KF_EXTENDED)
				flags |= WKE_EXTENDED;

			if (wkeFireKeyUpEvent(p->view, virtualKeyCode, flags, false))
				return 0;
		}
		   break;
		case ZEVENT_KEYDOWN: {
			unsigned int virtualKeyCode = event->wParam;
			unsigned int flags = 0;
			if (HIWORD(event->lParam) & KF_REPEAT)
				flags |= WKE_REPEAT;
			if (HIWORD(event->lParam) & KF_EXTENDED)
				flags |= WKE_EXTENDED;

			if (wkeFireKeyDownEvent(p->view, virtualKeyCode, flags, false))
				return 0;
		}
			break;
		case ZEVENT_LBUTTONDOWN:
		case ZEVENT_LBUTTONUP:
		case ZEVENT_RBUTTONDOWN:
		case ZEVENT_LDBLCLICK:
		case ZEVENT_MOUSEMOVE:{
			if (event->Type == ZEVENT_LBUTTONDOWN)
			{
				ZuiPaintManagerSetCapture(cp->m_pManager);
			}
			else if (event->Type == ZEVENT_LBUTTONUP)
				ZuiPaintManagerReleaseCapture(cp->m_pManager);

			unsigned int flags = 0;
			
			if (event->wParam & MK_CONTROL)
				flags |= WKE_CONTROL;
			if (event->wParam & MK_SHIFT)
				flags |= WKE_SHIFT;

			if (event->wParam & MK_LBUTTON)
				flags |= WKE_LBUTTON;
			if (event->wParam & MK_MBUTTON)
				flags |= WKE_MBUTTON;
			if (event->wParam & MK_RBUTTON)
				flags |= WKE_RBUTTON;

			UINT uMsg = 0;
			switch (event->Type)
			{
			case ZEVENT_LBUTTONDOWN:    uMsg = WM_LBUTTONDOWN; break;
			case ZEVENT_LBUTTONUP:      uMsg = WM_LBUTTONUP; break;
			case ZEVENT_RBUTTONDOWN:   uMsg = WM_RBUTTONDOWN; break;
			case ZEVENT_LDBLCLICK:      uMsg = WM_LBUTTONDBLCLK; break;
			case ZEVENT_MOUSEMOVE:     uMsg = WM_MOUSEMOVE; break;
			}
			if (wkeFireMouseEvent(p->view, uMsg, event->ptMouse.x - cp->m_rcItem.left, event->ptMouse.y - cp->m_rcItem.top, flags))
				return 0;
		}
		break;
		case ZEVENT_SCROLLWHEEL:
		{
			POINT pt;
			pt.x = LOWORD(event->lParam);
			pt.y = HIWORD(event->lParam);
			int nFlag = GET_X_LPARAM(event->wParam);
			int delta = (nFlag == SB_LINEDOWN) ? -120 : 120;
			unsigned int flags = 0;
			if (event->wParam & MK_CONTROL)
				flags |= WKE_CONTROL;
			if (event->wParam & MK_SHIFT)
				flags |= WKE_SHIFT;
			if (event->wParam & MK_LBUTTON)
				flags |= WKE_LBUTTON;
			if (event->wParam & MK_MBUTTON)
				flags |= WKE_MBUTTON;
			if (event->wParam & MK_RBUTTON)
				flags |= WKE_RBUTTON;
			bool handled = wkeFireMouseWheelEvent(p->view,pt.x, pt.y, delta, flags);
			if (handled)
				return;
		}
			break;
		case ZEVENT_SETCURSOR:
			if (wkeFireWindowsMessage(p->view, cp->m_pManager->m_hWndPaint, WM_SETCURSOR, 0, 0, NULL))
				return 0;
			break;
		default:
			break;
		}


		
	}
		break;
	case Proc_OnPaint:{
		ZuiGraphics gp = (ZuiGraphics)Param1;
		RECT *rc = &cp->m_rcItem;
		if (p->init) {
			//ZGraphics sp;
			//sp.hdc = wkeGetViewDC(p->view);
			//sp.Width = rc->right - rc->left;
			//sp.Height = rc->bottom - rc->top;
			//ZuiAlphaBlend(gp, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, &sp, 0, 0, 255);
			BitBlt(gp->hdc, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, wkeGetViewDC(p->view), 0, 0, SRCCOPY);
		}
	}
		break;
	case Proc_SetPos:{
		wkeSetHandleOffset(p->view, ((RECT*)Param1)->left, ((RECT*)Param1)->top);
		OutputDebugString(L"a");
	}
		break;
	case Proc_Browser_LoadUrl: {
		wkeLoadURLW(p->view, Param1);
		break;
	}
	case Proc_Browser_LoadHtml: {
		wkeLoadHTMLW(p->view, ((ZuiRes)Param1)->p);
		ZuiResDBDelRes((ZuiRes)Param1);
		break;
	}
	case Proc_Browser_GetView: {
		return p->view;
		break;
	}
	case Proc_SetAttribute: {
		if (wcscmp(Param1, L"url") == 0) ZuiControlCall(Proc_Browser_LoadUrl, cp, Param2, NULL, NULL);
		if (wcscmp(Param1, L"html") == 0)
			ZuiControlCall(Proc_Browser_LoadHtml, cp, ZuiResDBGetRes(Param2, ZREST_TXT), NULL, NULL);
	}
		break;
	case Proc_OnInit:{
		wkeSetHandle(p->view, cp->m_pManager->m_hWndPaint);
		ZuiPaintManagerSetTimer(cp, 1000, 20);
	}
		break;
	case Proc_GetControlFlags:
		return ZFLAG_SETCURSOR;
		break;
	default:
		break;
	}
	return p->old_call(ProcId, cp,0, Param1, Param2, Param3);
}




