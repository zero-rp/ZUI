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

typedef struct _ZWindows
{
    RB_ENTRY(_ZWindows) entry;
    uint32_t key;
    ZuiControl p;
}*ZuiWindows, ZWindows;
RB_HEAD(_ZWindows_Tree, _ZWindows);
static int ZWindows_Compare(struct _ZWindows *e1, struct _ZWindows *e2)
{
    return (e1->key < e2->key ? -1 : e1->key > e2->key);
}
RB_GENERATE_STATIC(_ZWindows_Tree, _ZWindows, entry, ZWindows_Compare);

static struct _ZWindows_Tree *m_window = NULL;
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
    case Proc_SetText: {
        return (ZuiAny)ZuiOsSetWindowTitle(p->m_osWindow, Param1);
    }
    case Proc_Window_SetWindowMin: {
        return (ZuiAny)ZuiOsSetWindowMin(p->m_osWindow);
    }
    case Proc_Window_SetWindowMax: {
        return (ZuiAny)ZuiOsSetWindowMax(p->m_osWindow);
    }
    case Proc_Window_SetWindowRestor: {
        return (ZuiAny)ZuiOsSetWindowRestor(p->m_osWindow);
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
        return (ZuiAny)(ZuiOsSetWindowSize(p->m_osWindow, Param1, Param2));
    }
    case Proc_Window_SetNoBox: {
        return (ZuiAny)(ZuiOsSetWindowNoBox(p->m_osWindow, Param1));
    }
    case Proc_Window_SetComBo: {
        return (ZuiAny)(ZuiOsSetWindowComBo(p->m_osWindow, Param1));
    }
    case Proc_Window_SetToolWindow: {
        return (ZuiAny)(ZuiOsSetWindowTool(p->m_osWindow, Param1));
    }
    case Proc_Window_Popup: {
        cp->m_bVisible = TRUE;
        ZuiOsWindowPopup(p->m_osWindow, Param1);
        return 0;
    }
    case Proc_Window_Center: {
        ZuiOsSetWindowCenter(p->m_osWindow);
        return 0;
    }
    case Proc_SetAttribute: {
        if (wcscmp(Param1, L"nobox") == 0) ZuiControlCall(Proc_Window_SetNoBox, cp, (ZuiAny)(wcscmp(Param2, L"true") == 0 ? TRUE : FALSE), NULL, NULL);
        else if (wcscmp(Param1, L"combo") == 0) ZuiControlCall(Proc_Window_SetComBo, cp, (ZuiAny)(wcscmp(Param2, L"true") == 0 ? TRUE : FALSE), NULL, NULL);
        else if (wcscmp(Param1, L"toolwindow") == 0) ZuiControlCall(Proc_Window_SetToolWindow, cp, (ZuiAny)(wcscmp(Param2, L"true") == 0 ? TRUE : FALSE), NULL, NULL);
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
            ZuiControlCall(Proc_Window_SetMinInfo, cp, (ZuiAny)cx, (ZuiAny)cy, NULL);
        }
        else if (wcscmp(Param1, L"maxinfo") == 0) {
            ZuiText pstr = NULL;
            int cx = wcstol(Param2, &pstr, 10);  ASSERT(pstr);
            int cy = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            ZuiControlCall(Proc_Window_SetMaxInfo, cp, (ZuiAny)cx, (ZuiAny)cy, NULL);
        }
        else if (wcscmp(Param1, L"size") == 0) {
            ZuiText pstr = NULL;
            int cx = wcstol(Param2, &pstr, 10);  ASSERT(pstr);
            int cy = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            ZuiControlCall(Proc_Window_SetSize, cp, (ZuiAny)cx, (ZuiAny)cy, NULL);
        }
        else if (wcscmp(Param1, L"name") == 0) {
            if (cp->m_sName) {
				if (wcscmp(cp->m_sName, Param2) != 0) {
					//删除以前的名字
					ZWindows theNode = { 0 };
					ZWindows *c;
					theNode.key = Zui_Hash(cp->m_sName);
					c = RB_FIND(_ZWindows_Tree, m_window, &theNode);
					if (c) {
						RB_REMOVE(_ZWindows_Tree, m_window, c);
						free(c);
					}
					free(cp->m_sName);
					cp->m_sName = NULL;
				}
				else {
					return 0;
				}
            }
                //保存现在的名字
                ZWindows *n = (ZWindows *)malloc(sizeof(ZWindows));
                memset(n, 0, sizeof(ZWindows));
                n->key = Zui_Hash(Param2);
                n->p = cp;
                RB_INSERT(_ZWindows_Tree, m_window, n);
                cp->m_sName = wcsdup(Param2);
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
        ZuiOsSetWindowVisible(p->m_osWindow, (ZuiBool)Param1);
        break;
    }
    case Proc_GetObject:
        if (Param1 == (ZuiAny)Type_Window)
            return (ZuiAny)p;
        break;
    case Proc_GetType: {
        return (ZuiAny)Type_Window;
    }
	case Proc_SetPos: {
		if (cp->m_pOs->m_bMax) {
			ZuiInt tmpwidth = cp->m_dwBorderWidth;
			cp->m_dwBorderWidth = 0;
			p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
			cp->m_dwBorderWidth = tmpwidth;
			return;
		}
		break;
	}
	case Proc_OnPaintBorder: {
		if (cp->m_pOs->m_bMax) return 0;
		break;
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
            p->m_osWindow = ZuiOsCreateWindow(cp, (ZuiBool)Param1);


            darray_append(m_window_array, cp);
            return p;
        }
        return NULL;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;
		if (ZuiControlNotify(_T("onclose"), cp, 0, 0, 0))
			return 0;

		if (cp->m_sName) {
			ZWindows theNode = { 0 };
			ZWindows *c;
			theNode.key = Zui_Hash(cp->m_sName);
			c = RB_FIND(_ZWindows_Tree, m_window, &theNode);
			if (c) {
				RB_REMOVE(_ZWindows_Tree, m_window, c);
				free(c);
			}
		}
        old_call(ProcId, cp, old_udata, Param1, Param2, Param3);

        ZuiOsDestroyWindow(p->m_osWindow);
		darray_delete(m_window_array,darray_find(m_window_array,cp));
        free(p);

        return 0;
    }
    case Proc_CoreInit: {
        m_window = (struct _ZWindows_Tree*)malloc(sizeof(struct _ZWindows_Tree));
        memset(m_window, 0, sizeof(struct _ZWindows_Tree));
        m_window_array = darray_create();
        return (ZuiAny)TRUE;
    }
    case Proc_CoreUnInit: {
        //这里销毁掉所有窗口
        struct _ZWindows * c = NULL;
        struct _ZWindows * cc = NULL;
        RB_FOREACH_SAFE(c, _ZWindows_Tree, m_window, cc) {
            RB_REMOVE(_ZWindows_Tree, m_window, c);
            free(c);
        }
        free(m_window);
        for (int i = 0; i < m_window_array->count; i++)
        {
            FreeZuiControl(m_window_array->data[i], TRUE);
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
    if (!Name)
        return NULL;
    ZWindows theNode = { 0 };
    ZWindows *node;
    theNode.key = Zui_Hash(Name);
    node = RB_FIND(_ZWindows_Tree, m_window, &theNode);
    if (!node)
        return NULL;
    return node->p;
}


