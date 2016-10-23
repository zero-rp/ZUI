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


ZAPI(ZuiAny) ZuiWindowProc(ZuiInt ProcId, ZuiControl cp, ZuiWindow p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	switch (ProcId)
	{
	case Proc_OnCreate: {
		static INT ClassRegistered;//是否注册WNDCLASS
		p = (ZuiWindow)malloc(sizeof(ZWindow));
		memset(p, 0, sizeof(ZWindow));
		//保存原来的回调地址,创建成功后回调地址指向当前函数
		//创建继承的控件 保存数据指针
		p->old_udata = ZuiVerticalLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
		p->old_call = (ZCtlProc)&ZuiVerticalLayoutProc;

		//创建宿主窗口
		if (!ClassRegistered)
		{
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
		}
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
		case ZEVENT_BUTTONDOWN: {

		}
								 break;
		case ZEVENT_BUTTONUP: {

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
	case Proc_OnInit: {

	}
					  break;
	default:
		break;
	}
	return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}




