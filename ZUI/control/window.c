#include <ZUI.h>



LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ZuiWindow pThis = NULL;
	if (uMsg == WM_NCCREATE) {

		LPCREATESTRUCT lpcs = (LPCREATESTRUCT)(lParam);
		pThis = (ZuiWindow)(lpcs->lpCreateParams);
		pThis->m_hWnd = hWnd;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LPARAM)pThis);
	}
	else {
		pThis = (ZuiWindow)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (uMsg == WM_NCDESTROY && pThis != NULL) {
			LRESULT lRes = CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
			SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
			pThis->m_hWnd = NULL;
			return lRes;
		}
	}
	if (pThis != NULL) {
		LRESULT lRes = 0;
		{
			if (uMsg == WM_CREATE) {

				return 0;
			}
			else if (uMsg == WM_DESTROY) {
				PostQuitMessage(0L);
			}
			else if (uMsg == WM_NCHITTEST)
			{
				if (pThis->m_nobox)
				{
					int x = LOWORD(lParam);
					int	y = HIWORD(lParam);
					if (x <= pThis->m_rect.Left + 3 && y <= pThis->m_rect.Top + 3) {
						return HTTOPLEFT;
					}
					else if (x <= pThis->m_rect.Left + 3 && y >= pThis->m_rect.Top + pThis->m_rect.Height - 3)
					{
						return HTBOTTOMLEFT;
					}
					else if (x >= pThis->m_rect.Left + pThis->m_rect.Width - 3 && y <= pThis->m_rect.Top + 3)
					{
						return HTTOPRIGHT;
					}
					else if (x >= pThis->m_rect.Left + pThis->m_rect.Width - 3 && y >= pThis->m_rect.Top + pThis->m_rect.Height - 3)
					{
						return HTBOTTOMRIGHT;
					}
					else if (x <= pThis->m_rect.Left + 2)
					{
						return HTLEFT;
					}
					else if (y <= pThis->m_rect.Top + 2)
					{
						return HTTOP;
					}
					else if (x >= pThis->m_rect.Left + pThis->m_rect.Width - 2)
					{
						return HTRIGHT;
					}
					else if (y >= pThis->m_rect.Top + pThis->m_rect.Height - 2)
					{
						return HTBOTTOM;
					}
					else
					{
						return HTCLIENT;
					}
					return HTCLIENT;
				}
			}
			else if(uMsg == WM_MOVE){
				pThis->m_rect.Left = LOWORD(lParam);
				pThis->m_rect.Top = HIWORD(lParam);
			}
			else if(uMsg == WM_SIZE)
			{
				pThis->m_rect.Width = LOWORD(lParam);
				pThis->m_rect.Height = HIWORD(lParam);
			}
		}
		if (pThis->m_pm)
			if (ZuiPaintManagerMessageHandler(pThis->m_pm, uMsg, wParam, lParam, &lRes))
				return lRes;
		return CallWindowProc(pThis->m_OldWndProc, pThis->m_hWnd, uMsg, wParam, lParam);
	}
	else {
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}


ZEXPORT ZuiAny ZCALL ZuiWindowProc(ZuiInt ProcId, ZuiControl cp, ZuiWindow p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	switch (ProcId)
	{
	case Proc_CoreInit: {
		WNDCLASS wc = { 0 };
		wc.style = 8;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = NULL;
		wc.lpfnWndProc = __WndProc;
		wc.hInstance = GetModuleHandleA(NULL);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = L"AA";
		RegisterClass(&wc);
		return 0;
	}
		break;
	case Proc_OnCreate: {
		p = (ZuiWindow)malloc(sizeof(ZWindow));
		memset(p, 0, sizeof(ZWindow));
		//保存原来的回调地址,创建成功后回调地址指向当前函数
		//创建继承的控件 保存数据指针
		p->old_udata = ZuiVerticalLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
		p->old_call = (ZCtlProc)&ZuiVerticalLayoutProc;

		//创建宿主窗口
		//创建绘制管理器
		p->m_pm = NewCPaintManagerUI();
		p->m_OldWndProc = DefWindowProc;
		p->root = cp;
		p->m_hWnd = CreateWindowEx(0, L"AA", L"aa", WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, GetModuleHandleA(NULL), p);
		
		ZuiPaintManagerInit(p->m_pm, p->m_hWnd);

		ZuiControl pRoot = p->root;

		ZuiPaintManagerAttachDialog(p->m_pm, pRoot);
		ShowWindow(p->m_hWnd, SW_SHOW);
		return p;
	}
						break;
	case Proc_OnEvent: {
		TEventUI *event = (TEventUI *)Param1;
		switch (event->Type)
		{
		case ZEVENT_MOUSELEAVE: {

		}
								 break;
		case ZEVENT_MOUSEENTER: {

		}
								 break;
		case ZEVENT_LBUTTONDOWN: {

		}
								 break;
		case ZEVENT_LBUTTONUP: {

		}
							   break;
		default:
			break;
		}



	}
					   break;
	case Proc_OnPaint: {

	}
					   break;
	case Proc_SetPos: {
		OutputDebugString(L"a");
	}
					  break;
	case Proc_Window_SetNoBox: {
		if (p->m_nobox == Param1)
			break;
		p->m_nobox = Param1;
		if (Param1)
		{
			SetWindowLong(p->m_hWnd, GWL_STYLE, -1811937280);
		}
		else {
			SetWindowLong(p->m_hWnd, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN);
		}
		RECT r;
		GetWindowRect(p->m_hWnd, &r);
		p->m_rect.Left = r.left;
		p->m_rect.Top = r.top;
		p->m_rect.Height = r.bottom - r.top;
		p->m_rect.Width = r.right - r.left;
		ZuiControlInvalidate(cp);
		
	}
							   break;
	case Proc_SetAttribute: {
		if (wcscmp(Param1, L"nobox") == 0) ZuiControlCall(Proc_Window_SetNoBox, cp, wcscmp(Param2, L"true") == 0 ? TRUE : FALSE, NULL, NULL);
	}
					  break;
	case Proc_OnInit: {

	}
					  break;
	default:
		break;
	}
	return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}




