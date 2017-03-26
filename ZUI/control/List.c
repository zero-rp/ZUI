#include <ZUI.h>


ZEXPORT ZuiAny ZCALL ZuiListProc(ZuiInt ProcId, ZuiControl cp, ZuiList p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {

    case Proc_SetPos: {
        RECT rc;
        //拿到父组件数据
        ZuiLayout op = (ZuiLayout)((ZuiVerticalLayout)p->old_udata)->old_udata;
        //通知父容器调整布局
        ZuiVerticalLayoutProc(ProcId, cp, p->old_udata, Param1, Param2, Param3);

        if (p->m_pHeader != NULL) { // 设置header各子元素x坐标,因为有些listitem的setpos
            //int iLeft = rc.left + m_rcInset.left;
            //int iRight = rc.right - m_rcInset.right;

            //m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);

            //if (!m_pHeader->IsVisible()) {
            //    for (int it = m_pHeader->GetCount() - 1; it >= 0; it--) {
            //        static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(true);
            //    }
            //}
            //m_pHeader->SetPos(CDuiRect(iLeft, 0, iRight, 0), false);
            //int iOffset = m_pList->GetScrollPos().cx;
            //for (int i = 0; i < m_ListInfo.nColumns; i++) {
            //    CControlUI* pControl = static_cast<CControlUI*>(m_pHeader->GetItemAt(i));
            //    if (!pControl->IsVisible()) continue;
            //    if (pControl->IsFloat()) continue;

            //    RECT rcPos = pControl->GetPos();
            //    if (iOffset > 0) {
            //        rcPos.left -= iOffset;
            //        rcPos.right -= iOffset;
            //        pControl->SetPos(rcPos, false);
            //    }
            //    m_ListInfo.rcColumn[i] = pControl->GetPos();
            //}
            //if (!m_pHeader->IsVisible()) {
            //    for (int it = m_pHeader->GetCount() - 1; it >= 0; it--) {
            //        static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(false);
            //    }
            //    m_pHeader->SetInternVisible(false);
            //}
        }


        if (p->m_pHeader == NULL) return;

        rc = cp->m_rcItem;
        rc.left += op->m_rcInset.left;
        rc.top += op->m_rcInset.top;
        rc.right -= op->m_rcInset.right;
        rc.bottom -= op->m_rcInset.bottom;

        if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible) {
            rc.top -= (ZuiInt)ZuiControlCall(Proc_ScrollBar_GetScrollPos, op->m_pVerticalScrollBar, NULL, NULL, NULL);
            rc.bottom -= (ZuiInt)ZuiControlCall(Proc_ScrollBar_GetScrollPos, op->m_pVerticalScrollBar, NULL, NULL, NULL);
            rc.bottom += (ZuiInt)ZuiControlCall(Proc_ScrollBar_GetScrollRange, op->m_pVerticalScrollBar, NULL, NULL, NULL);
            rc.right -= (ZuiInt)ZuiControlCall(Proc_GetFixedHeight, op->m_pVerticalScrollBar, NULL, NULL, NULL);
        }
        if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible) {
            rc.left -= (ZuiInt)ZuiControlCall(Proc_ScrollBar_GetScrollPos, op->m_pHorizontalScrollBar, NULL, NULL, NULL);
            rc.right -= (ZuiInt)ZuiControlCall(Proc_ScrollBar_GetScrollPos, op->m_pHorizontalScrollBar, NULL, NULL, NULL);
            rc.right += (ZuiInt)ZuiControlCall(Proc_ScrollBar_GetScrollRange, op->m_pHorizontalScrollBar, NULL, NULL, NULL);
            rc.bottom -= (ZuiInt)ZuiControlCall(Proc_GetFixedHeight, op->m_pHorizontalScrollBar, NULL, NULL, NULL);
        }
        
        //计算列数量
        p->m_ListInfo.nColumns = MIN(ZuiControlCall(Proc_Layout_GetCount, p->m_pHeader, NULL, NULL, NULL), LIST_MAX_COLUMNS);

        //表头为不可示状态
        //if (!p->m_pHeader->m_bVisible) {
        //    for (int it = m_pHeader->GetCount() - 1; it >= 0; it--) {
        //        static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(true);
        //    }
        //    m_pHeader->SetPos(CDuiRect(rc.left, 0, rc.right, 0), false);
        //}
        //得到列表偏移
        int iOffset = ((SIZE*)ZuiControlCall(Proc_Layout_GetScrollPos, p->m_pList, NULL, NULL, NULL))->cx;
        for (int i = 0; i < p->m_ListInfo.nColumns; i++) {
            ZuiControl pControl = ZuiControlCall(Proc_Layout_GetItemAt, p->m_pHeader, i, NULL, NULL);
            if (!pControl->m_bVisible) continue;
            if (pControl->m_bFloat) continue;
            RECT rcPos = *(RECT *)ZuiControlCall(Proc_GetPos, pControl, NULL, NULL, NULL);
            
            if (iOffset > 0) {
                rcPos.left -= iOffset;
                rcPos.right -= iOffset;
                ZuiControlCall(Proc_SetPos, pControl, &rcPos, FALSE, 0);
            }
            p->m_ListInfo.rcColumn[i] = *(RECT *)ZuiControlCall(Proc_GetPos, pControl, NULL, NULL, NULL);
        }
        //if (!m_pHeader->IsVisible()) {
        //    for (int it = m_pHeader->GetCount() - 1; it >= 0; it--) {
        //        static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(false);
        //    }
        //    m_pHeader->SetInternVisible(false);
        //}

        break;
    }
    case Proc_SetAttribute: {
        if (wcscmp(Param1, _T("header")) == 0) ZuiControlCall(Proc_SetVisible, p, wcscmp(Param2, L"hidden") == 0 ? FALSE : TRUE, NULL, NULL);
        break;
    }
    case Proc_List_SetScrollPos: {
        ZuiControlCall(Proc_ListBody_SetScrollPos, p->m_pList, Param1, NULL, NULL);
    }
    case Proc_List_GetHeader: {
        return p->m_pHeader;
    }
    case Proc_List_GetListInfo: {
        return &p->m_ListInfo;
    }
    case Proc_List_GetVerticalScrollBar: {
        return ZuiControlCall(Proc_Layout_GetVerticalScrollBar, p->m_pList, NULL, NULL, NULL);
    }
    case Proc_List_GetHorizontalScrollBar: {
        return ZuiControlCall(Proc_Layout_GetHorizontalScrollBar, p->m_pList, NULL, NULL, NULL);
    }
    case Proc_List_EnableScrollBar: {
        return ZuiControlCall(Proc_Layout_EnableScrollBar, p->m_pList, Param1, Param2, NULL);
    }
    case Proc_List_Add: {
        //判断添加的元素是否是表头控件
        if (ZuiControlCall(Proc_GetType, Param1, NULL, NULL, NULL) == Type_ListHeader) {
            if (p->m_pHeader != Param1 && ZuiControlCall(Proc_Layout_GetCount, p->m_pHeader, NULL, NULL, NULL) == 0) {
                //删除原来的头元素
                ZuiVerticalLayoutProc(Proc_Layout_Remove, cp, p->old_udata, p->m_pHeader, TRUE, NULL);
                FreeZuiControl(p->m_pHeader, NULL);
                p->m_pHeader = Param1;
                p->m_ListInfo.nColumns = MIN(ZuiControlCall(Proc_Layout_GetCount, p->m_pHeader, NULL, NULL, NULL), LIST_MAX_COLUMNS);
                return ZuiVerticalLayoutProc(Proc_Layout_AddAt, cp, p->old_udata, p->m_pHeader, 0, NULL);
            }
            return FALSE;
        }
        //判断元素是否是表头元素
        if (ZuiControlCall(Proc_GetType, Param1, NULL, NULL, NULL) == Type_ListHeaderItem) {
            //插入到头容器
            ZuiBool ret = ZuiControlCall(Proc_Layout_Add, p->m_pHeader, Param1, 0, NULL);
            //计算列数量
            p->m_ListInfo.nColumns = MIN(ZuiControlCall(Proc_Layout_GetCount, p->m_pHeader, NULL, NULL, NULL), LIST_MAX_COLUMNS);
            return ret;
        }
        // 插入的元素是行数据
        if (ZuiControlCall(Proc_GetObject, Param1, Type_ListElement, NULL, NULL)) {
            ZuiControlCall(Proc_ListElement_SetOwner, Param1, cp, NULL, NULL);
            ZuiControlCall(Proc_ListElement_SetIndex, Param1, ZuiControlCall(Proc_List_GetCount, p->m_pHeader, NULL, NULL, NULL), NULL, NULL);
        }
        
        return ZuiControlCall(Proc_Layout_Add, p->m_pList, Param1, 0, NULL);
    }
    case Proc_List_AddAt: {
        //判断添加的元素是否是表头控件
        if (ZuiControlCall(Proc_GetType, Param1, NULL, NULL, NULL) == Type_ListHeader) {
            if (p->m_pHeader != Param1 && ZuiControlCall(Proc_Layout_GetCount, p->m_pHeader, NULL, NULL, NULL) == 0) {
                //删除原来的头元素
                ZuiVerticalLayoutProc(Proc_Layout_Remove, cp, p->old_udata, p->m_pHeader, NULL, NULL, NULL);
                p->m_pHeader = Param1;
                p->m_ListInfo.nColumns = MIN(ZuiControlCall(Proc_Layout_GetCount, p->m_pHeader, NULL, NULL, NULL), LIST_MAX_COLUMNS);
                //表头永远在第一个位置
                return ZuiVerticalLayoutProc(Proc_Layout_AddAt, cp, p->old_udata, p->m_pHeader, 0, NULL);
            }
            return FALSE;
        }
        //判断元素是否是表头元素
        if (ZuiControlCall(Proc_GetType, Param1, NULL, NULL, NULL) == Type_ListHeaderItem) {
            //插入到头容器指定位置
            ZuiBool ret = ZuiControlCall(Proc_Layout_AddAt, p->m_pHeader, Param1, Param2, NULL);
            //计算列数量
            p->m_ListInfo.nColumns = MIN(ZuiControlCall(Proc_Layout_GetCount, p->m_pHeader, NULL, NULL, NULL), LIST_MAX_COLUMNS);
            return ret;
        }
        // 插入的元素是行数据
        if (!ZuiControlCall(Proc_Layout_AddAt, p->m_pList, Param1, Param2, NULL)) return FALSE;

        // 设置初始化当前控件
        if (ZuiControlCall(Proc_GetObject, Param1, Type_ListElement, NULL, NULL)) {
            ZuiControlCall(Proc_ListElement_SetOwner, Param1, cp, NULL, NULL);
            ZuiControlCall(Proc_ListElement_SetIndex, Param1, ZuiControlCall(Proc_List_GetCount, p->m_pHeader, NULL, NULL, NULL), NULL, NULL);
        }

        //for (int i = iIndex + 1; i < m_pList->GetCount(); ++i) {
        //    CControlUI* p = m_pList->GetItemAt(i);
        //    pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
        //    if (pListItem != NULL) {
        //        pListItem->SetIndex(i);
        //    }
        //}
        //if (m_iCurSel >= iIndex) m_iCurSel += 1;
        return TRUE;
    }
    case Proc_List_Remove: {
        //判断删除的元素是否是表头控件
        if (ZuiControlCall(Proc_GetType, Param1, NULL, NULL, NULL) == Type_ListHeader)
            return ZuiVerticalLayoutProc(Proc_Layout_Remove, cp, p->old_udata, Param1, NULL, NULL);
        //判断删除的元素是否是表头元素
        if (ZuiControlCall(Proc_GetType, Param1, NULL, NULL, NULL) == Type_ListHeaderItem)
            return ZuiControlCall(Proc_Layout_Remove, p->m_pHeader, Param1, NULL, NULL);
        // 删除的元素是行数据
        int iIndex = ZuiControlCall(Proc_Layout_GetItemIndex, p->m_pList, Param1, NULL, NULL);
        if (iIndex == -1) return FALSE;

        if (!ZuiControlCall(Proc_Layout_RemoveAt, p->m_pList, Param1, iIndex, NULL))
            return FALSE;

        //for (int i = iIndex; i < m_pList->GetCount(); ++i) {
        //    CControlUI* p = m_pList->GetItemAt(i);
        //    IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
        //    if (pListItem != NULL) {
        //        pListItem->SetIndex(i);
        //    }
        //}

        //if (iIndex == m_iCurSel && m_iCurSel >= 0) {
        //    int iSel = m_iCurSel;
        //    m_iCurSel = -1;
        //    SelectItem(FindSelectable(iSel, false));
        //}
        //else if (iIndex < m_iCurSel) m_iCurSel -= 1;
        return TRUE;
    }
    case Proc_List_RemoveAt: {
        //此调用只删除表数据,不影响表头
        if (!ZuiControlCall(Proc_Layout_RemoveAt, p->m_pList, Param1, NULL, NULL))
            return FALSE;

        //for (int i = iIndex; i < m_pList->GetCount(); ++i) {
        //    CControlUI* p = m_pList->GetItemAt(i);
        //    IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
        //    if (pListItem != NULL) pListItem->SetIndex(i);
        //}

        //if (iIndex == m_iCurSel && m_iCurSel >= 0) {
        //    int iSel = m_iCurSel;
        //    m_iCurSel = -1;
        //    SelectItem(FindSelectable(iSel, false));
        //}
        //else if (iIndex < m_iCurSel) m_iCurSel -= 1;
        return TRUE;
    }
    case Proc_List_RemoveAll: {
        p->m_iCurSel = -1;
        p->m_iExpandedItem = -1;
        return ZuiControlCall(Proc_Layout_RemoveAll, p->m_pList, NULL, NULL, NULL);
    }
    case Proc_OnCreate: {
        p = (ZuiList)ZuiMalloc(sizeof(ZList));
        memset(p, 0, sizeof(ZList));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiVerticalLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiVerticalLayoutProc;
        //创建表头
        p->m_pHeader = NewZuiControl(L"ListHeader", NULL, NULL, NULL);
        ZuiVerticalLayoutProc(Proc_Layout_Add, cp, p->old_udata, p->m_pHeader, NULL, NULL);
        ZuiControlCall(Proc_SetBkColor, p->m_pHeader, ARGB(255, 111, 222, 200), NULL, NULL);
        //创建表主体
        p->m_pList = NewZuiControl(L"ListBody", NULL, NULL, NULL);
        ZuiVerticalLayoutProc(Proc_Layout_Add, cp, p->old_udata, p->m_pList, NULL, NULL);
        ZuiControlCall(Proc_ListBody_SetOwner, p->m_pList, cp, NULL, NULL);

        p->m_ListInfo.dwLineColor = 0xFF004488;
        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;
        ZuiFree(p);

        return old_call(ProcId, cp, old_udata, Param1, Param2, Param3);
    }
    case Proc_GetObject: {
        if (Param1 == Type_List)
            return (ZuiAny)p;
        break;
    }
    case Proc_GetType:
        return (ZuiAny)Type_List;
    case Proc_CoreInit:
        //将辅助控件注册到系统
        ZuiControlRegisterAdd(L"listbody", (ZCtlProc)&ZuiListBodyProc);
        ZuiControlRegisterAdd(L"listheader", (ZCtlProc)&ZuiListHeaderProc);
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit:
        return (ZuiAny)TRUE;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}
