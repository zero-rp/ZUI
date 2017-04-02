#include <ZUI.h>
#if (defined PLATFORM_OS_WIN)
#pragma comment(lib, "Imm32.lib")
#pragma comment(lib, "Winmm.lib")
static LRESULT ZCALL __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ZuiOsWindow pThis = NULL;
    if (uMsg == WM_NCCREATE) {
        LPCREATESTRUCT lpcs = (LPCREATESTRUCT)(lParam);
        pThis = (ZuiWindow)(lpcs->lpCreateParams);
        pThis->m_hWnd = hWnd;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LPARAM)pThis);
    }
    else {
        pThis = (ZuiWindow)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        if (uMsg == WM_NCDESTROY && pThis != NULL) {
            LRESULT lRes = DefWindowProc(hWnd, uMsg, wParam, lParam);
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
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    else {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

ZuiBool ZuiOsInitialize() {
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
    return TRUE;
}
ZuiBool ZuiOsUnInitialize() {

}

ZuiOsWindow ZuiOsCreateWindow(ZuiControl root, ZuiBool show) {
    /*保存相关参数到ZOsWindow*/
    ZuiOsWindow OsWindow = (ZuiOsWindow)malloc(sizeof(ZOsWindow));
    if (OsWindow)
    {
        memset(OsWindow, 0, sizeof(ZOsWindow));

        OsWindow->root = root;

        OsWindow->m_pm = NewCPaintManagerUI();

        OsWindow->m_hWnd = CreateWindowEx(0, L"ZUI", L"",
            WS_POPUP | WS_CLIPCHILDREN,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            NULL, NULL, GetModuleHandleA(NULL),
            OsWindow);

        
        ZuiPaintManagerInit(OsWindow->m_pm, OsWindow->m_hWnd);
        ZuiPaintManagerAttachDialog(OsWindow->m_pm, root);

        //OsWindow->m_hDC = GetDC(OsWindow->m_hWnd);
        OsWindow->m_hIMC = ImmGetContext(OsWindow->m_hWnd);//获取系统的输入法
                                             /*屏蔽输入法*/
        ImmAssociateContext(OsWindow->m_hWnd, NULL);

        if (!show)
            ShowWindow(OsWindow->m_hWnd, SW_SHOW);
        return OsWindow;
    }
    return NULL;
}
ZuiVoid ZuiOsDestroyWindow(ZuiOsWindow OsWindow) {
    DestroyWindow(OsWindow->m_hWnd);
}
ZuiBool ZuiOsSetWindowTitle(ZuiOsWindow OsWindow, ZuiText Title) {
    return SetWindowText(OsWindow->m_hWnd, Title);
}
ZuiBool ZuiOsSetWindowMin(ZuiOsWindow OsWindow) {
    return ShowWindow(OsWindow->m_hWnd, SW_MINIMIZE);
}
ZuiBool ZuiOsSetWindowMax(ZuiOsWindow OsWindow) {
    OsWindow->m_bMax = TRUE;
    return ShowWindow(OsWindow->m_hWnd, SW_MAXIMIZE);
}
ZuiBool ZuiOsSetWindowRestor(ZuiOsWindow OsWindow) {
    OsWindow->m_bMax = FALSE;
    return ShowWindow(OsWindow->m_hWnd, SW_RESTORE);
}
ZuiBool ZuiOsSetWindowSize(ZuiOsWindow OsWindow, ZuiUInt w, ZuiUInt h) {
    ZuiControlCall(Proc_SetFixedWidth, OsWindow->root, w, NULL, NULL);
    ZuiControlCall(Proc_SetFixedHeight, OsWindow->root, h, NULL, NULL);
    return SetWindowPos(OsWindow->m_hWnd, NULL, 0, 0, w, h, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}
ZuiBool ZuiOsSetWindowNoBox(ZuiOsWindow OsWindow, ZuiBool b) {
    if (OsWindow->m_nobox == b)
        return FALSE;
    OsWindow->m_nobox = b;
    if (b)
    {
        SetWindowLong(OsWindow->m_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN);
    }
    else {
        SetWindowLong(OsWindow->m_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN);
    }
    RECT r;
    GetWindowRect(OsWindow->m_hWnd, &r);
    OsWindow->m_rect.Left = r.left;
    OsWindow->m_rect.Top = r.top;
    OsWindow->m_rect.Height = r.bottom - r.top;
    OsWindow->m_rect.Width = r.right - r.left;
    ZuiControlInvalidate(OsWindow->root, TRUE);
    return TRUE;
}
ZuiBool ZuiOsSetWindowComBo(ZuiOsWindow OsWindow, ZuiBool b) {
    if (OsWindow->m_combo == b)
        return FALSE;
    OsWindow->m_combo = b;
    DWORD dwStyle = GetWindowLong(OsWindow->m_hWnd, GWL_EXSTYLE);
    if (b) {
        dwStyle |= WS_EX_TOOLWINDOW;
    }
    else {
        dwStyle &= ~WS_EX_TOOLWINDOW;
    }
    SetWindowLong(OsWindow->m_hWnd, GWL_EXSTYLE, dwStyle);
    return TRUE;
}
ZuiBool ZuiOsSetWindowTool(ZuiOsWindow OsWindow, ZuiBool b) {
    DWORD dwStyle = GetWindowLong(OsWindow->m_hWnd, GWL_EXSTYLE);
    if (b) {
        dwStyle |= WS_EX_TOOLWINDOW;
    }
    else {
        dwStyle &= ~WS_EX_TOOLWINDOW;
    }
    SetWindowLong(OsWindow->m_hWnd, GWL_EXSTYLE, dwStyle);
    return TRUE;
}
ZuiVoid ZuiOsSetWindowVisible(ZuiOsWindow OsWindow, ZuiBool Visible) {
    if (Visible)
        ShowWindow(OsWindow->m_hWnd, SW_SHOWNORMAL);
    else
        ShowWindow(OsWindow->m_hWnd, SW_HIDE);
}
ZuiVoid ZuiOsWindowPopup(ZuiOsWindow OsWindow, ZuiPoint pt) {
    if (pt)
    {
        SetWindowPos(OsWindow->m_hWnd, NULL, pt->x, pt->y, OsWindow->m_rect.Width, OsWindow->m_rect.Height, SWP_NOZORDER | SWP_NOACTIVATE);
    }
    ShowWindow(OsWindow->m_hWnd, SW_SHOWNORMAL);
    SetFocus(OsWindow->m_hWnd);
}
ZuiVoid ZuiOsSetWindowCenter(ZuiOsWindow OsWindow) {
    RECT rc, rc1, rctomove;
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    rc.left = 0;
    rc.top = 0;
    rc.right = width;
    rc.bottom = height;
    GetClientRect(OsWindow->m_hWnd, &rc1);
    rctomove.left = (rc.right - rc.left) / 2 - (rc1.right - rc1.left) / 2;
    rctomove.right = (rc.right - rc.left) / 2 + (rc1.right - rc1.left) / 2;
    rctomove.top = (rc.bottom - rc.top) / 2 - (rc1.bottom - rc1.top) / 2;
    rctomove.bottom = (rc.bottom - rc.top) / 2 + (rc1.bottom - rc1.top) / 2;
    SetWindowPos(OsWindow->m_hWnd, HWND_TOPMOST, rctomove.left, rctomove.top, rc1.right - rc1.left, rc1.bottom - rc1.top, SWP_NOZORDER | SWP_NOACTIVATE);
}
#endif


