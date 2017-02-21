#include <ZUI.h>

void* CALLBACK ZuiTabLayoutProc(int ProcId, ZuiControl cp, ZuiTabLayout p, void* Param1, void* Param2, void* Param3) {
    switch (ProcId)
    {
    case Proc_CoreInit:
        return TRUE;
    case Proc_OnCreate: {
        p = (ZuiTabLayout)ZuiMalloc(sizeof(ZTabLayout));
        memset(p, 0, sizeof(ZTabLayout));
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiLayoutProc;
        p->m_iCurSel = -1;
        return p;
        break;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;

        ZuiFree(p);

        return old_call(ProcId, cp, old_udata, Param1, Param2, Param3);
        break;
    }
    case Proc_SetPos: {
        ZuiDefaultControlProc(ProcId, cp, 0, Param1, Param2, Param3);
        RECT rc = cp->m_rcItem;

        // Adjust for inset
        ZuiLayout op = (ZuiLayout)p->old_udata;
        rc.left += op->m_rcInset.left;
        rc.top += op->m_rcInset.top;
        rc.right -= op->m_rcInset.right;
        rc.bottom -= op->m_rcInset.bottom;

        for (int it = 0; it < darray_len(op->m_items); it++) {
            ZuiControl pControl = (ZuiControl)(op->m_items->data[it]);
            if (!pControl->m_bVisible) continue;
            if (pControl->m_bFloat) {
                ZuiControlCall(Proc_Layout_SetFloatPos, cp, (void *)it, 0, 0);
                continue;
            }

            if (it != p->m_iCurSel) continue;

            RECT *rcPadding = (RECT *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0));
            rc.left += rcPadding->left;
            rc.top += rcPadding->top;
            rc.right -= rcPadding->right;
            rc.bottom -= rcPadding->bottom;

            SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

            SIZE sz;
            SIZE * psz = (SIZE *)ZuiControlCall(Proc_EstimateSize, pControl, (void *)&szAvailable, 0, 0);
            sz.cx = psz->cx;
            sz.cy = psz->cy;
            if (sz.cx == 0) {
                sz.cx = MAX(0, szAvailable.cx);
            }
            if (sz.cx < (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0);
            if (sz.cx > (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0);

            if (sz.cy == 0) {
                sz.cy = MAX(0, szAvailable.cy);
            }
            if (sz.cy < (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0)) sz.cy = (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0);
			if (sz.cy >(LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0)) sz.cy = (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0);
			{
				RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy };
				ZuiControlCall(Proc_SetPos, pControl, &rcCtrl, FALSE, 0);
			}
        }
        return 0;
        break;
    }
    case Proc_Layout_Add: {
        BOOL ret = (BOOL)ZuiLayoutProc(Proc_Layout_Add, cp, p->old_udata, Param1, Param2, Param3);
        if (!ret)
            return ret;

        if (p->m_iCurSel == -1 && ((ZuiControl)(Param1))->m_bVisible)
        {
            p->m_iCurSel = ZuiLayoutProc(Proc_Layout_GetItemIndex, cp, p->old_udata, Param1, Param2, Param3);
        }
        else
        {
            ZuiControlCall(Proc_SetVisible, (ZuiControl)(Param1), FALSE, NULL, NULL);
        }

        return ret;
        break;
    }
    case Proc_Layout_AddAt: {
        BOOL ret = (BOOL)ZuiLayoutProc(Proc_Layout_AddAt, cp, p->old_udata, Param1, Param2, Param3);
        if (!ret)
            return ret;

        if (p->m_iCurSel == -1 && ((ZuiControl)(Param1))->m_bVisible)
        {
            p->m_iCurSel = ZuiLayoutProc(Proc_Layout_GetItemIndex, cp, p->old_udata, Param1, Param2, Param3);
        }
        else if (p->m_iCurSel != -1 && (int)Param2 <= p->m_iCurSel)
        {
            p->m_iCurSel += 1;
        }
        else
        {
            ZuiControlCall(Proc_SetVisible, (ZuiControl)(Param1), FALSE, NULL, NULL);
        }

        return ret;
        break;
    }
    case Proc_Layout_Remove: {
        if (Param1 == NULL)
            return FALSE;

        int index = (int)ZuiControlCall(Proc_Layout_GetItemIndex, cp, Param1, Param2, Param3);
        BOOL ret = (BOOL)ZuiLayoutProc(Proc_Layout_Remove, cp, p->old_udata, Param1, Param2, Param3);
        if (!ret)
            return FALSE;

        if (p->m_iCurSel == index)
        {
            if (ZuiControlCall(Proc_Layout_GetCount, cp, NULL, NULL, NULL) > 0)
            {
                p->m_iCurSel = 0;
                ZuiControlCall(Proc_SetVisible, (ZuiControl)ZuiControlCall(Proc_Layout_GetItemAt, cp, p->m_iCurSel, NULL, NULL), TRUE, NULL, NULL);
            }
            else
                p->m_iCurSel = -1;
            ZuiControlNeedParentUpdate(cp);
        }
        else if (p->m_iCurSel > index)
        {
            p->m_iCurSel -= 1;
        }

        return ret;
        break;
    }
    case Proc_Layout_RemoveAll: {
        p->m_iCurSel = -1;
        ZuiLayoutProc(Proc_Layout_RemoveAll, cp, p->old_udata, Param1, Param2, Param3);
        ZuiControlNeedParentUpdate(cp);
        return 0;
        break;
    }
    case Proc_Layout_Tab_SelectItem: {
        ZuiInt iIndex = (ZuiInt)Param1;
        ZuiLayout op = (ZuiLayout)p->old_udata;
        if (iIndex < 0 || iIndex >= darray_len(op->m_items)) return FALSE;
        if (iIndex == p->m_iCurSel) return TRUE;

        int iOldSel = p->m_iCurSel;
        p->m_iCurSel = iIndex;
        for (int it = 0; it < darray_len(op->m_items); it++)
        {
            if (it == iIndex) {
                ZuiControlCall(Proc_SetVisible, (ZuiControl)ZuiControlCall(Proc_Layout_GetItemAt, cp, (ZuiAny)it, NULL, NULL), TRUE, NULL, NULL);
                ZuiControlCall(Proc_SetFocus, (ZuiControl)ZuiControlCall(Proc_Layout_GetItemAt, cp, (ZuiAny)it, NULL, NULL), NULL, NULL, NULL);
            }
            else ZuiControlCall(Proc_SetVisible, (ZuiControl)ZuiControlCall(Proc_Layout_GetItemAt, cp, (ZuiAny)it, NULL, NULL), FALSE, NULL, NULL);
        }
        ZuiControlNeedParentUpdate(cp);

        return (ZuiAny)TRUE;
        break;
    }
    case Proc_JsPut: {
        js_State *J = Param2;
        if (wcscmp(Param1, L"selectitem") == 0) ZuiControlCall(Proc_Layout_Tab_SelectItem, cp, (ZuiAny)js_toint32(J, -1), NULL, NULL);
        break;
    }
    case Proc_CoreUnInit: {
        return NULL;
        break;
    }
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}