ZEXPORT ZuiAny ZCALL ZuiListBodyProc(ZuiInt ProcId, ZuiControl cp, ZuiListBody p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_SetPos: {
        ZuiDefaultControlProc(ProcId, cp, 0, Param1, Param2, Param3);
        RECT rc = *(RECT*)Param1;
        ZuiLayout op = ZuiControlCall(Proc_GetObject, cp, Type_Layout, NULL, NULL);
        // Adjust for inset
        rc.left += op->m_rcInset.left;
        rc.top += op->m_rcInset.top;
        rc.right -= op->m_rcInset.right;
        rc.bottom -= op->m_rcInset.bottom;
        if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible)
            rc.right -= (ZuiInt)ZuiControlCall(Proc_GetFixedWidth, op->m_pVerticalScrollBar, NULL, NULL, NULL);
        if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible)
            rc.bottom -= (ZuiInt)ZuiControlCall(Proc_GetFixedHeight, op->m_pHorizontalScrollBar, NULL, NULL, NULL);

        // Determine the minimum size
        SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
        if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible)
            szAvailable.cx += (ZuiInt)ZuiControlCall(Proc_ScrollBar_GetScrollRange, op->m_pHorizontalScrollBar, NULL, NULL, NULL);


        int cxNeeded = 0;
        int nAdjustables = 0;
        int cyFixed = 0;
        int nEstimateNum = 0;
        for (int it1 = 0; it1 < darray_len(op->m_items); it1++) {
            ZuiControl pControl = (ZuiControl)(op->m_items->data[it1]);
            if (!pControl->m_bVisible) continue;
            if (pControl->m_bFloat) continue;
            SIZE sz;
            SIZE * psz = (SIZE *)ZuiControlCall(Proc_EstimateSize, pControl, (void *)&szAvailable, 0, 0);
            sz.cx = psz->cx;
            sz.cy = psz->cy;
            if (sz.cy == 0) {
                nAdjustables++;
            }
            else {
                if (sz.cy < (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0))
                    sz.cy = (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0);
                if (sz.cy > (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0))
                    sz.cy = (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0);
            }
            cyFixed += sz.cy + ((RECT *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0)))->top + ((RECT *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0)))->bottom;

            RECT *rcPadding = (RECT *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0));
            sz.cx = MAX(sz.cx, 0);
            if (sz.cx < (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0)) { sz.cx = (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0); }
            if (sz.cx > (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0)) { sz.cx = (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0); }

            cxNeeded = MAX(cxNeeded, sz.cx);
            nEstimateNum++;
        }
        cyFixed += (nEstimateNum - 1) * op->m_iChildPadding;

        if (p->m_pOwner) {
            //得到表头控件
            ZuiControl pHeader = ZuiControlCall(Proc_List_GetHeader, p->m_pOwner, cp, NULL, NULL);
            if (pHeader != NULL && ZuiControlCall(Proc_Layout_GetCount, pHeader, NULL, NULL, NULL) > 0) {
                SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };
                cxNeeded = MAX(0, ((SIZE *)ZuiControlCall(Proc_EstimateSize, pHeader, (void *)&sz, 0, 0))->cx);
            }
        }

        // Place elements
        int cyNeeded = 0;
        int cyExpand = 0;
        if (nAdjustables > 0) cyExpand = MAX(0, (szAvailable.cy - cyFixed) / nAdjustables);
        // Position the elements
        SIZE szRemaining = szAvailable;
        int iPosY = rc.top;
        if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible) {
            iPosY -= (LONG)ZuiControlCall(Proc_ScrollBar_GetScrollPos, op->m_pVerticalScrollBar, NULL, NULL, NULL);
        }
        int iPosX = rc.left;
        if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible) {
            iPosX -= (LONG)ZuiControlCall(Proc_ScrollBar_GetScrollPos, op->m_pHorizontalScrollBar, NULL, NULL, NULL);
        }
        int iAdjustable = 0;
        int cyFixedRemaining = cyFixed;
        for (int it2 = 0; it2 < darray_len(op->m_items); it2++) {
            ZuiControl pControl = (ZuiControl)(op->m_items->data[it2]);
            if (!pControl->m_bVisible) continue;
            if (pControl->m_bFloat) {
                ZuiControlCall(Proc_Layout_SetFloatPos, cp, (void *)it2, 0, 0);
                continue;
            }

            RECT *rcPadding = (RECT *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0));
            szRemaining.cy -= rcPadding->top;
            SIZE sz;
            SIZE * psz = (SIZE *)ZuiControlCall(Proc_EstimateSize, pControl, (void *)&szRemaining, 0, 0);
            sz.cx = psz->cx;
            sz.cy = psz->cy;
            if (sz.cy == 0) {
                iAdjustable++;
                sz.cy = cyExpand;
                // Distribute remaining to last element (usually round-off left-overs)
                if (iAdjustable == nAdjustables) {
                    sz.cy = MAX(0, szRemaining.cy - rcPadding->bottom - cyFixedRemaining);
                }
                if (sz.cy < (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0)) { sz.cy = (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0); }
                if (sz.cy > (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0)) { sz.cy = (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0); }

            }
            else {
                if (sz.cy < (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0)) { sz.cy = (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0); }
                if (sz.cy > (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0)) { sz.cy = (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0); }
                cyFixedRemaining -= sz.cy;
            }

            sz.cx = MAX(cxNeeded, szAvailable.cx - rcPadding->left - rcPadding->right);

            if (sz.cx < (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0);
            if (sz.cx > (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0);

            RECT rcCtrl = { iPosX + rcPadding->left, iPosY + rcPadding->top, iPosX + rcPadding->left + sz.cx, iPosY + sz.cy + rcPadding->top + rcPadding->bottom };
            ZuiControlCall(Proc_SetPos, pControl, &rcCtrl, FALSE, 0);

            iPosY += sz.cy + op->m_iChildPadding + rcPadding->top + rcPadding->bottom;
            cyNeeded += sz.cy + rcPadding->top + rcPadding->bottom;
            szRemaining.cy -= sz.cy + op->m_iChildPadding + rcPadding->bottom;
        }
        cyNeeded += (nEstimateNum - 1) * op->m_iChildPadding;

        if (op->m_pHorizontalScrollBar != NULL) {
            if (cxNeeded > rc.right - rc.left) {
                if (op->m_pHorizontalScrollBar->m_bVisible) {
                    ZuiControlCall(Proc_ScrollBar_SetScrollRange, op->m_pHorizontalScrollBar, (ZuiAny)(cxNeeded - (rc.right - rc.left)), NULL, NULL);
                }
                else {
                    ZuiControlCall(Proc_SetVisible, op->m_pHorizontalScrollBar, (ZuiAny)TRUE, NULL, NULL);
                    ZuiControlCall(Proc_ScrollBar_SetScrollRange, op->m_pHorizontalScrollBar, (ZuiAny)(cxNeeded - (rc.right - rc.left)), NULL, NULL);
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

        // Process the scrollbar
        ZuiControlCall(Proc_Layout_ProcessScrollBar, cp, (ZuiAny)&rc, (ZuiAny)cxNeeded, (ZuiAny)cyNeeded);
        return 0;
    }
    case Proc_ListBody_SetScrollPos: {
        int cx = 0;
        int cy = 0;
        SIZE *szPos = Param1;
        ZuiLayout op = ZuiControlCall(Proc_GetObject, cp, Type_Layout, NULL, NULL);

        if (op->m_pVerticalScrollBar && op->m_pVerticalScrollBar->m_bVisible) {
            int iLastScrollPos = (ZuiInt)ZuiControlCall(Proc_ScrollBar_GetScrollPos, op->m_pVerticalScrollBar, NULL, NULL, NULL);
            ZuiControlCall(Proc_ScrollBar_SetScrollPos, op->m_pVerticalScrollBar, szPos->cy, NULL, NULL);
            cy = (ZuiInt)ZuiControlCall(Proc_ScrollBar_GetScrollPos, op->m_pVerticalScrollBar, NULL, NULL, NULL) - iLastScrollPos;
        }

        if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible) {
            int iLastScrollPos = (ZuiInt)ZuiControlCall(Proc_ScrollBar_GetScrollPos, op->m_pHorizontalScrollBar, NULL, NULL, NULL);
            ZuiControlCall(Proc_ScrollBar_SetScrollPos, op->m_pHorizontalScrollBar, szPos->cx, NULL, NULL);
            cx = (ZuiInt)ZuiControlCall(Proc_ScrollBar_GetScrollPos, op->m_pHorizontalScrollBar, NULL, NULL, NULL) - iLastScrollPos;
        }

        if (cx == 0 && cy == 0) return;

        for (int it2 = 0; it2 < op->m_items->count; it2++) {
            ZuiControl pControl = (ZuiControl)(op->m_items->data[it2]);
            if (!pControl->m_bVisible) continue;
            if (pControl->m_bFloat) continue;
            SIZE sz = { -cx, -cy };
            ZuiControlMove(pControl, &sz, FALSE);
        }

        ZuiControlInvalidate(cp, TRUE);

        if (cx != 0 && p->m_pOwner) {
            //得到表头控件
            ZuiControl pHeader = ZuiControlCall(Proc_List_GetHeader, p->m_pOwner, cp, NULL, NULL);
            if (pHeader == NULL) return;
            ZuiListInfo pInfo = ZuiControlCall(Proc_List_GetListInfo, p->m_pOwner, cp, NULL, NULL);
            pInfo->nColumns = MIN(ZuiControlCall(Proc_Layout_GetCount, pHeader, NULL, NULL, NULL), LIST_MAX_COLUMNS);
            for (int i = 0; i < pInfo->nColumns; i++) {
                ZuiControl pControl = ZuiControlCall(Proc_Layout_GetItemAt, pHeader, i, NULL, NULL);
                if (!pControl->m_bVisible) continue;
                if (pControl->m_bFloat) continue;
                SIZE sz = { -cx, -cy };
                ZuiControlMove(pControl, &sz, FALSE);
                pInfo->rcColumn[i] = *(RECT *)ZuiControlCall(Proc_GetPos, pControl, NULL, NULL, NULL);
            }

            ZuiControlInvalidate(pHeader, TRUE);
        }
        return 0;
    }
    case Proc_ListBody_SetOwner: {
        if (Param1)
            p->m_pOwner = Param1;
        break;
    }
    case Proc_OnCreate: {
        p = (ZuiListBody)ZuiMalloc(sizeof(ZListBody));
        memset(p, 0, sizeof(ZListBody));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiVerticalLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiVerticalLayoutProc;


        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;

        ZuiFree(p);

        return old_call(ProcId, cp, old_udata, Param1, Param2, Param3);
    }
    case Proc_GetObject: {
        if (Param1 == Type_ListBody)
            return (ZuiAny)p;
        break;
    }
    case Proc_GetType:
        return (ZuiAny)Type_ListBody;
    case Proc_CoreInit:
        //将辅助控件注册到系统
        ZuiControlRegisterAdd(L"listelement", (ZCtlProc)&ZuiListElementProc);
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}
ZEXPORT ZuiAny ZCALL ZuiListElementProc(ZuiInt ProcId, ZuiControl cp, ZuiListElement p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_OnPaintBkColor: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        RECT *rc = (RECT*)Param2;
        ZuiListInfo pInfo = ZuiControlCall(Proc_List_GetListInfo, p->m_pOwner, cp, NULL, NULL);
        if (pInfo->dwLineColor != 0) {
            ZuiDrawLine(gp, pInfo->dwLineColor, rc->left, rc->bottom - 1, rc->right, rc->bottom - 1, 1);
        }
        break;
    }
    case Proc_ListElement_SetOwner: {
        if (Param1)
            p->m_pOwner = Param1;
        break;
    }
    case Proc_ListElement_SetIndex: {
        return p->m_iIndex;
    }
    case Proc_ListElement_GetIndex: {
        p->m_iIndex = Param1;
        break;
    }
    case Proc_OnCreate: {
        p = (ZuiListHeaderItem)ZuiMalloc(sizeof(ZListHeaderItem));
        memset(p, 0, sizeof(ZListHeaderItem));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiLayoutProc;

        p->m_iIndex = -1;
        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;

        ZuiFree(p);

        return old_call(ProcId, cp, old_udata, Param1, Param2, Param3);
    }
    case Proc_GetObject: {
        if (Param1 == Type_ListElement)
            return (ZuiAny)p;
        break;
    }
    case Proc_GetType:
        return (ZuiAny)Type_ListElement;
    case Proc_CoreInit:
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit:
        return (ZuiAny)TRUE;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}
