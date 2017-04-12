#include <ZUI.h>

struct _tagWkeWebView;
typedef struct _tagWkeWebView* wkeWebView;
typedef enum {
    WKE_EXTENDED = 0x0100,
    WKE_REPEAT = 0x4000,

} wkeKeyFlags;
typedef enum {
    WKE_LBUTTON = 0x01,
    WKE_RBUTTON = 0x02,
    WKE_SHIFT = 0x04,
    WKE_CONTROL = 0x08,
    WKE_MBUTTON = 0x10,

} wkeMouseFlags;
typedef struct {
    int x;
    int y;
    int w;
    int h;

} wkeRect;
typedef void(*t_wkeInitialize)();
typedef void(*t_wkeDestroyWebView)(wkeWebView webView);
typedef void(*t_wkeOnTitleChanged)(wkeWebView webView, void *callback, void* callbackParam);
typedef void(*t_wkeOnURLChanged)(wkeWebView webView, void *callback, void* callbackParam);
typedef void(*t_wkeOnPaintUpdated)(wkeWebView webView, void *callback, void* callbackParam);
typedef void(*t_wkeOnCreateView)(wkeWebView webView, void *callback, void* param);
typedef void(*t_wkeRepaintIfNeeded)(wkeWebView webView);
typedef void(*t_wkeResize)(wkeWebView webView, int w, int h);
typedef void(*t_wkeSetFocus)(wkeWebView webView);
typedef void(*t_wkeKillFocus)(wkeWebView webView);
typedef BOOL(*t_wkeFireMouseWheelEvent)(wkeWebView webView, int x, int y, int delta, unsigned int flags);
typedef BOOL(*t_wkeFireKeyUpEvent)(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags, BOOL systemKey);
typedef BOOL(*t_wkeFireKeyDownEvent)(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags, BOOL systemKey);
typedef BOOL(*t_wkeFireKeyPressEvent)(wkeWebView webView, unsigned int charCode, unsigned int flags, BOOL systemKey);
typedef BOOL(*t_wkeFireMouseEvent)(wkeWebView webView, unsigned int message, int x, int y, unsigned int flags);
typedef BOOL(*t_wkeFireWindowsMessage)(wkeWebView webView, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result);
typedef wkeRect(*t_wkeGetCaretRect)(wkeWebView webView);
typedef const wchar_t*(*t_wkeGetStringW)(void *string);
typedef wkeWebView(*t_wkeCreateWebView)();
typedef HDC(*t_wkeGetViewDC)(wkeWebView webView);
typedef void(*t_wkeSetHandle)(wkeWebView webView, HWND wnd);
typedef void(*t_wkeSetHandleOffset)(wkeWebView webView, int x, int y);
typedef void(*t_wkeLoadURLW)(wkeWebView webView, const wchar_t* url);
typedef void(*t_wkeLoadHTMLW)(wkeWebView webView, const wchar_t* html);
typedef void(*t_wkeLoadFileW)(wkeWebView webView, const wchar_t* html);
typedef void(*t_wkeOnLoadUrlBegin)(wkeWebView webView, void* callback, void* callbackParam);
typedef void(*t_wkeOnNavigation)(wkeWebView webView, void* callback, void* param);
typedef void(*t_wkeSetUserAgentW)(wkeWebView webView, const wchar_t* userAgent);
typedef void*(*t_wkeRunJSW)(wkeWebView webView, const wchar_t* script);
typedef wchar_t*(*t_jsToTempStringW)(void *es, long long v);
typedef void*(*t_wkeGlobalExec)(wkeWebView webView);

