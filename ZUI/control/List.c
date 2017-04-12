#include <ZUI.h>


ZEXPORT ZuiAny ZCALL ZuiListProc(ZuiInt ProcId, ZuiControl cp, ZuiList p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_SetPos: {
        //通知父容器调整布局
        ZuiVerticalLayoutProc(ProcId, cp, p->old_udata, Param1, Param2, Param3);
        if (p->m_pHeader == NULL) return;
        //计算列数量
        p->m_ListInfo.nColumns = MIN(ZuiControlCall(Proc_Layout_GetCount, p->m_pHeader, NULL, NULL, NULL), ZLIST_MAX_COLUMNS);

        if (!p->m_pHeader->m_bVisible) {
            //for (int it = 0; it < m_pHeader->GetCount(); it++) {
            //    static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(true);
            //}
            //m_pHeader->SetPos(CDuiRect(rc.left, 0, rc.right, 0), bNeedInvalidate);
        }

        for (int i = 0; i < p->m_ListInfo.nColumns; i++) {
            ZuiControl pControl = ZuiControlCall(Proc_Layout_GetItemAt, p->m_pHeader, i, NULL, NULL);
            if (!pControl->m_bVisible) continue;
            if (pControl->m_bFloat) continue;
            p->m_ListInfo.rcColumn[i] = *(ZRect *)ZuiControlCall(Proc_GetPos, pControl, NULL, NULL, NULL);
        }
        if (!p->m_pHeader->m_bVisible) {
            //for (int it = 0; it < m_pHeader->GetCount(); it++) {
            //    static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(false);
            //}
        }
        ZuiControlCall(Proc_SetPos, p->m_pList, ZuiControlCall(Proc_GetPos, p->m_pList, NULL, NULL, NULL), Param2, NULL);
        return;
    }
    case Proc_SetAttribute: {
        if (wcscmp(Param1, _T("header")) == 0) ZuiControlCall(Proc_SetVisible, p, wcscmp(Param2, L"hidden") == 0 ? FALSE : TRUE, NULL, NULL);
        break;
    }
    case Proc_List_SetScrollPos: {
        return ZuiControlCall(Proc_ListBody_SetScrollPos, p->m_pList, Param1, NULL, NULL);
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
                p->m_ListInfo.nColumns = MIN(ZuiControlCall(Proc_Layout_GetCount, p->m_pHeader, NULL, NULL, NULL), ZLIST_MAX_COLUMNS);
                return ZuiVerticalLayoutProc(Proc_Layout_AddAt, cp, p->old_udata, p->m_pHeader, 0, NULL);
            }
            return FALSE;
        }
        //判断元素是否是表头元素
        if (ZuiControlCall(Proc_GetType, Param1, NULL, NULL, NULL) == Type_ListHeaderItem) {
            //插入到头容器
            ZuiBool ret = ZuiControlCall(Proc_Layout_Add, p->m_pHeader, Param1, 0, NULL);
            //计算列数量
            p->m_ListInfo.nColumns = MIN(ZuiControlCall(Proc_Layout_GetCount, p->m_pHeader, NULL, NULL, NULL), ZLIST_MAX_COLUMNS);
            return ret;
        }
        // 插入的元素是行数据
        if (ZuiControlCall(Proc_GetObject, Param1, Type_ListElement, NULL, NULL)) {
            ZuiControlCall(Proc_ListElement_SetOwner, Param1, cp, NULL, NULL);
            ZuiControlCall(Proc_ListElement_SetIndex, Param1, ZuiControlCall(Proc_List_GetCount, p->m_pList, NULL, NULL, NULL), NULL, NULL);
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
                p->m_ListInfo.nColumns = MIN(ZuiControlCall(Proc_Layout_GetCount, p->m_pHeader, NULL, NULL, NULL), ZLIST_MAX_COLUMNS);
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
            p->m_ListInfo.nColumns = MIN(ZuiControlCall(Proc_Layout_GetCount, p->m_pHeader, NULL, NULL, NULL), ZLIST_MAX_COLUMNS);
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
    case Proc_List_SelectItem: {
        // 取消所有选择项
        ZuiControlCall(Proc_List_UnSelectAllItems, cp, NULL, NULL, NULL);
        // 判断是否合法列表项
        if (Param1 < 0) return FALSE;
        ZuiControl pControl = ZuiControlCall(Proc_List_GetItemAt, cp, Param1, NULL, NULL);
        if (pControl == NULL) return FALSE;
        if (!ZuiControlCall(Proc_ListElement_Select, pControl, TRUE, NULL, NULL)) {
            return FALSE;
        }
        int iLastSel = p->m_iCurSel;
        p->m_iCurSel = Param1;
        darray_append(p->m_aSelItems, Param1);
        //EnsureVisible(iIndex);//定位滚动条
        //if (p->bTakeFocus) pControl->SetFocus();//设置焦点
        if (cp->m_pManager != NULL && iLastSel != p->m_iCurSel) {
            //事件通知
            
            //m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMSELECT, iIndex);
        }

        return TRUE;
    }
    case Proc_List_SelectMultiItem: {

    }
    case Proc_List_SetMultiSelect: {

    }
    case Proc_List_IsMultiSelect: {

    }
    case Proc_List_UnSelectItem: {
        if (!p->m_bMultiSel)
            return FALSE;
        if (Param2) {
            //for (int i = m_aSelItems.GetSize() - 1; i >= 0; --i) {
            //    int iSelIndex = (int)m_aSelItems.GetAt(i);
            //    if (iSelIndex == Param1) continue;
            //    CControlUI* pControl = GetItemAt(iSelIndex);
            //    if (pControl == NULL) continue;
            //    if (!pControl->IsEnabled()) continue;
            //    IListItemUI* pSelListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
            //    if (pSelListItem == NULL) continue;
            //    if (!pSelListItem->SelectMulti(false)) continue;
            //    m_aSelItems.Remove(i);
            //}
        }
        else {
            if (Param1 < 0) 
                return FALSE;
            ZuiControl pControl = ZuiControlCall(Proc_List_GetItemAt, cp, Param1, NULL, NULL);
            if (pControl == NULL) return FALSE;
            if (!pControl->m_bEnabled) return FALSE;
            
            int aIndex = darray_find(p->m_aSelItems, Param1);
            if (aIndex < 0) return FALSE;
            if (!ZuiControlCall(Proc_ListElement_SelectMulti, pControl, FALSE, NULL, NULL)) return FALSE;
            if (p->m_iCurSel == Param1) p->m_iCurSel = -1;
            darray_delete(p->m_aSelItems, aIndex);
        }
        return TRUE;
    }
    case Proc_List_SelectAllItems: {

    }
    case Proc_List_UnSelectAllItems: {
        for (int i = 0; i < p->m_aSelItems->count; ++i) {
            int iSelIndex = darray_getat(p->m_aSelItems, i);
            ZuiControl pControl = ZuiControlCall(Proc_List_GetItemAt, cp, iSelIndex, NULL, NULL);;
            if (pControl == NULL) continue;
            if (!pControl->m_bEnabled) continue;
            if (!ZuiControlCall(Proc_ListElement_SelectMulti, pControl, FALSE, NULL, NULL)) continue;
        }
        darray_empty(p->m_aSelItems);
        p->m_iCurSel = -1;
        return TRUE;
    }
    //直接转发到列表体的消息
    case Proc_Layout_LineUp:
    case Proc_Layout_LineDown:
    case Proc_Layout_PageUp:
    case Proc_Layout_PageDown:
    case Proc_Layout_HomeUp:
    case Proc_Layout_EndDown:
    case Proc_Layout_LineLeft:
    case Proc_Layout_LineRight:
    case Proc_Layout_PageLeft:
    case Proc_Layout_PageRight:
    case Proc_Layout_HomeLeft:
    case Proc_Layout_EndRight:
    case Proc_List_GetCount:
    case Proc_List_GetItemAt:
        return ZuiControlCall(ProcId, p->m_pList, Param1, Param2, Param3);
    case Proc_OnCreate: {
        p = (ZuiList)ZuiMalloc(sizeof(ZList));
        memset(p, 0, sizeof(ZList));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiVerticalLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiVerticalLayoutProc;

        p->m_aSelItems = darray_create();

        //创建表头
        p->m_pHeader = NewZuiControl(L"ListHeader", NULL, NULL, NULL);
        ZuiVerticalLayoutProc(Proc_Layout_Add, cp, p->old_udata, p->m_pHeader, NULL, NULL);
        ZuiControlCall(Proc_SetBkColor, p->m_pHeader, ARGB(255, 111, 222, 200), NULL, NULL);
        //创建表主体
        p->m_pList = NewZuiControl(L"ListBody", NULL, NULL, NULL);
        ZuiVerticalLayoutProc(Proc_Layout_Add, cp, p->old_udata, p->m_pList, NULL, NULL);
        ZuiControlCall(Proc_ListBody_SetOwner, p->m_pList, cp, NULL, NULL);

        p->m_ListInfo.dwLineColor = 0xFF004488;
        p->m_ListInfo.dwHotBkColor = 0xFF002244;
        p->m_ListInfo.dwSelectedBkColor = 0xFFFF1166;
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
        ZRect rc = *(ZRect*)Param1;
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
            cyFixed += sz.cy + ((ZRect *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0)))->top + ((ZRect *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0)))->bottom;

            ZRect *rcPadding = (ZRect *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0));
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
                if (op->m_pHorizontalScrollBar && op->m_pHorizontalScrollBar->m_bVisible)
                {
                    int nOffset = (LONG)ZuiControlCall(Proc_ScrollBar_GetScrollPos, op->m_pHorizontalScrollBar, NULL, NULL, NULL);
                    ZRect rcHeader = *(ZRect *)ZuiControlCall(Proc_GetPos, pHeader, NULL, NULL, NULL);
                    rcHeader.left = rc.left - nOffset;
                    ZuiControlCall(Proc_SetPos, pHeader, &rcHeader, FALSE, 0);
                }
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

            ZRect *rcPadding = (ZRect *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0));
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

            ZRect rcCtrl = { iPosX + rcPadding->left, iPosY + rcPadding->top, iPosX + rcPadding->left + sz.cx, iPosY + sz.cy + rcPadding->top + rcPadding->bottom };
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

        ZRect rcPos;

        if (p->m_pOwner) {
            //得到表头控件
            ZuiControl pHeader = ZuiControlCall(Proc_List_GetHeader, p->m_pOwner, cp, NULL, NULL);
            if (pHeader == NULL) return;
            ZuiListInfo pInfo = ZuiControlCall(Proc_List_GetListInfo, p->m_pOwner, cp, NULL, NULL);
            pInfo->nColumns = MIN(ZuiControlCall(Proc_Layout_GetCount, pHeader, NULL, NULL, NULL), ZLIST_MAX_COLUMNS);

            //if (!pHeader->m_bVisible) {
            //    for (int it = 0; it < ZuiControlCall(Proc_Layout_GetCount, pHeader, NULL, NULL, NULL); it++) {
            //        //static_cast<CControlUI*>(pHeader->GetItemAt(it))->SetInternVisible(true);
            //   }
            //}
            for (int i = 0; i < pInfo->nColumns; i++) {
                ZuiControl pControl = ZuiControlCall(Proc_Layout_GetItemAt, pHeader, i, NULL, NULL);
                if (!pControl->m_bVisible) continue;
                if (pControl->m_bFloat) continue;

                rcPos = *(ZRect *)ZuiControlCall(Proc_GetPos, pControl, NULL, NULL, NULL);
                rcPos.left -= cx;
                rcPos.right -= cx;
                ZuiControlCall(Proc_SetPos, pControl, &rcPos, TRUE, 0);
                pInfo->rcColumn[i] = *(ZRect *)ZuiControlCall(Proc_GetPos, pControl, NULL, NULL, NULL);
            }
            //将列表元素的移动调整到头移动完成后
            for (int it2 = 0; it2 < op->m_items->count; it2++) {
                ZuiControl pControl = (ZuiControl)(op->m_items->data[it2]);
                if (!pControl->m_bVisible) continue;
                if (pControl->m_bFloat) continue;

                rcPos = *(ZRect *)ZuiControlCall(Proc_GetPos, pControl, NULL, NULL, NULL);
                rcPos.left -= cx;
                rcPos.right -= cx;
                rcPos.top -= cy;
                rcPos.bottom -= cy;
                ZuiControlCall(Proc_SetPos, pControl, &rcPos, TRUE, 0);
            }

            //if (!pHeader->m_bVisible) {
            //    for (int it = 0; it < ZuiControlCall(Proc_Layout_GetCount, pHeader, NULL, NULL, NULL); it++) {
            //        //static_cast<CControlUI*>(pHeader->GetItemAt(it))->SetInternVisible(false);
            //    }
            //}
        }
        ZuiControlInvalidate(cp, TRUE);
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
    case Proc_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        if (!cp->m_bMouseEnabled && event->Type > ZEVENT__MOUSEBEGIN && event->Type < ZEVENT__MOUSEEND) {
            if (p->m_pOwner != NULL) 
                ZuiControlCall(Proc_OnEvent, p->m_pOwner, Param1, NULL, NULL);
            else 
                ZuiLayoutProc(Proc_OnEvent, cp, 0, Param1, NULL, NULL);
            return;
        }

        if (event->Type == ZEVENT_LDBLCLICK)
        {
            if (cp->m_bEnabled) {
                //Activate();
                ZuiControlInvalidate(cp, TRUE);
            }
            return;
        }
        if (event->Type == ZEVENT_KEYDOWN && cp->m_bEnabled)
        {
            if (event->chKey == VK_RETURN) {
                //Activate();
                ZuiControlInvalidate(cp, TRUE);
                return;
            }
        }
        if (event->Type == ZEVENT_LBUTTONDOWN)
        {
            if (cp->m_bEnabled) {
                if ((GetKeyState(VK_CONTROL) & 0x8000)) {
                    //SelectMulti(!IsSelected());
                }
                else
                    ZuiControlCall(Proc_ListElement_Select, cp, TRUE, NULL, NULL);
            }
            return;
        }
        if (event->Type == ZEVENT_LBUTTONUP)
        {
            if (cp->m_bEnabled) {
                //m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
            }
            return;
        }
        if (event->Type == ZEVENT_MOUSEMOVE)
        {
            return;
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
            if ((p->m_uButtonState & ZSTATE_HOT) != 0) {
                p->m_uButtonState &= ~ZSTATE_HOT;
                ZuiControlInvalidate(cp, TRUE);
            }
            return;
        }
        if (event->Type == ZEVENT_TIMER)
        {
            //m_pManager->SendNotify(this, DUI_MSGTYPE_TIMER, event.wParam, event.lParam);
            return;
        }

        if (event->Type == ZEVENT_CONTEXTMENU)
        {
            //if (IsContextMenuUsed()) {
            //    m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
            //    return;
            //}
        }
        if (p->m_pOwner != NULL) ZuiControlCall(Proc_OnEvent, p->m_pOwner, Param1, NULL, NULL);
        else ZuiLayoutProc(Proc_OnEvent, cp, 0, Param1, NULL, NULL);
        break;
    }
    case Proc_Invalidate: {
        if (!cp->m_bVisible) return;

        if (cp->m_pParent) {
            ZuiLayout pParentContainer = ZuiControlCall(Proc_GetObject, cp->m_pParent, Type_Layout, NULL, NULL);
            if (pParentContainer) {
                ZRect rc = *(ZRect *)ZuiControlCall(Proc_GetPos, cp->m_pParent, NULL, NULL, NULL);
                ZRect rcInset = pParentContainer->m_rcInset;
                rc.left += rcInset.left;
                rc.top += rcInset.top;
                rc.right -= rcInset.right;
                rc.bottom -= rcInset.bottom;
                if (pParentContainer->m_pVerticalScrollBar && pParentContainer->m_pVerticalScrollBar->m_bVisible)
                    rc.right -= (ZuiInt)ZuiControlCall(Proc_GetFixedWidth, pParentContainer->m_pVerticalScrollBar, NULL, NULL, NULL);
                if (pParentContainer->m_pHorizontalScrollBar && pParentContainer->m_pHorizontalScrollBar->m_bVisible)
                    rc.bottom -= (ZuiInt)ZuiControlCall(Proc_GetFixedHeight, pParentContainer->m_pHorizontalScrollBar, NULL, NULL, NULL);

                ZRect invalidateRc = cp->m_rcItem;
                if (!IntersectRect(&invalidateRc, &cp->m_rcItem, &rc))
                {
                    return;
                }

                ZuiControl pParent = cp->m_pParent;
                ZRect rcTemp;
                ZRect *rcParent;
                while (pParent = pParent->m_pParent)
                {
                    rcTemp = invalidateRc;
                    rcParent = (ZRect *)ZuiControlCall(Proc_GetPos, pParent, NULL, NULL, NULL);
                    if (!IntersectRect(&invalidateRc, &rcTemp, rcParent))
                    {
                        return;
                    }
                }
                //重置动画
                if (Param1 && cp->m_aAnime)
                    cp->m_aAnime->steup = NULL;
                if (cp->m_pManager != NULL) ZuiPaintManagerInvalidateRect(cp->m_pManager, &invalidateRc);
            }
            else {
                ZuiLayoutProc(Proc_Invalidate, cp, p->old_udata, Param1, NULL, NULL);
            }
        }
        else {
            ZuiLayoutProc(Proc_Invalidate, cp, p->old_udata, Param1, NULL, NULL);
        }
        return;
    }
    case Proc_SetPos: {
        ZuiLayout op = ZuiControlCall(Proc_GetObject, cp, Type_Layout, NULL, NULL);
        //通知父容器调整布局
        ZuiLayoutProc(ProcId, cp, p->old_udata, Param1, Param2, Param3);
        if (p->m_pOwner == NULL) return;
        //UINT uListType = m_pOwner->GetListType();
        //if (uListType != LT_LIST && uListType != LT_TREE) return;

        //CListUI* pList = static_cast<CListUI*>(m_pOwner);

        //if (uListType == LT_TREE)
        //{
        //    pList = (CListUI*)pList->CControlUI::GetInterface(_T("List"));
        //    if (pList == NULL) return;
        //}

        ZuiControl pHeader = ZuiControlCall(Proc_List_GetHeader, p->m_pOwner, cp, NULL, NULL);
        if (pHeader == NULL || !pHeader->m_bVisible)
            return;
        int nCount = op->m_items->count;
        for (int i = 0; i < nCount; i++)
        {
            ZuiControl pListItem = (ZuiControl)(op->m_items->data[i]);
            ZuiControl pHeaderItem = ZuiControlCall(Proc_Layout_GetItemAt, pHeader, i, NULL, NULL);
            if (pHeaderItem == NULL) return;
            ZRect *rcHeaderItem = ZuiControlCall(Proc_GetPos, pHeaderItem, NULL, NULL, NULL);
            if (pListItem != NULL && !(rcHeaderItem->left == 0 && rcHeaderItem->right == 0))
            {
                ZRect rt = *(ZRect *)ZuiControlCall(Proc_GetPos, pListItem, NULL, NULL, NULL);
                rt.left = rcHeaderItem->left;
                rt.right = rcHeaderItem->right;
                
                //rt.right--;
                //rt.bottom--;
                
                ZuiControlCall(Proc_SetPos, pListItem, &rt, FALSE, 0);
            }
        }
        return;
    }
    case Proc_OnPaintBkColor: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = (ZRect*)&cp->m_rcItem;
        if (p->m_pOwner == NULL) return;
        ZuiListInfo pInfo = ZuiControlCall(Proc_List_GetListInfo, p->m_pOwner, cp, NULL, NULL);
        
        

        DWORD iBackColor = 0;
        if (!pInfo->bAlternateBk || p->m_iIndex % 2 == 0) iBackColor = pInfo->dwBkColor;

        if ((p->m_uButtonState & ZSTATE_HOT) != 0) {
            iBackColor = pInfo->dwHotBkColor;
        }
        if (p->m_bSelected) {
            iBackColor = pInfo->dwSelectedBkColor;
        }
        if (!cp->m_bEnabled) {
            iBackColor = pInfo->dwDisabledBkColor;
        }
        if (iBackColor != 0) {
            ZuiDrawFillRect(gp, iBackColor, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
        }

        //if (!cp->m_bEnabled) {
        //    if (!pInfo->sDisabledImage.IsEmpty()) {
        //        if (!DrawImage(hDC, (LPCTSTR)pInfo->sDisabledImage)) {}
        //        else return;
        //    }
        //}
        //if (IsSelected()) {
        //    if (!pInfo->sSelectedImage.IsEmpty()) {
        //        if (!DrawImage(hDC, (LPCTSTR)pInfo->sSelectedImage)) {}
        //        else return;
        //    }
        //}
        //if ((m_uButtonState & UISTATE_HOT) != 0) {
        //    if (!pInfo->sHotImage.IsEmpty()) {
        //        if (!DrawImage(hDC, (LPCTSTR)pInfo->sHotImage)) {}
        //        else return;
        //    }
        //}
        //if (!m_sBkImage.IsEmpty()) {
        //    if (!pInfo->bAlternateBk || m_iIndex % 2 == 0) {
        //        if (!DrawImage(hDC, (LPCTSTR)m_sBkImage)) {}
        //    }
        //}

        //if (m_sBkImage.IsEmpty()) {
        //    if (!pInfo->sBkImage.IsEmpty()) {
        //        if (!DrawImage(hDC, (LPCTSTR)pInfo->sBkImage)) {}
        //        else return;
        //    }
        //}

        if (pInfo->dwLineColor != 0) {
            if (pInfo->bShowRowLine) {
                ZuiDrawLine(gp, pInfo->dwLineColor, rc->left, rc->bottom - 1, rc->right, rc->bottom - 1, 1);
            }
            if (pInfo->bShowColumnLine) {
                for (int i = 0; i < pInfo->nColumns; i++) {
                    ZuiDrawLine(gp, pInfo->dwLineColor, pInfo->rcColumn[i].right - 1, rc->top, pInfo->rcColumn[i].right - 1, rc->bottom - 1, 1);
                }
            }
        }
        break;
    }
    case Proc_ListElement_SetOwner: {
        if (Param1)
            p->m_pOwner = Param1;
        break;
    }
    case Proc_ListElement_GetIndex: {
        return p->m_iIndex;
    }
    case Proc_ListElement_SetIndex: {
        p->m_iIndex = Param1;
        break;
    }
    case Proc_ListElement_Select: {
        if (!cp->m_bEnabled) return FALSE;
        if (p->m_pOwner != NULL && p->m_bSelected)
            ZuiControlCall(Proc_List_UnSelectItem, p->m_pOwner, p->m_iIndex, TRUE, NULL);//如果被选中就先反选,然后重新选取
        if (Param1 == p->m_bSelected) return TRUE;
        p->m_bSelected = Param1;
        if (Param1 && p->m_pOwner != NULL)
            ZuiControlCall(Proc_List_SelectItem, p->m_pOwner, p->m_iIndex, FALSE, NULL);
        ZuiControlInvalidate(cp, TRUE);

        return TRUE;
    }
    case Proc_ListElement_SelectMulti: {
        if (!cp->m_bEnabled) return FALSE;
        if (Param1 == p->m_bSelected) return TRUE;

        p->m_bSelected = Param1;
        if (Param1 && p->m_pOwner != NULL)
            ZuiControlCall(Proc_List_SelectMultiItem, p->m_pOwner, p->m_iIndex, FALSE, NULL);
        ZuiControlInvalidate(cp, TRUE);
        return TRUE;
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
        p->cXY.cy = cp->m_cxyFixed.cy;
        p->cXY.cx = 0;
        if (p->cXY.cy == 0 && cp->m_pManager != NULL) {
            for (int it = 0; it < op->m_items->count; it++) {
                SIZE * psz = (SIZE *)ZuiControlCall(Proc_EstimateSize, op->m_items->data[it], Param1, 0, 0);
                p->cXY.cy = MAX(p->cXY.cy, psz->cy);
            }
            //高度为字体大小
            //int nMin = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 6;
            //cXY.cy = MAX(cXY.cy, nMin);
        }

        for (int it = 0; it < op->m_items->count; it++) {
            SIZE * psz = (SIZE *)ZuiControlCall(Proc_EstimateSize, op->m_items->data[it], Param1, 0, 0);
            p->cXY.cx += psz->cx;
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
    case Proc_GetControlFlags: {
        return ZFLAG_SETCURSOR;//需要设置鼠标
    }
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
            ZRect *rcSeparator = ZuiControlCall(Proc_ListHeaderItem_GetThumbRect, cp, NULL, NULL, NULL);
            if (p->m_iSepWidth >= 0)
                rcSeparator->left -= 4;
            else
                rcSeparator->right += 4;
            if (PtInRect(rcSeparator, *(POINT *)&event->ptMouse)) {
                if (p->m_bDragable) {
                    p->m_uButtonState |= ZSTATE_CAPTURED;
                    p->ptLastMouse = event->ptMouse;
                }
            }
            else {
                p->m_uButtonState |= ZSTATE_PUSHED;
                //表头单击事件
                //m_pManager->SendNotify(this, DUI_MSGTYPE_HEADERCLICK);
                ZuiControlInvalidate(cp, TRUE);
            }
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
                ZRect rc = cp->m_rcItem;
                if (p->m_iSepWidth >= 0) {
                    rc.right -= p->ptLastMouse.x - event->ptMouse.x;
                }
                else {
                    rc.left -= p->ptLastMouse.x - event->ptMouse.x;
                }

                if (rc.right - rc.left > ZuiControlCall(Proc_GetMinWidth, cp, 0, 0, 0)) {
                    cp->m_cxyFixed.cx = rc.right - rc.left;
                    p->ptLastMouse = event->ptMouse;
                    if (cp->m_pParent)
                        ZuiControlNeedParentUpdate(cp->m_pParent);
                }
            }
            return;
        }
        if (event->Type == ZEVENT_SETCURSOR)
        {
            ZRect *rcSeparator = ZuiControlCall(Proc_ListHeaderItem_GetThumbRect, cp, NULL, NULL, NULL);
            if (p->m_iSepWidth >= 0)
                rcSeparator->left -= 4;
            else
                rcSeparator->right += 4;
            if (cp->m_bEnabled && p->m_bDragable && PtInRect(rcSeparator, *(POINT *)&event->ptMouse)) {
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
        return;
    }
    case Proc_OnPaintText: {
        if (!cp->m_sText)
            return;
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = &cp->m_rcItem;
        ZRect r;
        MAKEZRECT(r, rc->left + 5, rc->top + 5, rc->right - rc->left - 10, rc->bottom - rc->top - 10);
        ZuiDrawString(gp, Global_Font, cp->m_sText, wcslen(cp->m_sText), &r);
        return;
    }
    case Proc_OnPaintStatusImage: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = (ZRect *)&cp->m_rcItem;
        ZuiImage img;
        if (cp->m_bFocused)
            p->m_uButtonState |= ZSTATE_FOCUSED;
        else p->m_uButtonState &= ~ZSTATE_FOCUSED;

        if ((p->m_uButtonState & ZSTATE_PUSHED) != 0) {
            if (p->m_diPushed) {
                img = p->m_diPushed->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            else if (p->m_diNormal) {
                img = p->m_diNormal->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
        }
        else if ((p->m_uButtonState & ZSTATE_HOT) != 0) {
            if (p->m_diHot) {
                img = p->m_diHot->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            else if (p->m_diNormal) {
                img = p->m_diNormal->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
        }
        else if ((p->m_uButtonState & ZSTATE_FOCUSED) != 0) {
            if (p->m_diFocused) {
                img = p->m_diFocused->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            else if (p->m_diNormal) {
                img = p->m_diNormal->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
        }
        else {
            if (p->m_diNormal) {
                img = p->m_diNormal->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
        }

        ZRect *rcThumb = ZuiControlCall(Proc_ListHeaderItem_GetThumbRect, cp, NULL, NULL, NULL);
        if (p->m_diSep) {
            rc = rcThumb;
            img = p->m_diSep->p;
            ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
        }
        //DrawImage(hDC, );*/
		return;
    }
    case Proc_ListHeaderItem_GetThumbRect: {
        if (p->m_iSepWidth >= 0) {
            p->Thumb.left = cp->m_rcItem.right - p->m_iSepWidth;
            p->Thumb.top = cp->m_rcItem.top;
            p->Thumb.right = cp->m_rcItem.right;
            p->Thumb.bottom = cp->m_rcItem.bottom;
        }
        else {
            p->Thumb.left = cp->m_rcItem.left;
            p->Thumb.top = cp->m_rcItem.top;
            p->Thumb.right = cp->m_rcItem.left - p->m_iSepWidth;
            p->Thumb.bottom = cp->m_rcItem.bottom;
        }

        //if (m_iSepWidth >= 0) return CDuiRect();
        //else return CDuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.left - m_iSepWidth, m_rcItem.bottom);
        return &p->Thumb;
    }
    case Proc_ListHeaderItem_EstimateSize: {
        //调整预设大小为字体高度
        //if (cp->m_cxyFixed.cy == 0) 
            //return CDuiSize(m_cxyFixed.cx, m_pManager->GetDefaultFontInfo()->tm.tmHeight + 14);
        break;
    }
    case Proc_OnCreate: {
        p = (ZuiListHeaderItem)ZuiMalloc(sizeof(ZListHeaderItem));
        memset(p, 0, sizeof(ZListHeaderItem));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiLayoutProc;
        //初始参数
        p->m_bDragable = TRUE;
        p->m_iSepWidth = 4;
        p->ptLastMouse.x = p->ptLastMouse.y = 0;
        ZuiControlCall(Proc_SetMinWidth, cp, 16, 0, 0);

        p->m_diNormal = ZuiResDBGetRes(L"default:default/list_header_bg.png", ZREST_IMG);  //通常图片
        p->m_diHot = ZuiResDBGetRes(L"default:default/list_header_hot.png", ZREST_IMG);        //点燃图片
        p->m_diPushed = ZuiResDBGetRes(L"default:default/list_header_pushed.png", ZREST_IMG);  //按下图片
        //p->m_diFocused= ZuiResDBGetRes(L"default:default/list_header_bg.png", ZREST_IMG); //焦点图片
        p->m_diSep = ZuiResDBGetRes(L"default:default/list_header_sep.png", ZREST_IMG);
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
    case Proc_SetAttribute: {
        if (wcscmp(Param1, _T("dragable")) == 0) ZuiControlCall(Proc_ListHeaderItem_SetDragable, p, wcscmp(Param2, L"true") == 0 ? TRUE : FALSE, NULL, NULL);
        else if (wcscmp(Param1, _T("sepwidth")) == 0) ZuiControlCall(Proc_ListHeaderItem_SetSepWidth, cp, _wtoi(Param2), NULL, NULL);
        else if (wcscmp(Param1, _T("normalimage")) == 0) ZuiControlCall(Proc_ListHeaderItem_SetNormalImage, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
        else if (wcscmp(Param1, _T("hotimage")) == 0) ZuiControlCall(Proc_ListHeaderItem_SetHotImage, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
        else if (wcscmp(Param1, _T("pushedimage")) == 0) ZuiControlCall(Proc_ListHeaderItem_SetPushedImage, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
        else if (wcscmp(Param1, _T("focusedimage")) == 0) ZuiControlCall(Proc_ListHeaderItem_SetFocusedImage, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
        else if (wcscmp(Param1, _T("sepimage")) == 0) ZuiControlCall(Proc_ListHeaderItem_SetSepImage, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
        break;
    }
    case Proc_ListHeaderItem_SetDragable: {
        p->m_bDragable = Param1;
        if (!Param1) p->m_uButtonState &= ~ZSTATE_CAPTURED;
        return;
    }
    case Proc_ListHeaderItem_SetSepWidth: {
        p->m_iSepWidth = Param1;
        return;
    }
    case Proc_ListHeaderItem_SetNormalImage: {
        if (p->m_diNormal)
            ZuiResDBDelRes(p->m_diNormal);
        p->m_diNormal = Param1;
        break;
    }
    case Proc_ListHeaderItem_SetHotImage: {
        if (p->m_diHot)
            ZuiResDBDelRes(p->m_diHot);
        p->m_diHot = Param1;
        break;
    }
    case Proc_ListHeaderItem_SetPushedImage: {
        if (p->m_diPushed)
            ZuiResDBDelRes(p->m_diPushed);
        p->m_diPushed = Param1;
        break;
    }
    case Proc_ListHeaderItem_SetFocusedImage: {
        if (p->m_diFocused)
            ZuiResDBDelRes(p->m_diFocused);
        p->m_diFocused = Param1;
        break;
    }
    case Proc_ListHeaderItem_SetSepImage: {
        if (p->m_diSep)
            ZuiResDBDelRes(p->m_diSep);
        p->m_diSep = Param1;
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

