#include <ZUI.h>




void* CALLBACK ZuiHorizontalLayoutProc(int ProcId, ZuiControl cp, ZuiHorizontalLayout p, void* Param1, void* Param2, void* Param3) {
    switch (ProcId)
    {
    case Proc_CoreInit:
        return TRUE;
    case Proc_OnCreate: {
        p = (ZuiHorizontalLayout)malloc(sizeof(ZHorizontalLayout));
        memset(p, 0, sizeof(ZHorizontalLayout));
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiLayoutProc;

        return p;
        break;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;

        free(p);

        return old_call(ProcId, cp, old_udata, Param1, Param2, Param3);
        break;
    }
    case Proc_SetPos: {
        ZuiDefaultControlProc(ProcId, cp, 0, Param1, Param2, Param3);
        RECT rc = cp->m_rcItem;
        ZuiLayout op = (ZuiLayout)p->old_udata;
        // Adjust for inset
        rc.left += op->m_rcInset.left;
        rc.top += op->m_rcInset.top;
        rc.right -= op->m_rcInset.right;
        rc.bottom -= op->m_rcInset.bottom;
        if (darray_len(op->m_items) == 0) {
            ZuiControlCall(Proc_Layout_ProcessScrollBar, cp, &rc, 0, 0);
            return 0;
        }

        if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible) rc.right -= (ZuiInt)ZuiControlCall(Proc_GetFixedWidth, op->m_pVerticalScrollBar, NULL, NULL, NULL);
        if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible) rc.bottom -= (ZuiInt)ZuiControlCall(Proc_GetFixedHeight, op->m_pHorizontalScrollBar, NULL, NULL, NULL);

        // Determine the minimum size
        SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
        if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible)
            szAvailable.cx += (ZuiInt)ZuiControlCall(Proc_ScrollBar_GetScrollRange, op->m_pHorizontalScrollBar, NULL, NULL, NULL);

        int nAdjustables = 0;
        int cxFixed = 0;
        int nEstimateNum = 0;
        //第一轮计算得到各种信息，不做实际布局处理 
        for (int it1 = 0; it1 < darray_len(op->m_items); it1++) {
            ZuiControl pControl = (ZuiControl)(op->m_items->data[it1]);
            if (!pControl->m_bVisible) continue;
            if (pControl->m_bFloat) continue;
            SIZE sz;
            SIZE * psz = (SIZE *)ZuiControlCall(Proc_EstimateSize, pControl, (void *)&szAvailable, 0, 0);
            sz.cx = psz->cx;
            sz.cy = psz->cy;
            if (sz.cx == 0) {
                //记录需要自动计算宽度的子控件的数量
                nAdjustables++;
            }
            else {
                if (sz.cx < (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0);
                if (sz.cx > (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0);
            }
            cxFixed += sz.cx + ((RECT *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0)))->left + ((RECT *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0)))->right;
            //记录需要做相对布局的子控件的数量 
            nEstimateNum++;
        }
        //cxFixed保存了所有相对布局的控件占用的宽度（包括了padding属性好childpadding属性占用的宽度）
        cxFixed += (nEstimateNum - 1) * op->m_iChildPadding;

        // Place elements
        int cxNeeded = 0;
        int cxExpand = 0;
        //cxExpand保存需要自动计算宽度的子控件的宽度
        int cyNeeded = 0;
        if (nAdjustables > 0) cxExpand = MAX(0, (szAvailable.cx - cxFixed) / nAdjustables);
        // Position the elements
        //szRemaining保存除已被布局的子控件以外的剩余空间
        SIZE szRemaining = szAvailable;

        int iPosX = rc.left;
        if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible) {
            iPosX -= (ZuiInt)ZuiControlCall(Proc_ScrollBar_GetScrollPos, op->m_pHorizontalScrollBar, NULL, NULL, NULL);
        }
        int iAdjustable = 0;
        //cxFixedRemaining记录当前还未被布局过的所有子控件的总宽度
        int cxFixedRemaining = cxFixed;
        for (int it2 = 0; it2 < darray_len(op->m_items); it2++) {
            ZuiControl pControl = (ZuiControl)(op->m_items->data[it2]);
            if (!pControl->m_bVisible)
                continue;
            if (pControl->m_bFloat) {
                ZuiControlCall(Proc_Layout_SetFloatPos, cp, (void *)it2, 0, 0);
                continue;
            }

            RECT *rcPadding = (RECT *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0));
            szRemaining.cx -= rcPadding->left;
            SIZE sz;
            SIZE * psz = (SIZE *)ZuiControlCall(Proc_EstimateSize, pControl, (void *)&szRemaining, 0, 0);
            sz.cx = psz->cx;
            sz.cy = psz->cy;
            if (sz.cx == 0) {
                iAdjustable++;
                sz.cx = cxExpand;
                // 这里判断如果是最后一个需要自动计算宽度的元素则做出不同的处理
                if (iAdjustable == nAdjustables) {
                    sz.cx = MAX(0, szRemaining.cx - rcPadding->right - cxFixedRemaining);
                }
                if (sz.cx < (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0);
                if (sz.cx > (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0);
            }
            else {
                if (sz.cx < (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0);
                if (sz.cx > (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0);
                cxFixedRemaining -= sz.cx + rcPadding->left + rcPadding->right;
            }
            cxFixedRemaining -= op->m_iChildPadding;

            sz.cy = (LONG)ZuiControlCall(Proc_GetFixedHeight, pControl, 0, 0, 0);
            if (sz.cy == 0) sz.cy = rc.bottom - rc.top - rcPadding->top - rcPadding->bottom;
            if (sz.cy < 0) sz.cy = 0;
            if (sz.cy < (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0)) sz.cy = (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0);
            if (sz.cy > (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0)) sz.cy = (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0);
            RECT rcCtrl = { iPosX + rcPadding->left, rc.top + rcPadding->top, iPosX + sz.cx + rcPadding->left , rc.top + rcPadding->top + sz.cy };
            ZuiControlCall(Proc_SetPos, pControl, &rcCtrl, FALSE, 0);

            iPosX += sz.cx + op->m_iChildPadding + rcPadding->left + rcPadding->right;
            cxNeeded += sz.cx + rcPadding->left + rcPadding->right;
            szRemaining.cx -= sz.cx + op->m_iChildPadding + rcPadding->right;

            int tmp = sz.cy + rcPadding->top + rcPadding->bottom;
            cyNeeded = (tmp > cyNeeded) ? tmp : cyNeeded;
        }
        cxNeeded += (nEstimateNum - 1) * op->m_iChildPadding;
        cyNeeded += (nEstimateNum - 1) * op->m_iChildPadding;
        if (op->m_pHorizontalScrollBar != NULL) {
            if (cxNeeded > rc.right - rc.left) {
                if (op->m_pHorizontalScrollBar->m_bVisible) {
                    ZuiControlCall(Proc_ScrollBar_SetScrollRange, op->m_pHorizontalScrollBar, cxNeeded - (rc.right - rc.left), NULL, NULL);
                }
                else {
                    ZuiControlCall(Proc_SetVisible, op->m_pHorizontalScrollBar, TRUE, NULL, NULL);
                    ZuiControlCall(Proc_ScrollBar_SetScrollRange, op->m_pHorizontalScrollBar, cxNeeded - (rc.right - rc.left), NULL, NULL);
                    ZuiControlCall(Proc_ScrollBar_SetScrollPos, op->m_pHorizontalScrollBar, 0, NULL, NULL);
                    rc.bottom -= (LONG)ZuiControlCall(Proc_GetFixedHeight, op->m_pHorizontalScrollBar, 0, 0, 0);
                }
            }
            else {
                if (op->m_pHorizontalScrollBar->m_bVisible) {
                    ZuiControlCall(Proc_SetVisible, op->m_pHorizontalScrollBar, FALSE, NULL, NULL);
                    ZuiControlCall(Proc_ScrollBar_SetScrollRange, op->m_pHorizontalScrollBar, 0, NULL, NULL);
                    ZuiControlCall(Proc_ScrollBar_SetScrollPos, op->m_pHorizontalScrollBar, 0, NULL, NULL);
                    rc.bottom += (LONG)ZuiControlCall(Proc_GetFixedHeight, op->m_pHorizontalScrollBar, 0, 0, 0);
                }
            }
        }
        ZuiControlCall(Proc_Layout_ProcessScrollBar, cp, &rc, cxNeeded, cyNeeded);
        return 0;
        break;
    }
    case Proc_OnPaint: {


        break;
    }
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}