t_wkeInitialize wkeInitialize;
t_wkeDestroyWebView wkeDestroyWebView;
t_wkeOnTitleChanged wkeOnTitleChanged;
t_wkeOnURLChanged wkeOnURLChanged;
t_wkeOnPaintUpdated wkeOnPaintUpdated;
t_wkeOnCreateView wkeOnCreateView;
t_wkeRepaintIfNeeded wkeRepaintIfNeeded;
t_wkeResize wkeResize;
t_wkeSetFocus wkeSetFocus;
t_wkeKillFocus wkeKillFocus;
t_wkeFireMouseWheelEvent wkeFireMouseWheelEvent;
t_wkeFireKeyUpEvent wkeFireKeyUpEvent;
t_wkeFireKeyDownEvent wkeFireKeyDownEvent;
t_wkeFireKeyPressEvent wkeFireKeyPressEvent;
t_wkeFireMouseEvent wkeFireMouseEvent;
t_wkeFireWindowsMessage wkeFireWindowsMessage;
t_wkeGetCaretRect wkeGetCaretRect;
t_wkeGetStringW wkeGetStringW;
t_wkeCreateWebView wkeCreateWebView;
t_wkeGetViewDC wkeGetViewDC;
t_wkeSetHandle wkeSetHandle;
t_wkeSetHandleOffset wkeSetHandleOffset;
t_wkeLoadURLW wkeLoadURLW;
t_wkeLoadHTMLW wkeLoadHTMLW;
t_wkeLoadFileW wkeLoadFileW;
t_wkeOnLoadUrlBegin wkeOnLoadUrlBegin;
t_wkeOnNavigation wkeOnNavigation;
t_wkeSetUserAgentW wkeSetUserAgentW;
t_wkeRunJSW wkeRunJSW;
t_jsToTempStringW jsToTempStringW;
t_wkeGlobalExec wkeGlobalExec;
// 回调：重绘
void _staticOnPaintUpdated(wkeWebView webView, void* param, const HDC hdc, int x, int y, int cx, int cy)
{
    ZuiBrowser pthis = (ZuiBrowser)param;
    pthis->init = TRUE;
    if (pthis->cp->m_bVisible)
    {
        ZuiControlInvalidate(pthis->cp, TRUE);
    }
}
// 回调：页面标题改变
void _staticOnTitleChanged(wkeWebView webWindow, void* param, void *title)
{
    ZuiBrowser p = (ZuiBrowser)param;
    if (p->titlechanged) {
        duv_push_ref(p->cp->m_pManager->m_ctx, p->titlechanged);
        ZuiBuilderJs_pushControl(p->cp->m_pManager->m_ctx, p->cp);
        duk_push_string_w(p->cp->m_pManager->m_ctx, wkeGetStringW(title));
        if (duk_pcall_method(p->cp->m_pManager->m_ctx, 2)) {
            LOG_DUK(p->cp->m_pManager->m_ctx);
        }
        duk_pop(p->cp->m_pManager->m_ctx);
    }
    ZuiControlNotify(L"titlechanged", ((ZuiBrowser)param)->cp, wkeGetStringW(title), NULL, NULL);
}
// 回调：创建新的页面，比如说调用了 window.open 或者点击了 <a target="_blank" .../>
wkeWebView _staticOnCreateView(wkeWebView webWindow, void* param, int navType, void *url, void* features)
{
    ZuiControl bro = NULL;
    ZuiBrowser p = (ZuiBrowser)param;
    if (p->newwindow) {
        duv_push_ref(p->cp->m_pManager->m_ctx, p->newwindow);
        ZuiBuilderJs_pushControl(p->cp->m_pManager->m_ctx, p->cp);
        duk_push_uint(p->cp->m_pManager->m_ctx, navType);
        duk_push_string_w(p->cp->m_pManager->m_ctx, wkeGetStringW(url));
        if (duk_pcall_method(p->cp->m_pManager->m_ctx, 3)) {
            LOG_DUK(p->cp->m_pManager->m_ctx);
        }
        bro = ZuiBuilderJs_toControl(p->cp->m_pManager->m_ctx, -1);
        duk_pop(p->cp->m_pManager->m_ctx);
    }
    if (bro)
        return ZuiControlCall(Proc_Browser_GetView, bro, NULL, NULL, NULL);//重定向到当前页面
    return 0;
}
// 回调：url改变
wkeWebView _staticOnURLChanged(wkeWebView webView, void* param, void *url)
{
    ((ZuiBrowser)param)->url = ZuiWcsdup(wkeGetStringW(url));
    ZuiBrowser p = (ZuiBrowser)param;
    if (p->urlchanged) {
        duv_push_ref(p->cp->m_pManager->m_ctx, p->urlchanged);
        ZuiBuilderJs_pushControl(p->cp->m_pManager->m_ctx, p->cp);
        duk_push_string_w(p->cp->m_pManager->m_ctx, ((ZuiBrowser)param)->url);
        if (duk_pcall_method(p->cp->m_pManager->m_ctx, 2)) {
            LOG_DUK(p->cp->m_pManager->m_ctx);
        }
        duk_pop(p->cp->m_pManager->m_ctx);
    }
    
    ZuiControlNotify(L"urlchanged", ((ZuiBrowser)param)->cp, ((ZuiBrowser)param)->url, NULL, NULL);
}
// 回调：转跳
BOOL _staticOnNavigation(wkeWebView webView, void* param, int navigationType, void *url) {
    int ret = 0;
    ZuiBrowser p = (ZuiBrowser)param;
    if (p->navigation) {
        duv_push_ref(p->cp->m_pManager->m_ctx, p->navigation);
        ZuiBuilderJs_pushControl(p->cp->m_pManager->m_ctx, p->cp);
        duk_push_string_w(p->cp->m_pManager->m_ctx, wkeGetStringW(url));
        if (duk_pcall_method(p->cp->m_pManager->m_ctx, 2)) {
            LOG_DUK(p->cp->m_pManager->m_ctx);
        }
        ret = duk_to_boolean(p->cp->m_pManager->m_ctx, -1);
        duk_pop(p->cp->m_pManager->m_ctx);
    }
    if (ret)
        return ret;
    return !ZuiControlNotify(L"navigation", ((ZuiBrowser)param)->cp, wkeGetStringW(url), NULL, NULL);
}
ZEXPORT ZuiAny ZCALL ZuiBrowserProc(ZuiInt ProcId, ZuiControl cp, ZuiBrowser p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_OnSize: {
        wkeResize(p->view, (int)Param1, (int)Param2);
        if (cp->m_bVisible) {
            wkeRepaintIfNeeded(p->view);
        }
        break;
    }
    case Proc_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        switch (event->Type)
        {
        case ZEVENT_TIMER: {
            wkeRepaintIfNeeded(p->view);
            break;
        }
        case ZEVENT_SETFOCUS:
            ZuiPaintManagerSetImeStatus(cp->m_pManager, TRUE);
            if (p->view) wkeSetFocus(p->view);
            break;
        case ZEVENT_KILLFOCUS:
            ZuiPaintManagerSetImeStatus(cp->m_pManager, FALSE);
            if (p->view) wkeKillFocus(p->view);
            break;
        case ZEVENT_CHAR: {
            unsigned int charCode = event->wParam;
            unsigned int flags = 0;
            if (HIWORD(event->lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(event->lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            if (wkeFireKeyPressEvent(p->view, charCode, flags, FALSE))
                //return 0;
                break;
            break;
        }
        case ZEVENT_KEYUP: {
            unsigned int virtualKeyCode = event->wParam;
            unsigned int flags = 0;
            if (HIWORD(event->lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(event->lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            if (wkeFireKeyUpEvent(p->view, virtualKeyCode, flags, FALSE))
                return 0;
            break;
        }
        case ZEVENT_KEYDOWN: {
            unsigned int virtualKeyCode = event->wParam;
            unsigned int flags = 0;
            if (HIWORD(event->lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(event->lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            if (wkeFireKeyDownEvent(p->view, virtualKeyCode, flags, FALSE))
                return 0;
            break;
        }
        case ZEVENT_LBUTTONDOWN:
        case ZEVENT_LBUTTONUP:
        case ZEVENT_RBUTTONDOWN:
        case ZEVENT_LDBLCLICK:
        case ZEVENT_MOUSEMOVE: {
            UINT uMsg = 0;
            unsigned int flags = 0;
            if (event->Type == ZEVENT_LBUTTONDOWN)
            {
                ZuiPaintManagerSetCapture(cp->m_pManager);
            }
            else if (event->Type == ZEVENT_LBUTTONUP)
                ZuiPaintManagerReleaseCapture(cp->m_pManager);


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
            break;
        }
        case ZEVENT_SCROLLWHEEL: {
            POINT pt;
            int nFlag = GET_X_LPARAM(event->wParam);
            int delta = (nFlag == SB_LINEDOWN) ? -120 : 120;
            unsigned int flags = 0;
            pt.x = LOWORD(event->lParam);
            pt.y = HIWORD(event->lParam);
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
            if (wkeFireMouseWheelEvent(p->view, pt.x, pt.y, delta, flags))
                return 0;
            break;
        }
        case ZEVENT_SETCURSOR:
            //if (wkeFireWindowsMessage(p->view, cp->m_pManager->m_hWndPaint, WM_SETCURSOR, 0, 0, NULL))
                return 0;
            break;
        default:
            break;
        }
        break;
    }
    case Proc_OnPaint: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = (ZRect *)&cp->m_rcItem;
        if (p->init) {
            //ZGraphics sp;
            //sp.hdc = wkeGetViewDC(p->view);
            //sp.Width = rc->right - rc->left;
            //sp.Height = rc->bottom - rc->top;
            //ZuiAlphaBlend(gp, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, &sp, 0, 0, 255);
            BitBlt(gp->hdc, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, wkeGetViewDC(p->view), 0, 0, SRCCOPY);
        }
        break;
    }
    case Proc_SetPos: {
        //wkeSetHandleOffset(p->view, ((RECT*)Param1)->left, ((RECT*)Param1)->top);
        break;
    }
    case Proc_Browser_LoadUrl: {
        wkeLoadURLW(p->view, Param1);
        break;
    }
    case Proc_Browser_LoadHtml: {
        if (Param1)
        {
            wkeLoadHTMLW(p->view, Param1);
        }
        break;
    }
    case Proc_Browser_LoadFile: {
        if (Param1)
        {
            wkeLoadFileW(p->view, Param1);
        }
        break;
    }
    case Proc_Browser_GetView: {
        return p->view;
        break;
    }
    case Proc_SetAttribute: {

        if (wcscmp(Param1, L"url") == 0) ZuiControlCall(Proc_Browser_LoadUrl, cp, Param2, NULL, NULL);
        if (wcscmp(Param1, L"html") == 0) {
            ZuiControlCall(Proc_Browser_LoadHtml, cp, Param2, NULL, NULL);
        }
        break;
    }
    case Proc_Browser_RunJs: {
        if (Param1) {
            return wkeRunJSW(p->view, Param1);
        }
    }
    case Proc_Browser_jsToString: {
        void *es = wkeGlobalExec(p->view);
        if (es && Param1) {

            long long v = Param1;
            return jsToTempStringW(es, v);
        }
        return NULL;
        break;
    }
    case Proc_JsSet: {
        duk_context *ctx = (duk_context *)Param1;
        switch ((ZuiInt)Param2)
        {
        case Js_Id_Browser_titlechanged: {
            if (duk_is_function(ctx, 0)) {
                if (p->titlechanged)
                    duv_unref(ctx, p->titlechanged);
                duk_dup(ctx, 0);
                p->titlechanged = duv_ref(ctx);
            }
            return 0;
        }
        case Js_Id_Browser_newwindow: {
            if (duk_is_function(ctx, 0)) {
                if (p->newwindow)
                    duv_unref(ctx, p->newwindow);
                duk_dup(ctx, 0);
                p->newwindow = duv_ref(ctx);
            }
            return 0;
        }
        case Js_Id_Browser_urlchanged: {
            if (duk_is_function(ctx, 0)) {
                if (p->urlchanged)
                    duv_unref(ctx, p->urlchanged);
                duk_dup(ctx, 0);
                p->urlchanged = duv_ref(ctx);
            }
            return 0;
        }
        case Js_Id_Browser_navigation: {
            if (duk_is_function(ctx, 0)) {
                if (p->navigation)
                    duv_unref(ctx, p->navigation);
                duk_dup(ctx, 0);
                p->navigation = duv_ref(ctx);
            }
            return 0;
        }
        case Js_Id_Browser_url: {
            if (duk_is_string(ctx, 0)) {
                ZuiControlCall(Proc_Browser_LoadUrl, cp, (ZuiAny)duk_get_string_w(ctx, 0), NULL, NULL);
            }
            return 0;
        }
        default:
            break;
        }
        break;
    }
    case Proc_JsGet: {
        duk_context *ctx = (duk_context *)Param1;
        switch ((ZuiInt)Param2)
        {
        case Js_Id_Browser_url: {
            duk_push_string_w(ctx, p->url);
            return 1;
        }
        default:
            break;
        }
        break;
    }
    case Proc_JsInit: {
        ZuiBuilderControlInit(Param1, "titlechanged", Js_Id_Browser_titlechanged, TRUE);
        ZuiBuilderControlInit(Param1, "newwindow", Js_Id_Browser_newwindow, TRUE);
        ZuiBuilderControlInit(Param1, "urlchanged", Js_Id_Browser_urlchanged, TRUE);
        ZuiBuilderControlInit(Param1, "navigation", Js_Id_Browser_navigation, TRUE);
        ZuiBuilderControlInit(Param1, "url", Js_Id_Browser_url, TRUE);
        break;
    }
    case Proc_GetImePoint: {
        wkeRect caret = wkeGetCaretRect(p->view);
        ZPoint pt;
        pt.x = caret.x;
        pt.y = caret.y;
        return &pt;
        break;
    }
    case Proc_OnInit: {
        //wkeSetHandle(p->view, cp->m_pManager->m_hWndPaint);
        ZuiPaintManagerSetTimer(cp, 1000, 20);
        break;
    }
    case Proc_GetControlFlags: {
        return (ZuiAny)ZFLAG_SETCURSOR;
        break;
    }
    case Proc_OnCreate: {
        p = (ZuiBrowser)ZuiMalloc(sizeof(ZBrowser));
        memset(p, 0, sizeof(ZBrowser));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        p->old_call = cp->call;
        p->cp = cp;
        p->view = wkeCreateWebView();
        wkeOnPaintUpdated(p->view, _staticOnPaintUpdated, p);//界面刷新
        wkeOnTitleChanged(p->view, _staticOnTitleChanged, p);
        wkeOnCreateView(p->view, _staticOnCreateView, p);
        wkeOnURLChanged(p->view, _staticOnURLChanged, p);
        wkeOnNavigation(p->view, _staticOnNavigation, p);
        //wkeSetUserAgentW(p->view, L"Mozilla/5.0 (Linux; Android 4.4.2; Nexus 4 Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/34.0.1847.114 Mobile Safari/537.36");
        return p;
    }
    case Proc_OnDestroy: {
        wkeDestroyWebView(p->view);
        break;
    }
    case Proc_GetObject:
        if (Param1 == Type_Browser)
            return (ZuiAny)p;
        break;
    case Proc_GetType:
        return (ZuiAny)Type_Browser;
    case Proc_CoreInit: {
        HMODULE dll = LoadLibrary(L"wke.dll");
        if (!dll)
            return FALSE;
        wkeInitialize = (t_wkeInitialize)GetProcAddress(dll, "wkeInitialize");
        if (!wkeInitialize)
            goto LoadLibraryErro;
        wkeDestroyWebView = (t_wkeDestroyWebView)GetProcAddress(dll, "wkeDestroyWebView");
        if (!wkeDestroyWebView)
            goto LoadLibraryErro;
        wkeOnTitleChanged = (t_wkeOnTitleChanged)GetProcAddress(dll, "wkeOnTitleChanged");
        if (!wkeOnTitleChanged)
            goto LoadLibraryErro;
        wkeOnURLChanged = (t_wkeOnURLChanged)GetProcAddress(dll, "wkeOnURLChanged");
        if (!wkeOnURLChanged)
            goto LoadLibraryErro;
        wkeOnPaintUpdated = (t_wkeOnPaintUpdated)GetProcAddress(dll, "wkeOnPaintUpdated");
        if (!wkeOnPaintUpdated)
            goto LoadLibraryErro;
        wkeOnCreateView = (t_wkeOnCreateView)GetProcAddress(dll, "wkeOnCreateView");
        if (!wkeOnCreateView)
            goto LoadLibraryErro;
        wkeRepaintIfNeeded = (t_wkeRepaintIfNeeded)GetProcAddress(dll, "wkeRepaintIfNeeded");
        if (!wkeRepaintIfNeeded)
            goto LoadLibraryErro;
        wkeResize = (t_wkeResize)GetProcAddress(dll, "wkeResize");
        if (!wkeResize)
            goto LoadLibraryErro;
        wkeSetFocus = (t_wkeSetFocus)GetProcAddress(dll, "wkeSetFocus");
        if (!wkeSetFocus)
            goto LoadLibraryErro;
        wkeKillFocus = (t_wkeKillFocus)GetProcAddress(dll, "wkeKillFocus");
        if (!wkeKillFocus)
            goto LoadLibraryErro;
        wkeFireMouseWheelEvent = (t_wkeFireMouseWheelEvent)GetProcAddress(dll, "wkeFireMouseWheelEvent");
        if (!wkeFireMouseWheelEvent)
            goto LoadLibraryErro;
        wkeFireKeyUpEvent = (t_wkeFireKeyUpEvent)GetProcAddress(dll, "wkeFireKeyUpEvent");
        if (!wkeFireKeyUpEvent)
            goto LoadLibraryErro;
        wkeFireKeyDownEvent = (t_wkeFireKeyDownEvent)GetProcAddress(dll, "wkeFireKeyDownEvent");
        if (!wkeFireKeyDownEvent)
            goto LoadLibraryErro;
        wkeFireKeyPressEvent = (t_wkeFireKeyPressEvent)GetProcAddress(dll, "wkeFireKeyPressEvent");
        if (!wkeFireKeyPressEvent)
            goto LoadLibraryErro;
        wkeFireMouseEvent = (t_wkeFireMouseEvent)GetProcAddress(dll, "wkeFireMouseEvent");
        if (!wkeFireMouseEvent)
            goto LoadLibraryErro;
        //wkeFireWindowsMessage = (t_wkeFireWindowsMessage)GetProcAddress(dll, "wkeFireWindowsMessage");
        //if (!wkeFireWindowsMessage)
        //    goto LoadLibraryErro;
        wkeGetCaretRect = (t_wkeGetCaretRect)GetProcAddress(dll, "wkeGetCaretRect");
        if (!wkeGetCaretRect)
            goto LoadLibraryErro;
        wkeGetStringW = (t_wkeGetStringW)GetProcAddress(dll, "wkeGetStringW");
        if (!wkeGetStringW)
            goto LoadLibraryErro;
        wkeCreateWebView = (t_wkeCreateWebView)GetProcAddress(dll, "wkeCreateWebView");
        if (!wkeCreateWebView)
            goto LoadLibraryErro;
        wkeGetViewDC = (t_wkeGetViewDC)GetProcAddress(dll, "wkeGetViewDC");
        if (!wkeGetViewDC)
            goto LoadLibraryErro;
        //wkeSetHandle = (t_wkeSetHandle)GetProcAddress(dll, "wkeSetHandle");
        //if (!wkeSetHandle)
        //    goto LoadLibraryErro;
        //wkeSetHandleOffset = (t_wkeSetHandleOffset)GetProcAddress(dll, "wkeSetHandleOffset");
        //if (!wkeSetHandleOffset)
        //    goto LoadLibraryErro;
        wkeLoadURLW = (t_wkeLoadURLW)GetProcAddress(dll, "wkeLoadURLW");
        if (!wkeLoadURLW)
            goto LoadLibraryErro;
        wkeLoadHTMLW = (t_wkeLoadHTMLW)GetProcAddress(dll, "wkeLoadHTMLW");
        if (!wkeLoadHTMLW)
            goto LoadLibraryErro;
        //wkeOnLoadUrlBegin = (t_wkeOnLoadUrlBegin)GetProcAddress(dll, "wkeOnLoadUrlBegin");
        //if (!wkeOnLoadUrlBegin)
        //    goto LoadLibraryErro;
        wkeOnNavigation = (t_wkeOnNavigation)GetProcAddress(dll, "wkeOnNavigation");
        if (!wkeOnNavigation)
            goto LoadLibraryErro;
        wkeSetUserAgentW = (t_wkeSetUserAgentW)GetProcAddress(dll, "wkeSetUserAgentW");
        if (!wkeSetUserAgentW)
            goto LoadLibraryErro;
        wkeRunJSW = (t_wkeRunJSW)GetProcAddress(dll, "wkeRunJSW");
        if (!wkeRunJSW)
            goto LoadLibraryErro;
        wkeLoadFileW = (t_wkeLoadFileW)GetProcAddress(dll, "wkeLoadFileW");
        if (!wkeLoadFileW)
            goto LoadLibraryErro;
        //jsToTempStringW = (t_jsToTempStringW)GetProcAddress(dll, "jsToTempStringW");
        //if (!jsToTempStringW)
        //    goto LoadLibraryErro;
        wkeGlobalExec = (t_wkeGlobalExec)GetProcAddress(dll, "wkeGlobalExec");
        if (!wkeGlobalExec)
            goto LoadLibraryErro;

        wkeInitialize();
        return TRUE;
    LoadLibraryErro:
        FreeLibrary(dll);
        return FALSE;
    }
    case Proc_CoreUnInit:
        return (ZuiAny)TRUE;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
}




