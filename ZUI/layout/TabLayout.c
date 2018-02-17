#include "TabLayout.h"
#include "Layout.h"
#include <core/control.h>
#include <core/builder.h>
#include <stdlib.h>

void* ZCALL ZuiTabLayoutProc(int ProcId, ZuiControl cp, ZuiTabLayout p, void* Param1, void* Param2, void* Param3) {
    switch (ProcId)
    {
    case Proc_SetPos: {
        ZuiDefaultControlProc(ProcId, cp, 0, Param1, Param2, Param3);
        ZRect rc = cp->m_rcItem;

        // Adjust for inset
        ZuiLayout op = (ZuiLayout)p->old_udata;
        rc.left += op->m_rcInset.left;
        rc.top += op->m_rcInset.top;
        rc.right -= op->m_rcInset.right;
        rc.bottom -= op->m_rcInset.bottom;
        rc.left += cp->m_dwBorderWidth;
        rc.top += cp->m_dwBorderWidth;
        rc.right -= cp->m_dwBorderWidth;
        rc.bottom -= cp->m_dwBorderWidth;

        for (int it = 0; it < darray_len(op->m_items); it++) {
            ZuiControl pControl = (ZuiControl)(op->m_items->data[it]);
            if (!pControl->m_bVisible) continue;
            if (pControl->m_bFloat) {
                ZuiControlCall(Proc_Layout_SetFloatPos, cp, (ZuiAny)it, 0, 0);
                continue;
            }

            if (it != p->m_iCurSel) continue;

            ZRect *rcPadding = (ZRect *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0));
            rc.left += rcPadding->left;
            rc.top += rcPadding->top;
            rc.right -= rcPadding->right;
            rc.bottom -= rcPadding->bottom;

            ZSize szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

            ZSize sz;
            ZSize * psz = (ZSize *)ZuiControlCall(Proc_EstimateSize, pControl, (void *)&szAvailable, 0, 0);
            sz.cx = psz->cx;
            sz.cy = psz->cy;
            if (sz.cx == 0) {
                sz.cx = MAX(0, szAvailable.cx);
            }
            if (sz.cx < (ZuiInt)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0)) sz.cx = (ZuiInt)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0);
            if (sz.cx > (ZuiInt)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0)) sz.cx = (ZuiInt)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0);

            if (sz.cy == 0) {
                sz.cy = MAX(0, szAvailable.cy);
            }
            if (sz.cy < (ZuiInt)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0)) sz.cy = (ZuiInt)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0);
            if (sz.cy > (ZuiInt)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0)) sz.cy = (ZuiInt)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0);
            {
                ZRect rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy };
                ZuiControlCall(Proc_SetPos, pControl, &rcCtrl, FALSE, 0);
            }
        }
        return 0;
    }
