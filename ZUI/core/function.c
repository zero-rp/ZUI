#include <uv.h>
#include <ZUI.h>
#include "dukluv\duv_module_core.h"
void Rect_Join(ZRect *rc, ZRect *rc1)
{
    if (rc1->left < rc->left) rc->left = rc1->left;
    if (rc1->top < rc->top) rc->top = rc1->top;
    if (rc1->right > rc->right) rc->right = rc1->right;
    if (rc1->bottom > rc->bottom) rc->bottom = rc1->bottom;
}

void * ZCALL Zui_Hash(wchar_t* str) {
    size_t hash = 0;
    wchar_t ch;
    for (long i = 0; ch = (size_t)*str++; i++)
    {
        if ((i & 1) == 0)
        {
            hash ^= ((hash << 7) ^ ch ^ (hash >> 3));
        }
        else
        {
            hash ^= (~((hash << 11) ^ ch ^ (hash >> 5)));
        }
    }
    return hash;
}

ZEXPORT ZuiBool ZCALL ZuiInit(ZuiInitConfig config) {
    if (config && config->m_hInstance) {
        m_hInstance = config->m_hInstance;
    }
#if RUN_DEBUG
    if (config && config->debug) {
        //启动调试器
        ZuiStartDebug();
    }
#endif
    /*初始化图形层*/
    if (!ZuiGraphInitialize()) {
        return FALSE;
    }
    /*初始化系统层*/
    if (!ZuiOsInitialize()) {
        return FALSE;
    }
    /*初始化全局变量*/
    if (!ZuiGlobalInit()) {
        return FALSE;
    }
    /*初始化绘制管理器*/
    if (!ZuiPaintManagerInitialize()) {
        return FALSE;
    }

    /*初始化模版管理器*/
    if (!ZuiTemplateInit())
    {
        return FALSE;
    }
    /*初始化导出接口*/
    if (!ZuiInitZuvFunc())
    {
        return FALSE;
    }
    /*初始化绑定器*/
    if (!ZuiBuilderInit())
    {
        return FALSE;
    }
    /*初始化资源池*/
    if (!ZuiResDBInit()) {
        return FALSE;
    }
    /*注册全局控件*/
    if (!ZuiControlRegister())
    {
        return FALSE;
    }
    return TRUE;
}
ZEXPORT ZuiBool ZCALL ZuiUnInit() {
    /*反初始化绘制管理器*/
    ZuiPaintManagerUnInitialize();
    /*反注册全局控件*/
    ZuiControlUnRegister();
    /*反初始化模版管理器*/
    ZuiTemplateUnInit();
    /*反初始化绑定器*/
    ZuiBuilderUnInit();
    /*反初始化资源池*/
    ZuiResDBUnInit();
    /*反初始化全局变量*/
    ZuiGlobalUnInit();
    /*反初始化系统层*/
    ZuiOsUnInitialize();
    /*反初始化图形层*/
    ZuiGraphUnInitialize();
    return TRUE;
}
ZEXPORT ZuiInt ZCALL ZuiMsgLoop() {
    //MSG Msg;
    //uv_loop_t *loop = uv_default_loop();
    //while (1) {
    //    int more = (0 != uv_run(loop, UV_RUN_NOWAIT));
    //    if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
    //        if (WM_QUIT == Msg.message) {
    //            break;
    //        }


    //        TranslateMessage(&Msg);
    //        DispatchMessageW(&Msg);
    //    }
    //    else {
    //        Sleep(1);
    //    }
    //}
    //return Msg.wParam;
    MSG Msg;
    uv_loop_t *loop = uv_default_loop();
    while (GetMessage(&Msg, NULL, 0, 0)) {
        if (WM_QUIT == Msg.message) {
            break;
        }
        uv_run(loop, UV_RUN_NOWAIT);
        TranslateMessage(&Msg);
        DispatchMessageW(&Msg);
    }
    return Msg.wParam;
}
ZEXPORT ZuiVoid ZCALL ZuiMsgLoop_exit() {
    PostQuitMessage(0);
}
ZuiControl MsgBox_pRoot;
ZuiAny ZCALL MsgBox_Notify_ctl(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    if (wcscmp(msg, L"onclick") == 0)
    {
        if (wcscmp(p->m_sName, L"WindowCtl_clos") == 0) {
            FreeZuiControl(p->m_pManager->m_pRoot, TRUE);
            PostMessage(0, WM_APP + 10, 0, 0);
        }
    }
    return 0;
}

ZEXPORT ZuiInt ZCALL ZuiMsgBox(ZuiControl rp,ZuiText text,ZuiText title) {
    ZuiControl p;
    MsgBox_pRoot = NewZuiControl(L"MessageBox", NULL, NULL, NULL);
    //取消最小化按钮
    p = ZuiControlFindName(MsgBox_pRoot, L"WindowCtl_min");
    ZuiControlCall(Proc_SetVisible, p, FALSE, NULL, NULL);
    //取消最大化按钮
    p = ZuiControlFindName(MsgBox_pRoot, L"WindowCtl_max");
    ZuiControlCall(Proc_SetVisible, p, FALSE, NULL, NULL);
    //挂接关闭按钮事件
    p = ZuiControlFindName(MsgBox_pRoot, L"WindowCtl_clos");
    ZuiControlRegNotify(p, MsgBox_Notify_ctl);
    //挂接确认按钮事件
    p = ZuiControlFindName(MsgBox_pRoot, L"ok");
    ZuiControlRegNotify(p, MsgBox_Notify_ctl);
    //挂接取消按钮事件
    p = ZuiControlFindName(MsgBox_pRoot, L"cancel");
    ZuiControlRegNotify(p, MsgBox_Notify_ctl);

    p = ZuiControlFindName(MsgBox_pRoot, L"text");
    ZuiControlCall(Proc_SetText, p, text, NULL, NULL);
    p = ZuiControlFindName(MsgBox_pRoot, L"title");
    ZuiControlCall(Proc_SetText, p, title, NULL, NULL);
    //禁用掉父窗口
    EnableWindow(rp->m_pManager->m_hWndPaint, FALSE);
    MSG Msg;
    while (1)
    {
        GetMessage(&Msg, NULL, 0, 0);
        if (Msg.message == WM_APP + 10)
            break;
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    //重新开启父窗口
    EnableWindow(rp->m_pManager->m_hWndPaint, TRUE);
    return;
}
ZEXPORT ZuiBool ZCALL ZuiIsPointInRect(ZuiRect Rect, ZuiPoint pt) {

    return PtInRect(Rect, *(POINT *)pt);
    /*
    if (pt->x <= Rect->Left) {
        return FALSE;
    }
    if (pt->x >= Rect->Width + Rect->Left) {
        return FALSE;
    }

    if (pt->y <= Rect->Top) {
        return FALSE;
    }
    if (pt->y >= Rect->Height + Rect->Top) {
        return FALSE;
    }

    return TRUE;
    */
}