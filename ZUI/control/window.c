#include "Window.h"
#include <core/tree.h>
#include <core/carray.h>
#include <core/control.h>
#include <core/builder.h>
#include <core/function.h>
#include <layout/Layout.h>
#include <layout/VerticalLayout.h>
#if (defined HAVE_JS) && (HAVE_JS == 1)
#include <duktape.h>
#endif
static rb_root *m_window = NULL;
DArray *m_window_array = NULL;

ZEXPORT ZuiAny ZCALL ZuiWindowProc(ZuiInt ProcId, ZuiControl cp, ZuiWindow p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
#if (defined HAVE_JS) && (HAVE_JS == 1)
    case Proc_JsInit: {
        ZuiBuilderControlInit(Param1, "SetWindowMin", Js_Id_Window_SetWindowMin, FALSE);
        ZuiBuilderControlInit(Param1, "SetWindowMax", Js_Id_Window_SetWindowMax, FALSE);
        ZuiBuilderControlInit(Param1, "SetWindowRestor", Js_Id_Window_SetWindowRestor, FALSE);
        ZuiBuilderControlInit(Param1, "Popup", Js_Id_Window_Popup, FALSE);
        break;
    }
    case Proc_JsCall: {
        duk_context *ctx = (duk_context *)Param1;
        switch ((ZuiInt)Param2)
        {
        case Js_Id_Window_SetWindowMin: {
            ZuiControlCall(Proc_Window_SetWindowMin, cp, NULL, NULL, NULL);
            return 0;
        }
        case Js_Id_Window_SetWindowMax: {
            ZuiControlCall(Proc_Window_SetWindowMax, cp, NULL, NULL, NULL);
            return 0;
        }
        case Js_Id_Window_SetWindowRestor: {
            ZuiControlCall(Proc_Window_SetWindowRestor, cp, NULL, NULL, NULL);
            return 0;
        }
        case Js_Id_Window_Popup: {
            //        if (js_gettop(Param2) == 3) {
            //            ZPoint pt = { js_toint32(Param2,1),js_toint32(Param2,2) };
            //            ZuiControlCall(Proc_Window_Popup, cp, &pt, NULL, NULL);
            //        }
            //        else
            //            ZuiControlCall(Proc_Window_Popup, cp, NULL, NULL, NULL);
            return 0;
        }
        default:
            break;
        }
        break;
    }
#endif
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
        return ZuiOsSetWindowTitle(p->m_osWindow, Param1);
    }
    case Proc_Window_SetWindowMin: {
        return ZuiOsSetWindowMin(p->m_osWindow);
    }
    case Proc_Window_SetWindowMax: {
        return ZuiOsSetWindowMax(p->m_osWindow);
    }
    case Proc_Window_SetWindowRestor: {
        return ZuiOsSetWindowRestor(p->m_osWindow);
    }
    case Proc_Window_SetMinInfo: {
        cp->m_pOs->m_szMinWindow.cx = Param1;
        cp->m_pOs->m_szMinWindow.cy = Param2;
        break;
    }
    case Proc_Window_SetMaxInfo: {
        cp->m_pOs->m_szMaxWindow.cx = Param1;
        cp->m_pOs->m_szMaxWindow.cy = Param2;
        break;
    }
    case Proc_Window_SetSize: {
        return ZuiOsSetWindowSize(p->m_osWindow, Param1, Param2);
    }
    case Proc_Window_SetNoBox: {
        return ZuiOsSetWindowNoBox(p->m_osWindow, Param1);
    }
    case Proc_Window_SetComBo: {
        return ZuiOsSetWindowComBo(p->m_osWindow, Param1);
    }
    case Proc_Window_SetToolWindow: {
        return ZuiOsSetWindowTool(p->m_osWindow, Param1);
    }
    case Proc_Window_Popup: {
        cp->m_bVisible = TRUE;
        ZuiOsWindowPopup(p->m_osWindow, Param1);
        return;
    }
    case Proc_Window_Center: {
        ZuiOsSetWindowCenter(p->m_osWindow);
        return;
    }
    case Proc_SetAttribute: {
        if (wcscmp(Param1, L"nobox") == 0) ZuiControlCall(Proc_Window_SetNoBox, cp, wcscmp(Param2, L"true") == 0 ? TRUE : FALSE, NULL, NULL);
        else if (wcscmp(Param1, L"combo") == 0) ZuiControlCall(Proc_Window_SetComBo, cp, wcscmp(Param2, L"true") == 0 ? TRUE : FALSE, NULL, NULL);
        else if (wcscmp(Param1, L"toolwindow") == 0) ZuiControlCall(Proc_Window_SetToolWindow, cp, wcscmp(Param2, L"true") == 0 ? TRUE : FALSE, NULL, NULL);
        else if (wcscmp(Param1, L"layered") == 0) {
            //if (wcscmp(Param2, L"true") == 0) {
            //    ZuiPaintManagerSetLayered(p->m_pm, TRUE);
            //}
            //else {
            //    ZuiPaintManagerSetLayered(p->m_pm, FALSE);
           //}
        }
        //else if (wcscmp(Param1, L"opacity") == 0) ZuiPaintManagerSetLayeredOpacity(p->m_pm, _wtoi(Param2));
        else if (wcscmp(Param1, L"mininfo") == 0) {
            ZuiText pstr = NULL;
            int cx = wcstol(Param2, &pstr, 10);  ASSERT(pstr);
            int cy = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            ZuiControlCall(Proc_Window_SetMinInfo, cp, cx, cy, NULL);
        }
        else if (wcscmp(Param1, L"maxinfo") == 0) {
            ZuiText pstr = NULL;
            int cx = wcstol(Param2, &pstr, 10);  ASSERT(pstr);
            int cy = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            ZuiControlCall(Proc_Window_SetMaxInfo, cp, cx, cy, NULL);
        }
        else if (wcscmp(Param1, L"size") == 0) {
            ZuiText pstr = NULL;
            int cx = wcstol(Param2, &pstr, 10);  ASSERT(pstr);
            int cy = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            ZuiControlCall(Proc_Window_SetSize, cp, cx, cy, NULL);
        }
        else if (wcscmp(Param1, L"name") == 0) {
            if (cp->m_sName)
                rb_delete(Zui_Hash(cp->m_sName), m_window);//删除以前的名字
            rb_insert(Zui_Hash(Param2), cp, m_window);//保存现在的名字
        }
        else if (wcscmp(Param1, L"center") == 0) {
            if (wcscmp(Param2, L"true") == 0) {
                ZuiControlCall(Proc_Window_Center, cp, NULL, NULL, NULL);
            }
        }
        else if (wcscmp(Param1, L"desktop") == 0) {
            //嵌入桌面
            //if (wcscmp(Param2, L"true") == 0) {
            //    SetParent(p->m_hWnd, findDesktopIconWnd());
            //}
            //else
            //{
            //    SetParent(p->m_hWnd, NULL);
            //}
        }
        break;
    }
    case Proc_SetVisible: {
        if (cp->m_bVisible == (BOOL)Param1)
            return 0;
        ZuiOsSetWindowVisible(p->m_osWindow, Param1);
        break;
    }
    case Proc_GetObject:
        if (Param1 == Type_Window)
            return (ZuiAny)p;
        break;
    case Proc_GetType: {
        return Type_Window;
    }
    case Proc_OnCreate: {
        p = (ZuiWindow)malloc(sizeof(ZWindow));
        if (p)
        {
            memset(p, 0, sizeof(ZWindow));
            //保存原来的回调地址,创建成功后回调地址指向当前函数
            //创建继承的控件 保存数据指针
            p->old_udata = ZuiVerticalLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
            p->old_call = (ZCtlProc)&ZuiVerticalLayoutProc;

            //创建宿主窗口
            p->m_osWindow = ZuiOsCreateWindow(cp, Param1);
            
            
            darray_append(m_window_array, cp);
            return p;
        }
        return NULL;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;


        old_call(ProcId, cp, old_udata, Param1, Param2, Param3);

        ZuiOsDestroyWindow(p->m_osWindow);
        free(p);
        return 0;
    }
    case Proc_CoreInit: {
        m_window = rb_new();
        m_window_array = darray_create();
        return TRUE;
    }
    case Proc_CoreUnInit: {
        //这里销毁掉所有窗口
        rb_free(m_window);
        for (size_t i = 0; i < m_window_array->count; i++)
        {
            FreeZuiControl(m_window_array->data[i], FALSE);
        }
        darray_destroy(m_window_array);
        return NULL;
    }
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}

ZEXPORT ZuiControl ZCALL ZuiWindowFindName(ZuiText Name) {
    rb_node*node;
    if (!Name)
        return NULL;
    node = rb_search(Zui_Hash(Name), m_window);
    if (!node)
        return NULL;
    return (ZuiControl)(node->data);
}


