#include <ZUI.h>

static rb_root *m_window = NULL;

static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
            if (uMsg == WM_NCHITTEST && pThis->m_nobox)
            {
                int x = GET_X_LPARAM(lParam);
                int	y = GET_Y_LPARAM(lParam);
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
            else if (uMsg == WM_MOVE) {
                pThis->m_rect.Left = GET_X_LPARAM(lParam);
                pThis->m_rect.Top = GET_Y_LPARAM(lParam);
            }
            else if (uMsg == WM_SIZE)
            {
                pThis->m_rect.Width = GET_X_LPARAM(lParam);
                pThis->m_rect.Height = GET_Y_LPARAM(lParam);
            }
            else if (uMsg == WM_NCLBUTTONDOWN) {
                if (pThis->m_bMax)
                {
                    return 0;
                }
            }
            else if (uMsg == WM_KILLFOCUS && pThis->m_combo)
            {
                pThis->root->m_bVisible = FALSE;
                ShowWindow(hWnd, SW_HIDE);
            }
            else if (uMsg == WM_DESTROY)
            {
                FreeCPaintManagerUI(pThis->m_pm);
                pThis->m_pm = NULL;
                return DefWindowProc(hWnd, uMsg, wParam, lParam);
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
        wc.lpszClassName = L"ZUI";
        RegisterClass(&wc);
        m_window = rb_new();
        return TRUE;
        break;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;

        p->m_pm->m_pRoot = NULL;

        old_call(ProcId, cp, old_udata, Param1, Param2, Param3);

        DestroyWindow(p->m_hWnd);
        free(p);
        return;
        break;
    }
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
        p->m_hWnd = CreateWindowEx(0, L"ZUI", L"", (Param2 ? WS_CHILDWINDOW : WS_POPUP) | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, Param2, NULL, GetModuleHandleA(NULL), p);

        ZuiPaintManagerInit(p->m_pm, p->m_hWnd);
        ZuiPaintManagerAttachDialog(p->m_pm, cp);
        if (!Param1)
            ShowWindow(p->m_hWnd, SW_SHOW);
        return p;
        break;
    }
    case Proc_SetBorderColor: {
        if (!cp->m_dwBorderColor) {
            //以前没有边框了,加上边距
            ((ZuiLayout)((ZuiVerticalLayout)p->old_udata)->old_udata)->m_rcInset.left += 1;
            ((ZuiLayout)((ZuiVerticalLayout)p->old_udata)->old_udata)->m_rcInset.bottom += 1;
            ((ZuiLayout)((ZuiVerticalLayout)p->old_udata)->old_udata)->m_rcInset.right += 1;
            ((ZuiLayout)((ZuiVerticalLayout)p->old_udata)->old_udata)->m_rcInset.top += 1;
        }
        break;
    }
    case Proc_Layout_SetInset: {
        if (!cp->m_dwBorderColor) {
            //以前没有边框了,加上边距
            ((ZuiLayout)((ZuiVerticalLayout)p->old_udata)->old_udata)->m_rcInset.left = 1;
            ((ZuiLayout)((ZuiVerticalLayout)p->old_udata)->old_udata)->m_rcInset.bottom = 1;
            ((ZuiLayout)((ZuiVerticalLayout)p->old_udata)->old_udata)->m_rcInset.right = 1;
            ((ZuiLayout)((ZuiVerticalLayout)p->old_udata)->old_udata)->m_rcInset.top = 1;
        }
        break;
    }
    case Proc_SetText: {
        SetWindowText(p->m_hWnd, Param1);
        break;
    }
    case Proc_Window_SetWindowMin: {
        ShowWindow(p->m_hWnd, SW_MINIMIZE);
        break;
    }
    case Proc_Window_SetWindowMax: {
        ShowWindow(p->m_hWnd, SW_MAXIMIZE);
        p->m_bMax = TRUE;
        break;
    }
    case Proc_Window_SetWindowRestor: {
        ShowWindow(p->m_hWnd, SW_RESTORE);
        p->m_bMax = FALSE;
        break;
    }
    case Proc_Window_SetMinInfo: {
        cp->m_pManager->m_szMinWindow.cx = Param1;
        cp->m_pManager->m_szMinWindow.cy = Param2;
        break;
    }
    case Proc_Window_SetMaxInfo: {
        cp->m_pManager->m_szMaxWindow.cx = Param1;
        cp->m_pManager->m_szMaxWindow.cy = Param2;
        break;
    }
    case Proc_Window_SetSize: {
        ZuiControlCall(Proc_SetFixedWidth, cp, Param1, NULL, NULL);
        ZuiControlCall(Proc_SetFixedHeight, cp, Param2, NULL, NULL);
        SetWindowPos(p->m_hWnd, NULL, 0, 0, Param1, Param2, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
        break;
    }
    case Proc_Window_SetNoBox: {
        if (p->m_nobox == Param1)
            break;
        p->m_nobox = Param1;
        if (Param1)
        {
            SetWindowLong(p->m_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN);
        }
        else {
            SetWindowLong(p->m_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN);
        }
        RECT r;
        GetWindowRect(p->m_hWnd, &r);
        p->m_rect.Left = r.left;
        p->m_rect.Top = r.top;
        p->m_rect.Height = r.bottom - r.top;
        p->m_rect.Width = r.right - r.left;
        ZuiControlInvalidate(cp, TRUE);
        break;
    }
    case Proc_Window_SetComBo: {
        if (p->m_combo == Param1)
            break;
        p->m_combo = Param1;
        DWORD dwStyle = GetWindowLong(p->m_hWnd, GWL_EXSTYLE);
        if (Param1) {
            dwStyle |= WS_EX_TOOLWINDOW;
        }
        else {
            dwStyle &= ~WS_EX_TOOLWINDOW;
        }
        SetWindowLong(p->m_hWnd, GWL_EXSTYLE, dwStyle);
        break;
    }
    case Proc_SetAttribute: {
        if (wcscmp(Param1, L"nobox") == 0) ZuiControlCall(Proc_Window_SetNoBox, cp, wcscmp(Param2, L"true") == 0 ? TRUE : FALSE, NULL, NULL);
        else if (wcscmp(Param1, L"combo") == 0) ZuiControlCall(Proc_Window_SetComBo, cp, wcscmp(Param2, L"true") == 0 ? TRUE : FALSE, NULL, NULL);
        else if (wcscmp(Param1, L"layered") == 0) {
            if (wcscmp(Param2, L"true") == 0) {
                ZuiPaintManagerSetLayered(p->m_pm, TRUE);
            }
            else {
                ZuiPaintManagerSetLayered(p->m_pm, FALSE);
            }
        }
        else if (wcscmp(Param1, L"opacity") == 0) ZuiPaintManagerSetLayeredOpacity(p->m_pm, _wtoi(Param2));
        else if (wcscmp(Param1, L"mininfo") == 0) {
            LPTSTR pstr = NULL;
            int cx = wcstol(Param2, &pstr, 10);  ASSERT(pstr);
            int cy = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            ZuiControlCall(Proc_Window_SetMinInfo, cp, cx, cy, NULL);
        }
        else if (wcscmp(Param1, L"maxinfo") == 0) {
            LPTSTR pstr = NULL;
            int cx = wcstol(Param2, &pstr, 10);  ASSERT(pstr);
            int cy = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            ZuiControlCall(Proc_Window_SetMaxInfo, cp, cx, cy, NULL);
        }
        else if (wcscmp(Param1, L"size") == 0) {
            LPTSTR pstr = NULL;
            int cx = wcstol(Param2, &pstr, 10);  ASSERT(pstr);
            int cy = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            ZuiControlCall(Proc_Window_SetSize, cp, cx, cy, NULL);
        }
        else if (wcscmp(Param1, L"name") == 0) {
            if (cp->m_sName)
                rb_delete(Zui_Hash(cp->m_sName), m_window);//删除以前的名字
            rb_insert(Zui_Hash(Param2), cp, m_window);//保存现在的名字
        }
        break;
    }
    case Proc_JsHas: {
        if (wcscmp(Param1, L"SetWindowMin") == 0) return 1;
        else if (wcscmp(Param1, L"SetWindowMax") == 0) return 1;
        else if (wcscmp(Param1, L"SetWindowRestor") == 0) return 1;
        else if (wcscmp(Param1, L"Popup") == 0) return 1;
        break;
    }
    case Proc_JsCall: {
        if (wcscmp(Param1, L"SetWindowMin") == 0) ZuiControlCall(Proc_Window_SetWindowMin, cp, NULL, NULL, NULL);
        else if (wcscmp(Param1, L"SetWindowMax") == 0) ZuiControlCall(Proc_Window_SetWindowMax, cp, NULL, NULL, NULL);
        else if (wcscmp(Param1, L"SetWindowRestor") == 0) ZuiControlCall(Proc_Window_SetWindowRestor, cp, NULL, NULL, NULL);
        else if (wcscmp(Param1, L"Popup") == 0) {
            if (js_gettop(Param2) == 3) {
                ZPoint pt = { js_toint32(Param2,1),js_toint32(Param2,2) };
                ZuiControlCall(Proc_Window_Popup, cp, &pt, NULL, NULL);
            }
            else
                ZuiControlCall(Proc_Window_Popup, cp, NULL, NULL, NULL);
        }
        break;
    }
    case Proc_SetVisible: {
        if (cp->m_bVisible == (BOOL)Param1)
            return 0;
        if (Param1)
            ShowWindow(p->m_hWnd, SW_SHOWNORMAL);
        else
            ShowWindow(p->m_hWnd, SW_HIDE);
        break;
    }
    case Proc_Window_Popup: {
        cp->m_bVisible = TRUE;
        if (Param1)
        {
            SetWindowPos(p->m_hWnd, NULL, ((ZuiPoint)Param1)->x, ((ZuiPoint)Param1)->y, p->m_rect.Width, p->m_rect.Height, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        ShowWindow(p->m_hWnd, SW_SHOWNORMAL);
        SetFocus(p->m_hWnd);
        break;
    }
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}

ZEXPORT ZuiControl ZCALL ZuiWindowFindName(ZuiText Name) {
    if (!Name)
        return NULL;
    rb_node*node = rb_search(Zui_Hash(Name), m_window);
    if (!node)
        return NULL;
    return (ZuiControl)(node->data);
}