#if (defined HAVE_JS) && (HAVE_JS == 1)
    case Proc_JsSet: {
        duk_context *ctx = (duk_context *)Param1;
        switch ((ZuiInt)Param2)
        {
        case Js_Id_TabLayout_selectitem: {
            if (duk_is_number(ctx, 0)) {
                ZuiControlCall(Proc_TabLayout_SelectItem, cp, (ZuiAny)duk_to_int(ctx, 0), NULL, NULL);
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
        case Js_Id_TabLayout_selectitem: {
            duk_push_int(ctx, p->m_iCurSel);
            return 1;
        }
        default:
            break;
        }
        break;
    }
    case Proc_JsInit: {
        ZuiBuilderControlInit(Param1, "selectitem", Js_Id_TabLayout_selectitem, TRUE);
        break;
    }
#endif
    case Proc_Layout_Add: {
        ZuiBool ret = (ZuiBool)ZuiLayoutProc(Proc_Layout_Add, cp, p->old_udata, Param1, Param2, Param3);
        if (!ret)
            return (ZuiAny)ret;

        if (p->m_iCurSel == -1 && ((ZuiControl)(Param1))->m_bVisible)
        {
            p->m_iCurSel = (ZuiInt)ZuiLayoutProc(Proc_Layout_GetItemIndex, cp, p->old_udata, Param1, Param2, Param3);
        }
        else
        {
            ZuiControlCall(Proc_SetVisible, (ZuiControl)(Param1), FALSE, NULL, NULL);
        }

        return (ZuiAny)ret;
    }
    case Proc_Layout_AddAt: {
        ZuiBool ret = (ZuiBool)ZuiLayoutProc(Proc_Layout_AddAt, cp, p->old_udata, Param1, Param2, Param3);
        if (!ret)
            return (ZuiAny)ret;

        if (p->m_iCurSel == -1 && ((ZuiControl)(Param1))->m_bVisible)
        {
            p->m_iCurSel = (ZuiInt)ZuiLayoutProc(Proc_Layout_GetItemIndex, cp, p->old_udata, Param1, Param2, Param3);
        }
        else if (p->m_iCurSel != -1 && (ZuiInt)Param2 <= p->m_iCurSel)
        {
            p->m_iCurSel += 1;
        }
        else
        {
            ZuiControlCall(Proc_SetVisible, (ZuiControl)(Param1), FALSE, NULL, NULL);
        }

        return (ZuiAny)ret;
    }
    case Proc_Layout_Remove: {
        if (Param1 == NULL)
            return FALSE;

        int index = (int)ZuiControlCall(Proc_Layout_GetItemIndex, cp, Param1, Param2, Param3);
        //ZuiBool ret = (ZuiBool)ZuiLayoutProc(Proc_Layout_Remove, cp, p->old_udata, Param1, Param2, Param3);
        if (index == -1)
            return FALSE;
        FreeZuiControl((ZuiControl)Param1, FALSE);
        if (p->m_iCurSel == index)
        {
            if (ZuiControlCall(Proc_Layout_GetCount, cp, NULL, NULL, NULL) > 0)
            {
                p->m_iCurSel = 0;
                ZuiControlCall(Proc_SetVisible, (ZuiControl)ZuiControlCall(Proc_Layout_GetItemAt, cp, (ZuiAny)p->m_iCurSel, NULL, NULL), (ZuiAny)TRUE, NULL, NULL);
            }
            else
                p->m_iCurSel = -1;
            ZuiControlNeedParentUpdate(cp);
        }
        else if (p->m_iCurSel > index)
        {
            p->m_iCurSel -= 1;
        }

        return (ZuiAny)1;
    }
    case Proc_Layout_RemoveAll: {
        p->m_iCurSel = -1;
        ZuiLayoutProc(Proc_Layout_RemoveAll, cp, p->old_udata, Param1, Param2, Param3);
        ZuiControlNeedParentUpdate(cp);
        return 0;
    }
    case Proc_TabLayout_SelectItem: {
        ZuiInt iIndex = (ZuiInt)Param1;
        ZuiLayout op = (ZuiLayout)p->old_udata;
        if (iIndex < 0 || iIndex >= darray_len(op->m_items)) return FALSE;
        if (iIndex == p->m_iCurSel) return (ZuiAny)TRUE;

        int iOldSel = p->m_iCurSel;
        p->m_iCurSel = iIndex;
        for (int it = 0; it < darray_len(op->m_items); it++)
        {
            if (it == iIndex) {
                ZuiControlCall(Proc_SetVisible, (ZuiControl)ZuiControlCall(Proc_Layout_GetItemAt, cp, (ZuiAny)it, NULL, NULL), (ZuiAny)TRUE, NULL, NULL);
                ZuiControlCall(Proc_SetFocus, (ZuiControl)ZuiControlCall(Proc_Layout_GetItemAt, cp, (ZuiAny)it, NULL, NULL), NULL, NULL, NULL);
            }
            else ZuiControlCall(Proc_SetVisible, (ZuiControl)ZuiControlCall(Proc_Layout_GetItemAt, cp, (ZuiAny)it, NULL, NULL), (ZuiAny)FALSE, NULL, NULL);
        }
        ZuiControlNeedParentUpdate(cp);

        return (ZuiAny)TRUE;
    }
    case Proc_OnCreate: {
        p = (ZuiTabLayout)malloc(sizeof(ZTabLayout));
        memset(p, 0, sizeof(ZTabLayout));
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiLayoutProc;
        p->m_iCurSel = -1;
        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;

        free(p);

        return old_call(ProcId, cp, old_udata, Param1, Param2, Param3);
    }
    case Proc_GetType:
        return (ZuiAny)Type_TabLayout;
    case Proc_CoreInit:
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit:
        return NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}

