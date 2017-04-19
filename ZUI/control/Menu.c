#include <ZUI.h>



ZEXPORT ZuiControl ZCALL ZuiLayoutLoadNodeMenu(mxml_node_t *tree, ZuiControl win) {
    mxml_node_t *node;
    ZuiText ClassName = NULL;
    ZuiBool Visible = FALSE, Enabled = TRUE;
    ZuiControl Control;
    for (node = mxmlFindElement(tree, tree, NULL, NULL, NULL, MXML_DESCEND); node != NULL; node = mxmlWalkNext(node, NULL, MXML_DESCEND)/*node = mxmlFindElement(node, tree, NULL,NULL,NULL,MXML_DESCEND)*/) {
        {
        LoadNodeBedin:
            ClassName = node->value.name;
            LOG_DEGUB(L"layout创建控件: 类名:%ls\r\n", ClassName);
            if (wcscmp(ClassName, L"Template") == 0) {//模版类
                ZuiAddTemplate(node);
                node = node->next;
                if (node) {
                    ClassName = node->value.name;
                    goto LoadNodeBedin;
                }
                else
                    continue;
            }
            if (wcscmp(ClassName, L"Menu") == 0) {//菜单类
                ZuiAddMenu(node, node->parent->user_data);
                node = node->next;
                if (node) {
                    ClassName = node->value.name;
                    goto LoadNodeBedin;
                }
                else
                    continue;
            }
            if (wcscmp(ClassName, L"Include") == 0) {//包含文件
                ZuiText src = NULL;
                for (size_t i = 0; i < node->value.num_attrs; i++)
                {
                    if (wcscmp(node->value.attrs[i].name, L"src") == 0) {
                        src = node->value.attrs[i].value;
                    }
                }
                ZuiRes res = ZuiResDBGetRes(src, NULL);
                if (res) {
                    mxml_node_t *new_node = ZuiLayoutLoad(res->p, res->plen);
                    mxmlAdd(node->parent ? node->parent : node, MXML_ADD_BEFORE, node, new_node);
                    ZuiResDBDelRes(res);
                }
            }
#if (defined HAVE_JS) && (HAVE_JS == 1)
            else if (wcscmp(ClassName, L"LoadScript") == 0) {
                ZuiText src = NULL;
                for (size_t i = 0; i < node->value.num_attrs; i++)
                {
                    if (wcscmp(node->value.attrs[i].name, L"src") == 0) {
                        src = node->value.attrs[i].value;
                    }
                }
                ZuiBuilderJsLoad(win->m_pManager->m_ctx, src);
            }
#endif
            else if (!node->user_data) {//当前节点还未创建
                Control = NewZuiControl(ClassName, NULL, NULL, NULL);
                if (node->parent->user_data && wcsicmp(ClassName, L"Menu") != 0) {
                    //上级控件已存在且当前欲创建的子窗口不为窗口对象
                    if (Control) {
                        node->user_data = Control;//保存控件到节点
                        /*添加到容器*/
                        ZuiControlCall(Proc_Layout_Add, node->parent->user_data, Control, NULL, NULL);
                    }
                    else {
                        /*当前控件创建失败 子控件肯定创建不了 删除节点*/
                        mxmlDelete(node);
                        /*再次从头处理*/
                        node = tree;
                        continue;//窗口创建失败就没必要继续下去了
                    }
                }
                else if (!node->parent->user_data && wcsicmp(ClassName, L"Menu") == 0) {
                    //上级控件已存在且当前欲创建的子窗口为窗口对象
                    if (Control) {
                        node->user_data = Control;//保存控件到节点
                        win = Control;
                    }
                    else
                        break;//窗口创建失败就没必要继续下去了
                }
                /*解析属性*/
                for (size_t i = 0; i < node->value.num_attrs; i++)
                {
                    ZuiControlCall(Proc_SetAttribute, Control, node->value.attrs[i].name, node->value.attrs[i].value, NULL);
                }
            }
        }
    }
    return win;
}