ZEXPORT ZuiAny ZCALL ZuiListHeaderProc(ZuiInt ProcId, ZuiControl cp, ZuiListHeader p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_EstimateSize: {
        ZuiLayout op = ZuiControlCall(Proc_GetObject, cp, Type_Layout, NULL, NULL);
        p->cXY.Height = cp->m_cxyFixed.cy;
        p->cXY.Width = 0;
        if (p->cXY.Height == 0 && cp->m_pManager != NULL) {
            for (int it = 0; it < op->m_items->count; it++) {
                SIZE * psz = (SIZE *)ZuiControlCall(Proc_EstimateSize, op->m_items->data[it], Param1, 0, 0);
                p->cXY.Height = MAX(p->cXY.Height, psz->cy);
            }
            //int nMin = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 6;
            //cXY.cy = MAX(cXY.cy, nMin);
        }

        for (int it = 0; it < op->m_items->count; it++) {
            SIZE * psz = (SIZE *)ZuiControlCall(Proc_EstimateSize, op->m_items->data[it], Param1, 0, 0);
            p->cXY.Width += psz->cx;
        }

        return &p->cXY;
    }
    case Proc_OnCreate: {
        p = (ZuiListHeader)ZuiMalloc(sizeof(ZListHeader));
        memset(p, 0, sizeof(ZListHeader));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiHorizontalLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiHorizontalLayoutProc;


        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;

        ZuiFree(p);

        return old_call(ProcId, cp, old_udata, Param1, Param2, Param3);
    }
    case Proc_GetObject: {
        if (Param1 == Type_ListHeader)
            return (ZuiAny)p;
        break;
    }
    case Proc_GetType:
        return (ZuiAny)Type_ListHeader;
    case Proc_CoreInit:
        //将辅助控件注册到系统
        ZuiControlRegisterAdd(L"listheaderitem", (ZCtlProc)&ZuiListHeaderItemProc);
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit: {
        return (ZuiAny)TRUE;
    }
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}
ZEXPORT ZuiAny ZCALL ZuiListHeaderItemProc(ZuiInt ProcId, ZuiControl cp, ZuiListHeaderItem p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        //不响应鼠标消息
        if (!cp->m_bMouseEnabled && event->Type > ZEVENT__MOUSEBEGIN && event->Type < ZEVENT__MOUSEEND) {
            if (cp->m_pParent != NULL)
                ZuiControlCall(Proc_OnEvent, cp->m_pParent, Param1, NULL, NULL);
            else
                ZuiLayoutProc(Proc_OnEvent, cp, 0, Param1, NULL, NULL);
            return;
        }

        if (event->Type == ZEVENT_SETFOCUS)
        {
            ZuiControlInvalidate(cp, TRUE);
        }
        if (event->Type == ZEVENT_KILLFOCUS)
        {
            ZuiControlInvalidate(cp, TRUE);
        }
        if (event->Type == ZEVENT_LBUTTONDOWN || event->Type == ZEVENT_LDBLCLICK)
        {
            if (!cp->m_bEnabled) return;
            //RECT rcSeparator = GetThumbRect();
            //if (p->m_iSepWidth >= 0)//111024 by cddjr, 增加分隔符区域，方便用户拖动
            //    rcSeparator.left -= 4;
            //else
            //    rcSeparator.right += 4;
            //if (PtInRect(&rcSeparator, *(POINT *)&event->ptMouse)) {
            //    if (p->m_bDragable) {
            //        p->m_uButtonState |= ZSTATE_CAPTURED;
            //        ptLastMouse = event->ptMouse;
            //    }
            //}
            //else {
            //    p->m_uButtonState |= ZSTATE_PUSHED;
            //    //表头单击事件
            //    //m_pManager->SendNotify(this, DUI_MSGTYPE_HEADERCLICK);
            //    ZuiControlInvalidate(cp, TRUE);
            //}
            return;
        }
        if (event->Type == ZEVENT_LBUTTONUP)
        {
            if ((p->m_uButtonState & ZSTATE_CAPTURED) != 0) {
                p->m_uButtonState &= ~ZSTATE_CAPTURED;
                if (cp->m_pParent)
                    ZuiControlNeedParentUpdate(cp->m_pParent);
            }
            else if ((p->m_uButtonState & ZSTATE_PUSHED) != 0) {
                p->m_uButtonState &= ~ZSTATE_PUSHED;
                ZuiControlInvalidate(cp, TRUE);
            }
            return;
        }
        if (event->Type == ZEVENT_MOUSEMOVE)
        {
            if ((p->m_uButtonState & ZSTATE_CAPTURED) != 0) {
                RECT rc = cp->m_rcItem;
                //if (p->m_iSepWidth >= 0) {
                //    rc.right -= ptLastMouse.x - event.ptMouse.x;
                //}
                //else {
                //    rc.left -= ptLastMouse.x - event->ptMouse.x;
                //}

                //if (rc.right - rc.left > GetMinWidth()) {
                //    m_cxyFixed.cx = rc.right - rc.left;
                //    ptLastMouse = event->ptMouse;
                //    if (GetParent())
                //        GetParent()->NeedParentUpdate();
                //}
            }
            return;
        }
        if (event->Type == ZEVENT_SETCURSOR)
        {
            RECT rcSeparator = {0};// = GetThumbRect();
            if (p->m_iSepWidth >= 0)//111024 by cddjr, 增加分隔符区域，方便用户拖动
                rcSeparator.left -= 4;
            else
                rcSeparator.right += 4;
            if (cp->m_bEnabled && p->m_bDragable && PtInRect(&rcSeparator, *(POINT *)&event->ptMouse)) {
                SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
                return;
            }
        }
        if (event->Type == ZEVENT_MOUSEENTER)
        {
            if (cp->m_bEnabled) {
                p->m_uButtonState |= ZSTATE_HOT;
                ZuiControlInvalidate(cp, TRUE);
            }
            return;
        }
        if (event->Type == ZEVENT_MOUSELEAVE)
        {
            if (cp->m_bEnabled) {
                p->m_uButtonState &= ~ZSTATE_HOT;
                ZuiControlInvalidate(cp, TRUE);
            }
            return;
        }
        break;
    }
    case Proc_OnPaintText: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        RECT *rc = Param2;
        ZRect r;
        MAKEZRECT(r, rc->left + 5, rc->top + 5, rc->right - rc->left - 10, rc->bottom - rc->top - 10);
        ZuiDrawString(gp, Global_StringFormat, cp->m_sText, &r);
        break;
    }
    case Proc_OnCreate: {
        p = (ZuiListHeaderItem)ZuiMalloc(sizeof(ZListHeaderItem));
        memset(p, 0, sizeof(ZListHeaderItem));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiLayoutProc;


        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;

        ZuiFree(p);

        return old_call(ProcId, cp, old_udata, Param1, Param2, Param3);
    }
    case Proc_GetObject: {
        if (Param1 == Type_ListHeaderItem)
            return (ZuiAny)p;
        break;
    }
    case Proc_GetType:
        return (ZuiAny)Type_ListHeaderItem;
    case Proc_CoreInit:
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}

