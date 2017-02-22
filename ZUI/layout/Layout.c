#include <ZUI.h>




void* CALLBACK ZuiLayoutProc(int ProcId, ZuiControl cp, ZuiLayout p, void* Param1, void* Param2, void* Param3) {
    switch (ProcId)
    {
    case Proc_CoreInit:
        return TRUE;
        break;
    case Proc_OnCreate: {
        p = (ZuiLayout)ZuiMalloc(sizeof(ZLayout));
        memset(p, 0, sizeof(ZLayout));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        p->old_call = cp->call;
        p->m_bMouseChildEnabled = TRUE;
        p->m_iChildPadding = 0;
        p->m_items = darray_create();
        return p;
        break;
    }
    case Proc_OnDestroy: {
        for (int it = darray_len(p->m_items) - 1; it >= 0; it--) {
            ZuiControlCall(Proc_OnDestroy, p->m_items->data[it], TRUE, Param2, Param3);
        }
        ZCtlProc old_call = p->old_call;
        if (p->m_pHorizontalScrollBar)
            FreeZuiControl(p->m_pHorizontalScrollBar, NULL);
        if (p->m_pVerticalScrollBar)
            FreeZuiControl(p->m_pVerticalScrollBar, NULL);
        darray_destroy(p->m_items);
        ZuiFree(p);

        return old_call(ProcId, cp, 0, Param1, Param2, Param3);
        break;
        break;
    }
    case Proc_SetVisible: {
        p->old_call(Proc_SetVisible, cp, 0, Param1, Param2, Param3);
        for (int it = darray_len(p->m_items) - 1; it >= 0; it--) {
            ZuiControlCall(Proc_SetVisible, p->m_items->data[it], Param1, Param2, Param3);
        }
        return 0;
        break;
    }
    case Proc_Layout_Add: {
        if (Param1 == NULL) return FALSE;

        if (cp->m_pManager != NULL)
            ZuiPaintManagerInitControls(cp->m_pManager, (ZuiControl)Param1, cp);
        if (cp->m_bVisible)
            ZuiControlNeedUpdate(cp);

        return (void *)darray_append(p->m_items, Param1);
        break;
    }
    case Proc_Layout_AddAt: {
        if (Param1 == NULL) return FALSE;

        if (cp->m_pManager != NULL)
            ZuiPaintManagerInitControls(cp->m_pManager, (ZuiControl)Param1, cp);
        if (cp->m_bVisible)
            ZuiControlNeedUpdate(cp);

        return (void *)darray_insert(p->m_items, (int)Param2, (ZuiControl)Param1);
        break;
    }
    case Proc_Layout_Remove: {
        if (Param1 == NULL)
            return FALSE;

        for (int it = 0; it < darray_len(p->m_items); it++) {
            if ((ZuiControl)(p->m_items->data[it]) == (ZuiControl)Param1) {
                ZuiControlNeedUpdate(cp);
                if (!Param2)
                    FreeZuiControl((ZuiControl)Param1, NULL);
                return darray_delete(p->m_items, it);
            }
        }
        return FALSE;
        break;
    }
    case Proc_Layout_RemoveAt: {
        ZuiControl pControl = ZuiLayoutProc(Proc_Layout_GetItemAt, cp, p, Param1, 0, 0);
        if (pControl != NULL) {
            return ZuiLayoutProc(Proc_Layout_Remove, cp, p, pControl, 0, 0);
        }

        return FALSE;
        break;
    }
    case Proc_Layout_RemoveAll: {
        for (int it = 0; it < darray_len(p->m_items); it++) {
            FreeZuiControl((ZuiControl)(p->m_items->data[it]), NULL);
        }
        darray_empty(p->m_items);
        ZuiControlNeedUpdate(cp);
        break;
    }
    case Proc_Layout_GetCount: {
        return darray_len(p->m_items);
        break;
    }
    case Proc_Layout_GetItemIndex: {
        for (int it = 0; it < darray_len(p->m_items); it++) {
            if (p->m_items->data[it] == Param1) {
                return it;
            }
        }
        return -1;
        break;
    }
    case Proc_Layout_SetItemIndex: {
        for (int it = 0; it < darray_len(p->m_items); it++) {
            if (p->m_items->data[it] == Param1) {
                ZuiControlNeedUpdate(cp);
                darray_delete(p->m_items, it);
                return darray_insert(p->m_items, Param2, Param1);
            }
        }

        return FALSE;
        break;
    }
    case Proc_Layout_GetItemAt: {
        if (Param1 < 0 || Param1 >= darray_len(p->m_items)) return NULL;
        return p->m_items->data[(LONG)Param1];
        break;
    }
    case Proc_Layout_SetFloatPos: {
        if ((int)Param1 < 0 || (int)Param1 >= darray_len(p->m_items)) {
            return 0;
        }
        ZuiControl pControl = (ZuiControl)(p->m_items->data[(int)Param1]);

        if (!pControl->m_bVisible)
            return 0;
        if (!pControl->m_bFloat) {
            //不是浮动控件
            return 0;
        }
			SIZE *szXY = (SIZE *)ZuiControlCall(Proc_GetFixedXY, pControl, 0, 0, 0);
			SIZE sz = { (LONG)ZuiControlCall(Proc_GetFixedWidth, pControl, 0, 0, 0), (LONG)ZuiControlCall(Proc_GetFixedHeight, pControl, 0, 0, 0) };
			TPercentInfo rcPercent = { 0 };// pControl->GetFloatPercent();
			LONG width = cp->m_rcItem.right - cp->m_rcItem.left;
			LONG height = cp->m_rcItem.bottom - cp->m_rcItem.top;
			RECT rcCtrl = { 0 };
			rcCtrl.left = (LONG)(width*rcPercent.left) + szXY->cx;
			rcCtrl.top = (LONG)(height*rcPercent.top) + szXY->cy;
			rcCtrl.right = (LONG)(width*rcPercent.right) + szXY->cx + sz.cx;
			rcCtrl.bottom = (LONG)(height*rcPercent.bottom) + szXY->cy + sz.cy;
			ZuiControlCall(Proc_SetPos, pControl, &rcCtrl, FALSE, 0);
		
        break;
    }
    case Proc_SetPos: {
		RECT rc;
        p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
        if (darray_isempty(p->m_items))
            return 0;

        rc = cp->m_rcItem;
        rc.left += p->m_rcInset.left;
        rc.top += p->m_rcInset.top;
        rc.right -= p->m_rcInset.right;
        rc.bottom -= p->m_rcInset.bottom;

        for (int it = 0; it < darray_len(p->m_items); it++) {
            ZuiControl pControl = (ZuiControl)(p->m_items->data[it]);
            if (!pControl->m_bVisible)
                continue;
            if (pControl->m_bFloat) {
                ZuiControlCall(Proc_Layout_SetFloatPos, cp, (void *)it, 0, 0);
            }
            else {
				RECT rcCtrl;
                SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };
                if (sz.cx < (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0);
                if (sz.cx > (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0);
                if (sz.cy < (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0)) sz.cy = (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0);
                if (sz.cy > (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0)) sz.cy = (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0);
				rcCtrl.left = rc.left;
				rcCtrl.top = rc.top;
				rcCtrl.right = rc.left + sz.cx;
				rcCtrl.bottom = rc.top + sz.cy;
                ZuiControlCall(Proc_SetPos, pControl, &rcCtrl, FALSE, 0);
            }
        }
        break;
    }
    case Proc_FindControl: {
		ZuiControl pResult = NULL;
        // Check if this guy is valid
        if (((UINT)Param3 & ZFIND_VISIBLE) != 0 && !cp->m_bVisible) return NULL;
        if (((UINT)Param3 & ZFIND_ENABLED) != 0 && !cp->m_bEnabled) return NULL;
        if (((UINT)Param3 & ZFIND_HITTEST) != 0 && !ZuiIsPointInRect(&cp->m_rcItem, Param2)) return NULL;
        if (((UINT)Param3 & ZFIND_UPDATETEST) != 0 && ((FINDCONTROLPROC)Param1)(cp, Param2) != NULL) return NULL;

        
        if (p->m_pVerticalScrollBar != NULL) pResult = (ZuiControl)ZuiControlCall(Proc_FindControl, p->m_pVerticalScrollBar, Param1, Param2, Param3);
        if (pResult == NULL && p->m_pHorizontalScrollBar != NULL) pResult = (ZuiControl)ZuiControlCall(Proc_FindControl, p->m_pHorizontalScrollBar, Param1, Param2, Param3);
        if (pResult != NULL) return pResult;

        if (((UINT)Param3 & ZFIND_ME_FIRST) != 0) {
            if (((UINT)Param3 & ZFIND_HITTEST) == 0 || cp->m_bMouseEnabled) pResult = ((FINDCONTROLPROC)Param1)(cp, Param2);
        }
        if (pResult != NULL) return pResult;

        if (((UINT)Param3 & ZFIND_HITTEST) == 0 || p->m_bMouseChildEnabled) {
            RECT rc = cp->m_rcItem;
            rc.left += p->m_rcInset.left;
            rc.top += p->m_rcInset.top;
            rc.right -= p->m_rcInset.right;
            rc.bottom -= p->m_rcInset.bottom;
            if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) rc.right -= (ZuiInt)ZuiControlCall(Proc_GetFixedWidth, p->m_pVerticalScrollBar, NULL, NULL, NULL);
            if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) rc.bottom -= (ZuiInt)ZuiControlCall(Proc_GetFixedHeight, p->m_pHorizontalScrollBar, NULL, NULL, NULL);

            if (((UINT)Param3 & ZFIND_TOP_FIRST) != 0) {
                for (int it = darray_len(p->m_items) - 1; it >= 0; it--) {
                    pResult = (ZuiControl)ZuiControlCall(Proc_FindControl, (ZuiControl)(p->m_items->data[it]), Param1, Param2, Param3);
                    if (pResult != NULL) {
                        if (((UINT)Param3 & ZFIND_HITTEST) != 0 && !pResult->m_bFloat && !ZuiIsPointInRect(&rc, Param2))
                            continue;
                        else
                            return pResult;
                    }
                }
            }
            else {
                for (int it = 0; it < darray_len(p->m_items); it++) {
                    pResult = (ZuiControl)ZuiControlCall(Proc_FindControl, (ZuiControl)(p->m_items->data[it]), Param1, Param2, Param3);
                    if (pResult != NULL) {
                        if (((UINT)Param3 & ZFIND_HITTEST) != 0 && !pResult->m_bFloat && !ZuiIsPointInRect(&rc, Param2))
                            continue;
                        else
                            return pResult;
                    }
                }
            }
        }

        pResult = NULL;
        if (pResult == NULL && ((UINT)Param3 & ZFIND_ME_FIRST) == 0) {
            if (((UINT)Param3 & ZFIND_HITTEST) == 0 || cp->m_bMouseEnabled) pResult = ((FINDCONTROLPROC)Param1)(cp, Param2);
        }
        return pResult;
        break;
    }
    case Proc_OnPaint: {
        //这里是所有绘制的调度中心
        RECT rcTemp = { 0 };
		ZuiRegion OldRgn;
        if (!IntersectRect(&rcTemp, (RECT *)Param2, &cp->m_rcItem))
            //不在绘制区域
            return 0;
        //保存原始剪裁区
        OldRgn = ZuiCreateRegion();
        ZuiGraphicsGetClipRegion((ZuiGraphics)Param1, OldRgn);
        //设置新剪裁区

        ZRect rcClip;
        MAKEZRECT(rcClip, rcTemp.left, rcTemp.top, rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top);
        ZuiRegion rgn = ZuiCreateRegionRect(&rcClip);
        ZuiGraphicsSetClipRegion((ZuiGraphics)Param1, rgn, 1);


        p->old_call(Proc_OnPaint, cp, 0, Param1, Param2, 0);
        if (darray_len(p->m_items) > 0) {
            RECT rc = cp->m_rcItem;
            rc.left += p->m_rcInset.left;
            rc.top += p->m_rcInset.top;
            rc.right -= p->m_rcInset.right;
            rc.bottom -= p->m_rcInset.bottom;
            if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) rc.right -= (ZuiInt)ZuiControlCall(Proc_GetFixedWidth, p->m_pVerticalScrollBar, NULL, NULL, NULL);
            if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) rc.bottom -= (ZuiInt)ZuiControlCall(Proc_GetFixedHeight, p->m_pHorizontalScrollBar, NULL, NULL, NULL);

            if (!IntersectRect(&rcTemp, (RECT *)Param2, &rc)) {
                for (int it = 0; it < darray_len(p->m_items); it++) {
                    ZuiControl pControl = (ZuiControl)(p->m_items->data[it]);
                    if (!pControl->m_bVisible) continue;
                    if (!IntersectRect(&rcTemp, (RECT *)Param2, (RECT *)ZuiControlCall(Proc_GetPos, pControl, 0, 0, 0))) continue;
                    if (pControl->m_bFloat) {
                        if (!IntersectRect(&rcTemp, &cp->m_rcItem, (RECT *)ZuiControlCall(Proc_GetPos, pControl, 0, 0, 0))) continue;
                        ZuiControlCall(Proc_OnPaint, pControl, Param1, Param2, Param3);
                    }
                }
            }
            else {
                //保存原始剪裁区
                ZuiRegion OldRgn_child = ZuiCreateRegion();
                ZuiGraphicsGetClipRegion((ZuiGraphics)Param1, OldRgn_child);
                //设置子剪裁区
                MAKEZRECT(rcClip, rcTemp.left, rcTemp.top, rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top);
                ZuiRegion rgn_child = ZuiCreateRegionRect(&rcClip);
                ZuiGraphicsSetClipRegion((ZuiGraphics)Param1, rgn_child, 1);

                for (int it = 0; it < darray_len(p->m_items); it++) {
                    ZuiControl pControl = (ZuiControl)(p->m_items->data[it]);
                    if (!pControl->m_bVisible) continue;
                    if (!IntersectRect(&rcTemp, (RECT *)Param2, (RECT *)ZuiControlCall(Proc_GetPos, pControl, 0, 0, 0))) continue;
                    if (pControl->m_bFloat) {
                        if (!IntersectRect(&rcTemp, (RECT *)Param2, (RECT *)ZuiControlCall(Proc_GetPos, pControl, 0, 0, 0))) continue;
                        ZuiGraphicsSetClipRegion((ZuiGraphics)Param1, OldRgn_child, 0);
                        ZuiControlCall(Proc_OnPaint, pControl, Param1, Param2, Param3);
                        ZuiGraphicsSetClipRegion((ZuiGraphics)Param1, rgn_child, 0);
                    }
                    else {
                        if (!IntersectRect(&rcTemp, &rc, (RECT *)ZuiControlCall(Proc_GetPos, pControl, 0, 0, 0))) continue;
                        if (pControl->m_aAnime)
                            pControl->m_aAnime->OnPaint(pControl, Param1, Param2);
                        else
                            ZuiControlCall(Proc_OnPaint, pControl, Param1, &pControl->m_rcItem, Param3);
                    }
                }

                //恢复剪裁区
                ZuiGraphicsSetClipRegion((ZuiGraphics)Param1, OldRgn_child, 0);
                ZuiDestroyRegion(OldRgn_child);
                ZuiDestroyRegion(rgn_child);
            }
        }
        //绘制滚动条
        if (p->m_pVerticalScrollBar != NULL && p->m_pVerticalScrollBar->m_bVisible) {
            if (IntersectRect(&rcTemp, Param2, (RECT *)ZuiControlCall(Proc_GetPos, p->m_pVerticalScrollBar, 0, 0, 0))) {
                ZuiControlCall(Proc_OnPaint, p->m_pVerticalScrollBar, Param1, Param2, Param3);
            }
        }

        if (p->m_pHorizontalScrollBar != NULL && p->m_pHorizontalScrollBar->m_bVisible) {
            if (IntersectRect(&rcTemp, Param2, (RECT *)ZuiControlCall(Proc_GetPos, p->m_pHorizontalScrollBar, 0, 0, 0))) {
                ZuiControlCall(Proc_OnPaint, p->m_pHorizontalScrollBar, Param1, Param2, Param3);
            }
        }


        //恢复剪裁区

        ZuiGraphicsSetClipRegion((ZuiGraphics)Param1, OldRgn, 0);
        ZuiDestroyRegion(OldRgn);
        ZuiDestroyRegion(rgn);
        return 0;//绘图完毕,不需要默认
    }
    case Proc_Layout_SetInset: {
        memcpy(&p->m_rcInset, Param1, sizeof(RECT));
        ZuiControlNeedParentUpdate(cp);
        break;
    }
    case Proc_SetAttribute: {
        if (wcscmp(Param1, _T("inset")) == 0) {
            RECT rcInset = { 0 };
            LPTSTR pstr = NULL;
            rcInset.left = _tcstol(Param2, &pstr, 10);  ASSERT(pstr);
            rcInset.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcInset.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcInset.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZuiControlCall(Proc_Layout_SetInset, cp, &rcInset, NULL, NULL);
        }
        else if (wcscmp(Param1, _T("vscrollbar")) == 0) {
            ZuiControlCall(Proc_Layout_EnableScrollBar, cp, _tcscmp(Param2, _T("true")) == 0, ZuiControlCall(Proc_Layout_GetHorizontalScrollBar, cp, NULL, NULL, NULL) != NULL, NULL);
        }
        else if (wcscmp(Param1, _T("hscrollbar")) == 0) {
            ZuiControlCall(Proc_Layout_EnableScrollBar, cp, ZuiControlCall(Proc_Layout_GetVerticalScrollBar, cp, NULL, NULL, NULL) != NULL, _tcscmp(Param2, _T("true")) == 0, NULL);
        }
        break;
    }
    case Proc_JsPut: {
        js_State *J = Param2;
        if (wcscmp(Param1, L"inset") == 0) {
            RECT rcPadding = { 0 };
            LPTSTR pstr = NULL;
            rcPadding.left = _tcstol(js_tostring(J, -1), &pstr, 10);  ASSERT(pstr);
            rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZuiControlCall(Proc_Layout_SetInset, p, &rcPadding, NULL, NULL);
        }

        break;
    }
    case Proc_JsHas: {
        js_State *J = Param2;
        if (wcscmp(Param1, L"Add") == 0)
            return 1;
        else if (wcscmp(Param1, L"AddAt") == 0)
            return 1;
        else if (wcscmp(Param1, L"GetItemIndex") == 0)
            return 1;
        else if (wcscmp(Param1, L"GetItemAt") == 0)
            return 1;
        else if (wcscmp(Param1, L"count") == 0) js_pushnumber(J, (int)ZuiControlCall(Proc_Layout_GetCount, cp, NULL, NULL, NULL));
        else {
            for (int it = darray_len(p->m_items) - 1; it >= 0; it--) {
                if (((ZuiControl)(p->m_items->data[it]))->m_sName)
                {
                    if (wcscmp(Param1, ((ZuiControl)(p->m_items->data[it]))->m_sName) == 0) {
                        ZuiBuilderJs_pushControl(Param2, p->m_items->data[it]);
                        return 0;
                        continue;
                    }
                }
            }
        }
        break;
    }
    case Proc_JsCall: {
        if (wcscmp(Param1, L"Add") == 0) {
            ZuiControl ite = ZuiBuilderJs_toControl(Param2, -1);
            ZuiControlCall(Proc_Layout_Add, cp, ite, NULL, NULL);
        }
        else if (wcscmp(Param1, L"AddAt") == 0) {
            ZuiControl ite = ZuiBuilderJs_toControl(Param2, 1);
            ZuiControlCall(Proc_Layout_AddAt, cp, ite, js_toint32(Param2, 2), NULL);
        }
        else if (wcscmp(Param1, L"GetItemIndex") == 0) {
            ZuiControl ite = ZuiBuilderJs_toControl(Param2, -1);
            ZuiInt index = ZuiControlCall(Proc_Layout_GetItemIndex, cp, ite, NULL, NULL);
            js_pushnumber(Param2, index);
        }
        else if (wcscmp(Param1, L"GetItemAt") == 0) {
            ZuiControl ite = ZuiControlCall(Proc_Layout_GetItemAt, cp, js_toint32(Param2, -1), NULL, NULL);
            if (ite)
                ZuiBuilderJs_pushControl(Param2, ite);
            else
                js_pushundefined(Param2);
        }
        break;
    }
    case Proc_SetManager: {
        for (int it = 0; it < darray_len(p->m_items); it++) {
            ZuiControlCall(Proc_SetManager, (ZuiControl)(p->m_items->data[it]), Param1, cp, Param3);
        }
        if (p->m_pVerticalScrollBar != NULL) ZuiControlCall(Proc_SetManager, p->m_pVerticalScrollBar, Param1, cp, Param3);
        if (p->m_pHorizontalScrollBar != NULL) ZuiControlCall(Proc_SetManager, p->m_pHorizontalScrollBar, Param1, cp, Param3);
        break;
    }
    case Proc_Layout_GetScrollPos: {
        SIZE sz = { 0, 0 };
        if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) sz.cy = ZuiControlCall(Proc_ScrollBar_GetScrollPos, p->m_pVerticalScrollBar, NULL, NULL, NULL);
        if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) sz.cx = ZuiControlCall(Proc_ScrollBar_GetScrollPos, p->m_pHorizontalScrollBar, NULL, NULL, NULL);
        return &sz;
        break;
    }
    case Proc_Layout_GetScrollRange: {
        SIZE sz = { 0, 0 };
        if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) sz.cy = ZuiControlCall(Proc_ScrollBar_GetScrollRange, p->m_pVerticalScrollBar, NULL, NULL, NULL);
        if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) sz.cx = ZuiControlCall(Proc_ScrollBar_GetScrollRange, p->m_pHorizontalScrollBar, NULL, NULL, NULL);
        return &sz;
        break;
    }
    case Proc_Layout_SetScrollPos: {
        SIZE *szPos = Param1;
		RECT rcPos;
        int cx = 0;
        int cy = 0;
        if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) {
            int iLastScrollPos = ZuiControlCall(Proc_ScrollBar_GetScrollPos, p->m_pVerticalScrollBar, NULL, NULL, NULL);
            ZuiControlCall(Proc_ScrollBar_SetScrollPos, p->m_pVerticalScrollBar, szPos->cy, NULL, NULL);
            cy = (ZuiInt)ZuiControlCall(Proc_ScrollBar_GetScrollPos, p->m_pVerticalScrollBar, NULL, NULL, NULL) - iLastScrollPos;
        }

        if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) {
            int iLastScrollPos = ZuiControlCall(Proc_ScrollBar_GetScrollPos, p->m_pHorizontalScrollBar, NULL, NULL, NULL);
            ZuiControlCall(Proc_ScrollBar_SetScrollPos, p->m_pHorizontalScrollBar, szPos->cx, NULL, NULL);
            cx = (ZuiInt)ZuiControlCall(Proc_ScrollBar_GetScrollPos, p->m_pHorizontalScrollBar, NULL, NULL, NULL) - iLastScrollPos;
        }

        if (cx == 0 && cy == 0) return;

        for (int it2 = 0; it2 < darray_len(p->m_items); it2++) {
            ZuiControl pControl = (ZuiControl)(p->m_items->data[it2]);
            if (!pControl->m_bVisible) continue;
            if (pControl->m_bFloat) continue;
            RECT *prcPos = (RECT *)ZuiControlCall(Proc_GetPos, pControl, NULL, NULL, NULL);
            rcPos = *prcPos;
            rcPos.left -= cx;
            rcPos.right -= cx;
            rcPos.top -= cy;
            rcPos.bottom -= cy;
            ZuiControlCall(Proc_SetPos, pControl, &rcPos, NULL, NULL);
        }

        ZuiControlInvalidate(cp,TRUE);
        break;
    }
    case Proc_Layout_SetScrollStepSize: {
        if (Param1 > 0)
            p->m_nScrollStepSize = Param1;
        break;
    }
    case Proc_Layout_GetScrollStepSize: {
        return p->m_nScrollStepSize;
        break;
    }
    case Proc_Layout_LineUp: {
        int cyLine = p->m_nScrollStepSize;
        if (p->m_nScrollStepSize == 0)
        {
            cyLine = 8;
            //if (m_pManager) cyLine = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8;
        }

        SIZE *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        SIZE sz = { psz->cx, psz->cy };
        sz.cy -= cyLine;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_LineDown: {
        int cyLine = p->m_nScrollStepSize;
        if (p->m_nScrollStepSize == 0)
        {
            cyLine = 8;
            //if (m_pManager) cyLine = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8;
        }

        SIZE *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        SIZE sz = { psz->cx, psz->cy };
        sz.cy += cyLine;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_PageUp: {
        SIZE *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        SIZE sz = { psz->cx, psz->cy };
        int iOffset = cp->m_rcItem.bottom - cp->m_rcItem.top - p->m_rcInset.top - p->m_rcInset.bottom;
        if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) iOffset -= (ZuiInt)ZuiControlCall(Proc_GetFixedHeight, p->m_pHorizontalScrollBar, NULL, NULL, NULL);
        sz.cy -= iOffset;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_PageDown: {
        SIZE *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        SIZE sz = { psz->cx, psz->cy };
        int iOffset = cp->m_rcItem.bottom - cp->m_rcItem.top - p->m_rcInset.top - p->m_rcInset.bottom;
        if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) iOffset -= (ZuiInt)ZuiControlCall(Proc_GetFixedHeight, p->m_pHorizontalScrollBar, NULL, NULL, NULL);
        sz.cy += iOffset;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_HomeUp: {
        SIZE *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        SIZE sz = { psz->cx, psz->cy };
        sz.cy = 0;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_EndDown: {
        SIZE *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        SIZE sz = { psz->cx, psz->cy };
        sz.cy = ((SIZE *)ZuiControlCall(Proc_Layout_GetScrollRange, cp, NULL, NULL, NULL))->cy;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_LineLeft: {
        int cxLine = p->m_nScrollStepSize == 0 ? 8 : p->m_nScrollStepSize;

        SIZE *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        SIZE sz = { psz->cx, psz->cy };
        sz.cx -= cxLine;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_LineRight: {
        int cxLine = p->m_nScrollStepSize == 0 ? 8 : p->m_nScrollStepSize;

        SIZE *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        SIZE sz = { psz->cx, psz->cy };
        sz.cx += cxLine;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_PageLeft: {
        SIZE *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        SIZE sz = { psz->cx, psz->cy };
        int iOffset = cp->m_rcItem.right - cp->m_rcItem.left - p->m_rcInset.left - p->m_rcInset.right;
        if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) iOffset -= (ZuiInt)ZuiControlCall(Proc_GetFixedWidth, p->m_pVerticalScrollBar, NULL, NULL, NULL);
        sz.cx -= iOffset;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_PageRight: {
        SIZE *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        SIZE sz = { psz->cx, psz->cy };
        int iOffset = cp->m_rcItem.right - cp->m_rcItem.left - p->m_rcInset.left - p->m_rcInset.right;
        if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) iOffset -= (ZuiInt)ZuiControlCall(Proc_GetFixedWidth, p->m_pVerticalScrollBar, NULL, NULL, NULL);
        sz.cx += iOffset;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_HomeLeft: {
        SIZE *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        SIZE sz = { psz->cx, psz->cy };
        sz.cx = 0;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_EndRight: {
        SIZE *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        SIZE sz = { psz->cx, psz->cy };
        sz.cx = ((SIZE *)ZuiControlCall(Proc_Layout_GetScrollRange, cp, NULL, NULL, NULL))->cx;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_EnableScrollBar: {
        BOOL bEnableVertical = Param1;
        BOOL bEnableHorizontal = Param2;
        if (bEnableVertical && !p->m_pVerticalScrollBar) {
            p->m_pVerticalScrollBar = NewZuiControl(L"scrollbar", NULL, NULL, NULL);//创建滚动条
            ZuiControlCall(Proc_ScrollBar_SetOwner, p->m_pVerticalScrollBar, cp, NULL, NULL);
            p->m_pVerticalScrollBar->m_pManager = cp->m_pManager;

            //			m_pVerticalScrollBar->SetVisible(false);
        }
        else if (!bEnableVertical && p->m_pVerticalScrollBar) {
            FreeZuiControl(p->m_pVerticalScrollBar, NULL);
            p->m_pVerticalScrollBar = NULL;
        }

        if (bEnableHorizontal && !p->m_pHorizontalScrollBar) {
            p->m_pHorizontalScrollBar = NewZuiControl(L"scrollbar", NULL, NULL, NULL);//创建滚动条
            ZuiControlCall(Proc_ScrollBar_SetHorizontal, p->m_pHorizontalScrollBar, TRUE, NULL, NULL);
            ZuiControlCall(Proc_ScrollBar_SetOwner, p->m_pHorizontalScrollBar, cp, NULL, NULL);
            p->m_pHorizontalScrollBar->m_pManager = cp->m_pManager;
            //			m_pHorizontalScrollBar->SetVisible(false);

        }
        else if (!bEnableHorizontal && p->m_pHorizontalScrollBar) {
            FreeZuiControl(p->m_pHorizontalScrollBar, NULL);
            p->m_pHorizontalScrollBar = NULL;
        }

        ZuiControlNeedUpdate(cp);
        break;
    }
    case Proc_Layout_GetVerticalScrollBar: {
        return p->m_pVerticalScrollBar;
        break;
    }
    case Proc_Layout_GetHorizontalScrollBar: {
        return p->m_pHorizontalScrollBar;
        break;
    }
    case Proc_Layout_ProcessScrollBar: {
        RECT *rc = Param1;
        int cxRequired = Param2;
        int cyRequired = Param3;

        while (p->m_pHorizontalScrollBar)
        {
            if (cxRequired > rc->right - rc->left && !p->m_pHorizontalScrollBar->m_bVisible)
            {
                ZuiControlCall(Proc_SetVisible, p->m_pHorizontalScrollBar, TRUE, NULL, NULL);
                ZuiControlCall(Proc_ScrollBar_SetScrollRange, p->m_pHorizontalScrollBar, cxRequired - (rc->right - rc->left), NULL, NULL);
                ZuiControlCall(Proc_ScrollBar_SetScrollPos, p->m_pHorizontalScrollBar, 0, NULL, NULL);
                p->m_bScrollProcess = TRUE;
                ZuiControlCall(Proc_SetPos, cp, &cp->m_rcItem, FALSE, 0);
                p->m_bScrollProcess = FALSE;
                break;
            }

            if (!p->m_pHorizontalScrollBar->m_bVisible) break;

            int cxScroll = cxRequired - (rc->right - rc->left);
            if (cxScroll <= 0 && !p->m_bScrollProcess)
            {
                ZuiControlCall(Proc_SetVisible, p->m_pHorizontalScrollBar, FALSE, NULL, NULL);
                ZuiControlCall(Proc_ScrollBar_SetScrollPos, p->m_pHorizontalScrollBar, 0, NULL, NULL);
                ZuiControlCall(Proc_ScrollBar_SetScrollRange, p->m_pHorizontalScrollBar, 0, NULL, NULL);
                ZuiControlCall(Proc_SetPos, cp, &cp->m_rcItem, FALSE, 0);
            }
            else
            {
                RECT rcScrollBarPos = { rc->left, rc->bottom, rc->right, rc->bottom + (ZuiInt)ZuiControlCall(Proc_GetFixedWidth, p->m_pHorizontalScrollBar, NULL, NULL, NULL) };
                ZuiControlCall(Proc_SetPos, p->m_pHorizontalScrollBar, &rcScrollBarPos, FALSE, 0);

                if (ZuiControlCall(Proc_ScrollBar_GetScrollRange, p->m_pHorizontalScrollBar, NULL, NULL, NULL) != cxScroll) {
                    int iScrollPos = ZuiControlCall(Proc_ScrollBar_GetScrollPos, p->m_pHorizontalScrollBar, NULL, NULL, NULL);
                    ZuiControlCall(Proc_ScrollBar_SetScrollRange, p->m_pHorizontalScrollBar, abs(cxScroll), NULL, NULL);
                    if (ZuiControlCall(Proc_ScrollBar_GetScrollRange, p->m_pHorizontalScrollBar, NULL, NULL, NULL) == 0) {
                        ZuiControlCall(Proc_SetVisible, p->m_pHorizontalScrollBar, FALSE, NULL, NULL);
                        ZuiControlCall(Proc_ScrollBar_SetScrollPos, p->m_pHorizontalScrollBar, 0, NULL, NULL);
                    }
                    if (iScrollPos > ZuiControlCall(Proc_ScrollBar_GetScrollPos, p->m_pHorizontalScrollBar, NULL, NULL, NULL)) {
                        ZuiControlCall(Proc_SetPos, cp, &cp->m_rcItem, FALSE, 0);
                    }
                }
            }
            break;
        }

        while (p->m_pVerticalScrollBar)
        {
            if (cyRequired > rc->bottom - rc->top && !p->m_pVerticalScrollBar->m_bVisible) {
                ZuiControlCall(Proc_SetVisible, p->m_pVerticalScrollBar, TRUE, NULL, NULL);
                ZuiControlCall(Proc_ScrollBar_SetScrollRange, p->m_pVerticalScrollBar, cyRequired - (rc->bottom - rc->top), NULL, NULL);
                ZuiControlCall(Proc_ScrollBar_SetScrollPos, p->m_pVerticalScrollBar, 0, NULL, NULL);
                p->m_bScrollProcess = TRUE;
                ZuiControlCall(Proc_SetPos, cp, &cp->m_rcItem, FALSE, 0);
                p->m_bScrollProcess = FALSE;
                break;
            }
            // No scrollbar required
            if (!p->m_pVerticalScrollBar->m_bVisible) break;

            // Scroll not needed anymore?
            int cyScroll = cyRequired - (rc->bottom - rc->top);
            if (cyScroll <= 0 && !p->m_bScrollProcess) {
                ZuiControlCall(Proc_SetVisible, p->m_pVerticalScrollBar, FALSE, NULL, NULL);
                ZuiControlCall(Proc_ScrollBar_SetScrollPos, p->m_pVerticalScrollBar, 0, NULL, NULL);
                ZuiControlCall(Proc_ScrollBar_SetScrollRange, p->m_pVerticalScrollBar, 0, NULL, NULL);
                ZuiControlCall(Proc_SetPos, cp, &cp->m_rcItem, FALSE, 0);
            }
            else
            {
                RECT rcScrollBarPos = { rc->right, rc->top, rc->right + (ZuiInt)ZuiControlCall(Proc_GetFixedWidth, p->m_pVerticalScrollBar, NULL, NULL, NULL), rc->bottom };
                ZuiControlCall(Proc_SetPos, p->m_pVerticalScrollBar, &rcScrollBarPos, FALSE, 0);

                if (ZuiControlCall(Proc_ScrollBar_GetScrollRange, p->m_pVerticalScrollBar, NULL, NULL, NULL) != cyScroll) {
                    int iScrollPos = ZuiControlCall(Proc_ScrollBar_GetScrollPos, p->m_pVerticalScrollBar, NULL, NULL, NULL);
                    ZuiControlCall(Proc_ScrollBar_SetScrollRange, p->m_pVerticalScrollBar, abs(cyScroll), NULL, NULL);
                    if (ZuiControlCall(Proc_ScrollBar_GetScrollRange, p->m_pVerticalScrollBar, NULL, NULL, NULL) == 0) {
                        ZuiControlCall(Proc_SetVisible, p->m_pVerticalScrollBar, FALSE, NULL, NULL);
                        ZuiControlCall(Proc_ScrollBar_SetScrollPos, p->m_pVerticalScrollBar, 0, NULL, NULL);
                    }
                    if (iScrollPos > ZuiControlCall(Proc_ScrollBar_GetScrollPos, p->m_pVerticalScrollBar, NULL, NULL, NULL)) {
                        ZuiControlCall(Proc_SetPos, cp, &cp->m_rcItem, FALSE, 0);
                    }
                }
            }
            break;
        }
    }
                                       break;
    case Proc_CoreUnInit: {
        return NULL;
        break;
    }
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
}