static LRESULT WINAPI __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ZuiMenu pThis = NULL;
    if (uMsg == WM_NCCREATE) {

        LPCREATESTRUCT lpcs = (LPCREATESTRUCT)(lParam);
        pThis = (ZuiMenu)(lpcs->lpCreateParams);
        pThis->m_hWnd = hWnd;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LPARAM)pThis);
    }
    else {
        pThis = (ZuiMenu)GetWindowLongPtr(hWnd, GWLP_USERDATA);
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
            if (uMsg == WM_MOUSEACTIVATE)
            {
                return MA_NOACTIVATE;
            }
            else if (uMsg == WM_DESTROY)
            {
                FreeCPaintManagerUI(pThis->m_pm);
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

ZEXPORT ZuiAny ZCALL ZuiMenuProc(ZuiInt ProcId, ZuiControl cp, ZuiMenu p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_SetAttribute: {
        if (wcscmp(Param1, L"layered") == 0) {
            if (wcscmp(Param2, L"true") == 0) {
                ZuiPaintManagerSetLayered(p->m_pm, TRUE);
            }
            else {
                ZuiPaintManagerSetLayered(p->m_pm, FALSE);
            }
        }
        else if (wcscmp(Param1, L"opacity") == 0) ZuiPaintManagerSetLayeredOpacity(p->m_pm, _wtoi(Param2));
        else if (wcscmp(Param1, L"size") == 0) {
            LPTSTR pstr = NULL;
            int cx = wcstol(Param2, &pstr, 10); ASSERT(pstr);
            int cy = wcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZuiControlCall(Proc_Window_SetSize, cp, cx, cy, NULL);
        }
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
    case Proc_Window_SetSize: {
        ZuiControlCall(Proc_SetFixedWidth, cp, Param1, NULL, NULL);
        ZuiControlCall(Proc_SetFixedHeight, cp, Param2, NULL, NULL);
        SetWindowPos(p->m_hWnd, NULL, 0, 0, Param1, Param2, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
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
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;

        p->m_pm->m_pRoot = NULL;

        old_call(ProcId, cp, old_udata, Param1, Param2, Param3);

        DestroyWindow(p->m_hWnd);
        ZuiFree(p);
        return;
    }
    case Proc_OnCreate: {
        p = (ZuiWindow)ZuiMalloc(sizeof(ZWindow));
        memset(p, 0, sizeof(ZWindow));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiVerticalLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiVerticalLayoutProc;

        //创建宿主窗口
        //创建绘制管理器
        p->m_pm = NewCPaintManagerUI();
        p->m_pm->m_bUnfocusPaintWindow = TRUE;//菜单窗口为无焦点窗口
        p->m_OldWndProc = DefWindowProc;
        p->root = cp;
        p->m_hWnd = CreateWindowEx(WS_EX_NOACTIVATE | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, L"ZUI_MENU", L"", WS_POPUP | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, GetModuleHandleA(NULL), p);

        ZuiPaintManagerInit(p->m_pm, p->m_hWnd);
        ZuiPaintManagerAttachDialog(p->m_pm, cp);
        if (!Param1)
            ShowWindow(p->m_hWnd, SW_SHOWNOACTIVATE);
        return p;
    }
    case Proc_GetObject:
        if (Param1 == Type_Menu)
            return (ZuiAny)p;
        break;
    case Proc_GetType:
        return (ZuiAny)Type_Menu;
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
        wc.lpszClassName = L"ZUI_MENU";
        RegisterClass(&wc);
        //将辅助控件注册到系统
        ZuiControlRegisterAdd(L"menuitem", (ZCtlProc)&ZuiMenuItemProc);
        return (ZuiAny)TRUE;
    }
    case Proc_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}
ZuiDestroyMenu(ZuiMenu p) {
    ZuiMenu next;
    if (!p)
        return;
    next = p;
    while (next)
    {
        if (!next->m_aSubMenu)
            break;
        next = next->m_aSubMenu;
    }
    while (next)
    {
        if (next == p)
            break;
        ZuiMenu old = next;
        next = next->m_aParentMenu;
        FreeZuiControl(old->root, TRUE);
    }
    FreeZuiControl(p->root, TRUE);
    if (p->m_aParentMenu)
        p->m_aParentMenu->m_aSubMenu = NULL;
    else
        p->bExit = TRUE;
}
ZEXPORT ZuiAny ZCALL ZuiMenuItemProc(ZuiInt ProcId, ZuiControl cp, ZuiMenuItem p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        switch (event->Type)
        {
        case ZEVENT_MOUSELEAVE: {
            return 0;
        }
        case ZEVENT_MOUSEENTER: {
            ZuiDestroyMenu(((ZuiMenu)cp->m_pParent->m_sUserData)->m_aSubMenu);//销毁掉上一个子菜单
            if (p->node)
            {
                mxml_node_t *xnode = mxmlClone(p->node, NULL);//先把节点克隆出来

                ZuiControl win = NewZuiControl(L"menu", NULL, NULL, NULL);
                xnode->user_data = win;
                for (size_t i = 0; i < xnode->value.num_attrs; i++)
                {
                    ZuiControlCall(Proc_SetAttribute, win, xnode->value.attrs[i].name, xnode->value.attrs[i].value, NULL);
                }

                ZuiLayoutLoadNodeMenu(xnode, win);
                mxmlDelete(xnode);//释放克隆出来的节点

                ZuiMenu np = win->m_sUserData;
                ((ZuiMenu)cp->m_pParent->m_sUserData)->m_aSubMenu = np;
                np->m_aParentMenu = (ZuiMenu)cp->m_pParent->m_sUserData;
                ZPoint pt = { cp->m_rcItem.right ,cp->m_rcItem.top };
                ZuiClientToScreen(cp, &pt);
                SetWindowPos(np->m_hWnd, NULL, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
            }

            break;
        }
        default:
            break;
        }
        break;
    }
    case Proc_SetAttribute: {
        break;
    }
    case Proc_OnCreate: {
        p = (ZuiMenuItem)ZuiMalloc(sizeof(ZMenuItem));
        memset(p, 0, sizeof(ZMenuItem));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiVerticalLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiVerticalLayoutProc;

        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;

        if (p->node)
            mxmlDelete(p->node);//释放克隆出来的节点
        ZuiFree(p);

        return old_call(ProcId, cp, old_udata, Param1, Param2, Param3);
    }
    case Proc_GetObject:
        if (Param1 == Type_MenuItem)
            return (ZuiAny)p;
        break;
    case Proc_GetType:
        return (ZuiAny)Type_MenuItem;
    case Proc_CoreInit:
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}

ZuiMenu ZuiAddMenu(mxml_node_t *node, ZuiControl win) {
    if (win) {
        //子菜单
        ZuiMenuItem p = win->m_sUserData;
        p->node = mxmlClone(node, NULL);
    }
    else {
        ZuiText classname = NULL;
        /*解析属性*/
        for (size_t i = 0; i < node->value.num_attrs; i++)
        {
            if (wcscmp(node->value.attrs[i].name, L"name") == 0)
            {
                classname = node->value.attrs[i].value;
            }
        }
        if (classname)
        {
            mxml_node_t *new_node = mxmlClone(node, NULL);
            if (new_node)
            {
                wcslwr(classname);
                rb_insert((key_t)Zui_Hash(classname), new_node, Global_MenuClass);
            }
        }
    }
    return;
}
ZuiBool ZuiIsMenuWnd(ZuiMenu p, HWND hwnd) {
    ZuiMenu next = p;
    while (next)
    {
        if (next->m_hWnd == hwnd)
            return TRUE;
        next = next->m_aSubMenu;
    }

    return FALSE;
}

ZEXPORT ZuiVoid ZCALL ZuiPopupMenu(ZuiPaintManager mp, ZuiText name, ZuiPoint pt) {
    if (name) {
        rb_node *node = rb_search((key_t)Zui_Hash(name), Global_MenuClass);
        if (node) {
            mxml_node_t *xnode = mxmlClone((mxml_node_t *)node->data, NULL);//先把节点克隆出来

            ZuiControl win = NewZuiControl(L"menu", NULL, NULL, NULL);
            xnode->user_data = win;
            for (size_t i = 0; i < xnode->value.num_attrs; i++)
            {
                ZuiControlCall(Proc_SetAttribute, win, xnode->value.attrs[i].name, xnode->value.attrs[i].value, NULL);
            }

            ZuiLayoutLoadNodeMenu(xnode, win);
            mxmlDelete(xnode);//释放克隆出来的节点

            ZuiMenu p = win->m_sUserData;
            SetWindowPos(p->m_hWnd, NULL, pt->x, pt->y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);

            int nRet = -1;
            BOOL bMenuDestroyed = FALSE;
            BOOL bMsgQuit = FALSE;
            while (TRUE)
            {
                if (p->bExit)
                {
                    nRet = 0;
                    break;
                }
                HWND A = GetForegroundWindow();
                if (GetForegroundWindow() != mp->m_hWndPaint)
                {
                    break;
                }
                BOOL bInterceptOther = FALSE;
                MSG msg = { 0 };
                if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                {
                    if (msg.message == WM_KEYDOWN
                        || msg.message == WM_SYSKEYDOWN
                        || msg.message == WM_KEYUP
                        || msg.message == WM_SYSKEYUP
                        || msg.message == WM_CHAR
                        || msg.message == WM_IME_CHAR)
                    {
                        //transfer the message to menu window
                        //if (m_menus->IsKeyEvent())
                        //{
                        // SKILL_ASSERT(m_pKeyEvent->GetMenuWnd()->GetHWND());
                        // msg.hwnd = m_pKeyEvent->GetMenuWnd()->GetHWND();
                        //}
                    }
                    else if (msg.message == WM_LBUTTONDOWN
                        || msg.message == WM_RBUTTONDOWN
                        || msg.message == WM_NCLBUTTONDOWN
                        || msg.message == WM_NCRBUTTONDOWN
                        || msg.message == WM_LBUTTONDBLCLK)
                    {
                        //click on other window
                        if (!ZuiIsMenuWnd(p, msg.hwnd))
                        {
                            ZuiDestroyMenu(p);
                            //为了和菜单再次的弹出消息同步
                            //这里会导致父窗口被拖动
                            //PostMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
                            bInterceptOther = TRUE;
                            bMenuDestroyed = TRUE;
                        }
                    }
                    else if (msg.message == WM_LBUTTONUP
                        || msg.message == WM_RBUTTONUP
                        || msg.message == WM_NCLBUTTONUP
                        || msg.message == WM_NCRBUTTONUP
                        || msg.message == WM_CONTEXTMENU)
                    {
                        if (!ZuiIsMenuWnd(p, msg.hwnd))
                        {
                            //防止菜单同时弹出多个
                            PostMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
                            break;
                        }
                    }
                    else if (msg.message == WM_QUIT)
                    {
                        bMsgQuit = TRUE;
                    }

                    //拦截非菜单窗口的MouseMove消息
                    if (msg.message == WM_MOUSEMOVE)
                    {
                        if (!ZuiIsMenuWnd(p, msg.hwnd))
                        {
                            bInterceptOther = TRUE;
                        }
                    }

                    if (!bInterceptOther)
                    {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                    }
                }
                else
                {
                    MsgWaitForMultipleObjects(0, 0, 0, 10, QS_ALLINPUT);
                }

                if (bMenuDestroyed)
                    break;

                if (bMsgQuit)
                {
                    PostQuitMessage(msg.wParam);
                    break;
                }
            }

            if (!bMenuDestroyed)
            {
                ZuiDestroyMenu(p);
            }
            return nRet;
        }
    }
}