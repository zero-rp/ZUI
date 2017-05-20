#include "Layout.h"
#include <core/control.h>
#include <stdlib.h>

void* ZCALL ZuiLayoutProc(int ProcId, ZuiControl cp, ZuiLayout p, void* Param1, void* Param2, void* Param3) {
    switch (ProcId)
    {
    case Proc_FindControl: {
        ZuiControl pResult = NULL;
        // Check if this guy is valid
        if (((ZuiUInt)Param3 & ZFIND_VISIBLE) != 0 && !cp->m_bVisible) return NULL;
        if (((ZuiUInt)Param3 & ZFIND_ENABLED) != 0 && !cp->m_bEnabled) return NULL;
        if (((ZuiUInt)Param3 & ZFIND_HITTEST) != 0 && !ZuiIsPointInRect(&cp->m_rcItem, Param2)) return NULL;
        if (((ZuiUInt)Param3 & ZFIND_UPDATETEST) != 0 && ((FINDCONTROLPROC)Param1)(cp, Param2) != NULL) return NULL;


        if (((ZuiUInt)Param3 & ZFIND_ME_FIRST) != 0) {
            if (((ZuiUInt)Param3 & ZFIND_HITTEST) == 0 || cp->m_bMouseEnabled) pResult = ((FINDCONTROLPROC)Param1)(cp, Param2);
        }
        if (pResult != NULL) return pResult;

        if (((ZuiUInt)Param3 & ZFIND_HITTEST) == 0 || p->m_bMouseChildEnabled) {
            ZRect rc = cp->m_rcItem;
            rc.left += p->m_rcInset.left;
            rc.top += p->m_rcInset.top;
            rc.right -= p->m_rcInset.right;
            rc.bottom -= p->m_rcInset.bottom;
            if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) rc.right -= (ZuiInt)ZuiControlCall(Proc_GetFixedWidth, p->m_pVerticalScrollBar, NULL, NULL, NULL);
            if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) rc.bottom -= (ZuiInt)ZuiControlCall(Proc_GetFixedHeight, p->m_pHorizontalScrollBar, NULL, NULL, NULL);

            if (((ZuiUInt)Param3 & ZFIND_TOP_FIRST) != 0) {
                for (int it = darray_len(p->m_items) - 1; it >= 0; it--) {
                    pResult = (ZuiControl)ZuiControlCall(Proc_FindControl, (ZuiControl)(p->m_items->data[it]), Param1, Param2, Param3);
                    if (pResult != NULL) {
                        if (((ZuiUInt)Param3 & ZFIND_HITTEST) != 0 && !pResult->m_bFloat && !ZuiIsPointInRect(&rc, Param2))
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
                        if (((ZuiUInt)Param3 & ZFIND_HITTEST) != 0 && !pResult->m_bFloat && !ZuiIsPointInRect(&rc, Param2))
                            continue;
                        else
                            return pResult;
                    }
                }
            }
        }

        if (p->m_pVerticalScrollBar != NULL) pResult = (ZuiControl)ZuiControlCall(Proc_FindControl, p->m_pVerticalScrollBar, Param1, Param2, Param3);
        if (pResult == NULL && p->m_pHorizontalScrollBar != NULL) pResult = (ZuiControl)ZuiControlCall(Proc_FindControl, p->m_pHorizontalScrollBar, Param1, Param2, Param3);
        if (pResult != NULL)
            return pResult;

        pResult = NULL;
        if (pResult == NULL && ((ZuiUInt)Param3 & ZFIND_ME_FIRST) == 0) {
            if (((ZuiUInt)Param3 & ZFIND_HITTEST) == 0 || cp->m_bMouseEnabled) pResult = ((FINDCONTROLPROC)Param1)(cp, Param2);
        }
        return pResult;
    }
    case Proc_OnPaint: {
        //这里是所有绘制的调度中心
        ZRect rcTemp = { 0 };
        ZuiRect rc = Param2;
        if (!IntersectRect(&rcTemp, (ZRect *)Param2, &cp->m_rcItem))
            //不在绘制区域
            return 0;
        //保存原始剪裁区
        ZRectR OldBox;
        ZuiGraphicsGetClipBox((ZuiGraphics)Param1, &OldBox);
        //设置新剪裁区

        ZRectR rcClip;
        MAKEZRECT(rcClip, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom);
        ZuiGraphicsSetClipBox((ZuiGraphics)Param1, &rcClip);

        p->old_call(Proc_OnPaint, cp, 0, Param1, Param2, 0);
        if (darray_len(p->m_items) > 0) {
            ZRect rc = cp->m_rcItem;
            rc.left += p->m_rcInset.left;
            rc.top += p->m_rcInset.top;
            rc.right -= p->m_rcInset.right;
            rc.bottom -= p->m_rcInset.bottom;
            if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) rc.right -= (ZuiInt)ZuiControlCall(Proc_GetFixedWidth, p->m_pVerticalScrollBar, NULL, NULL, NULL);
            if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) rc.bottom -= (ZuiInt)ZuiControlCall(Proc_GetFixedHeight, p->m_pHorizontalScrollBar, NULL, NULL, NULL);

            if (!IntersectRect(&rcTemp, (ZRect *)Param2, &rc)) {
                for (int it = 0; it < darray_len(p->m_items); it++) {
                    ZuiControl pControl = (ZuiControl)(p->m_items->data[it]);
                    if (!pControl->m_bVisible) continue;
                    if (!IntersectRect(&rcTemp, (ZRect *)Param2, (ZRect *)ZuiControlCall(Proc_GetPos, pControl, 0, 0, 0))) continue;
                    if (pControl->m_bFloat) {
                        if (!IntersectRect(&rcTemp, &cp->m_rcItem, (ZRect *)ZuiControlCall(Proc_GetPos, pControl, 0, 0, 0))) continue;
                        IntersectRect(&rcTemp, (ZRect *)Param2, &pControl->m_rcItem);
                        MAKEZRECT(rcClip, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom);
                        ZuiGraphicsSetClipBox((ZuiGraphics)Param1, &rcClip);
                        ZuiControlCall(Proc_OnPaint, pControl, Param1, &rcTemp, Param3);
                    }
                }
            }
            else {
                //保存原始剪裁区
                ZRectR OldBox_child;
                ZuiGraphicsGetClipBox((ZuiGraphics)Param1, &OldBox_child);
                //设置子剪裁区
                MAKEZRECT(rcClip, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom);
                ZuiGraphicsSetClipBox((ZuiGraphics)Param1, &rcClip);

                for (int it = 0; it < darray_len(p->m_items); it++) {
                    ZuiControl pControl = (ZuiControl)(p->m_items->data[it]);
                    if (!pControl->m_bVisible) continue;
                    if (!IntersectRect(&rcTemp, (ZRect *)Param2, (ZRect *)ZuiControlCall(Proc_GetPos, pControl, 0, 0, 0))) continue;
                    if (pControl->m_bFloat) {
                        if (!IntersectRect(&rcTemp, (ZRect *)Param2, (ZRect *)ZuiControlCall(Proc_GetPos, pControl, 0, 0, 0)))
                            continue;
                        IntersectRect(&rcTemp, (ZRect *)Param2, &pControl->m_rcItem);
                        MAKEZRECT(rcClip, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom);
                        ZuiGraphicsSetClipBox((ZuiGraphics)Param1, &rcClip);
                        if (pControl->m_aAnime)
                            pControl->m_aAnime->OnPaint(pControl, Param1, Param2);
                        else
                            ZuiControlCall(Proc_OnPaint, pControl, Param1, &rcTemp, Param3);
                        ZuiGraphicsSetClipBox((ZuiGraphics)Param1, &OldBox_child);
                    }
                    else {
                        if (!IntersectRect(&rcTemp, &rc, (ZRect *)ZuiControlCall(Proc_GetPos, pControl, 0, 0, 0))) continue;
                        if (pControl->m_aAnime)
                            pControl->m_aAnime->OnPaint(pControl, Param1, Param2);
                        else {
                            IntersectRect(&rcTemp, (ZRect *)Param2, &pControl->m_rcItem);
                            MAKEZRECT(rcClip, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom);
                            ZuiGraphicsSetClipBox((ZuiGraphics)Param1, &rcClip);
                            ZuiControlCall(Proc_OnPaint, pControl, Param1, &rcTemp, Param3);
                        }
                    }
                }

                //恢复剪裁区
                ZuiGraphicsSetClipBox((ZuiGraphics)Param1, &OldBox_child);
            }
        }
        //绘制滚动条
        if (p->m_pVerticalScrollBar != NULL && p->m_pVerticalScrollBar->m_bVisible) {
            if (IntersectRect(&rcTemp, Param2, (ZRect *)ZuiControlCall(Proc_GetPos, p->m_pVerticalScrollBar, 0, 0, 0))) {
                ZuiControlCall(Proc_OnPaint, p->m_pVerticalScrollBar, Param1, Param2, Param3);
            }
        }

        if (p->m_pHorizontalScrollBar != NULL && p->m_pHorizontalScrollBar->m_bVisible) {
            if (IntersectRect(&rcTemp, Param2, (ZRect *)ZuiControlCall(Proc_GetPos, p->m_pHorizontalScrollBar, 0, 0, 0))) {
                ZuiControlCall(Proc_OnPaint, p->m_pHorizontalScrollBar, Param1, Param2, Param3);
            }
        }
        //ZuiControlCall(Proc_EndPaint, cp, Param1, Param2, 0);


        //恢复剪裁区
        ZuiGraphicsSetClipBox((ZuiGraphics)Param1, &OldBox);
        return 0;//绘图完毕,不需要默认
    }
    case Proc_SetPos: {
        ZRect rc;
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
                ZRect rcCtrl;
                ZSize sz = { rc.right - rc.left, rc.bottom - rc.top };
                if (sz.cx < (ZuiInt)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0)) sz.cx = (ZuiInt)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0);
                if (sz.cx > (ZuiInt)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0)) sz.cx = (ZuiInt)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0);
                if (sz.cy < (ZuiInt)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0)) sz.cy = (ZuiInt)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0);
                if (sz.cy > (ZuiInt)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0)) sz.cy = (ZuiInt)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0);
                rcCtrl.left = rc.left;
                rcCtrl.top = rc.top;
                rcCtrl.right = rc.left + sz.cx;
                rcCtrl.bottom = rc.top + sz.cy;
                ZuiControlCall(Proc_SetPos, pControl, &rcCtrl, FALSE, 0);
            }
        }
        break;
    }
#if (defined HAVE_JS) && (HAVE_JS == 1)
    case Proc_JsGet: {
        duk_context *ctx = (duk_context *)Param1;
        switch ((ZuiInt)Param2)
        {
        case Js_Id_Layout_count: {
            duk_push_int(ctx, (int)ZuiControlCall(Proc_Layout_GetCount, cp, NULL, NULL, NULL));
            return 1;
        }
        case Js_Id_Layout_inset: {
            return 0;
        }
        default:
            break;
        }
        break;
    }
    case Proc_JsSet: {
        duk_context *ctx = (duk_context *)Param1;
        switch ((ZuiInt)Param2)
        {
        case Js_Id_Layout_count: {
            return 0;
        }
        case Js_Id_Layout_inset: {
            ZRect rcPadding = { 0 };
            ZuiText pstr = NULL;
            rcPadding.left = _tcstol(duk_get_string_w(ctx, 0), &pstr, 10);  ASSERT(pstr);
            rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZuiControlCall(Proc_Layout_SetInset, cp, &rcPadding, NULL, NULL);
            return 0;
        }
        default:
            break;
        }
        break;
    }
    case Proc_JsCall: {
        duk_context *ctx = (duk_context *)Param1;
        switch ((ZuiInt)Param2)
        {
        case Js_Id_Layout_Add: {
            ZuiControl ite = ZuiBuilderJs_toControl(ctx, 0);
            duk_push_boolean(ctx, ZuiControlCall(Proc_Layout_Add, cp, ite, NULL, NULL));
            return 1;
        }
        case Js_Id_Layout_AddAt: {
            ZuiControl ite = ZuiBuilderJs_toControl(ctx, 0);
            duk_push_boolean(ctx, ZuiControlCall(Proc_Layout_AddAt, cp, ite, (ZuiAny)duk_to_int(ctx, 1), NULL));
            return 1;
        }
        case Js_Id_Layout_GetItemIndex: {
            ZuiControl ite = ZuiBuilderJs_toControl(ctx, 0);
            ZuiInt index = ZuiControlCall(Proc_Layout_GetItemIndex, cp, ite, NULL, NULL);
            duk_push_int(ctx, index);
            return 1;
        }
        case Js_Id_Layout_GetItemAt: {
            ZuiControl ite = ZuiControlCall(Proc_Layout_GetItemAt, cp, (ZuiAny)duk_to_int(ctx, 0), NULL, NULL);
            if (ite)
                ZuiBuilderJs_pushControl(ctx, ite);
            else
                duk_push_null(ctx);
            return 1;
        }
        case Js_Id_Layout_GetItemName: {
            if (duk_is_string(ctx, 0)) {
                ZuiControl fp = NULL;
                ZuiText name = duk_get_string_w(ctx, 0);
                for (int it = 0; it < darray_len(p->m_items); it++) {
                    ZuiControl pResult = (ZuiControl)(p->m_items->data[it]);
                    if (pResult->m_sName && wcscmp(name, pResult->m_sName) == 0) {
                        fp = (ZuiControl)(p->m_items->data[it]);
                        break;
                    }
                }
                if (!fp)
                    LOG_ERROR(L"Layout GetByName失败: Name:%ls\r\n", duk_get_string_w(ctx, 0));
                if (fp)
                {
                    duk_get_global_string(ctx, "Control");
                    duk_push_pointer(ctx, fp);
                    duk_new(ctx, 1);
                    return 1;
                }
            }
            duk_push_null(ctx);
            return 1;
        }
        default:
            break;
        }
        break;
    }
    case Proc_JsInit: {
        ZuiBuilderControlInit(Param1, "Add", Js_Id_Layout_Add, FALSE);
        ZuiBuilderControlInit(Param1, "AddAt", Js_Id_Layout_AddAt, FALSE);
        ZuiBuilderControlInit(Param1, "GetItemIndex", Js_Id_Layout_GetItemIndex, FALSE);
        ZuiBuilderControlInit(Param1, "GetItemAt", Js_Id_Layout_GetItemAt, FALSE);
        ZuiBuilderControlInit(Param1, "GetItemName", Js_Id_Layout_GetItemName, FALSE);

        ZuiBuilderControlInit(Param1, "count", Js_Id_Layout_count, TRUE);
        ZuiBuilderControlInit(Param1, "inset", Js_Id_Layout_inset, TRUE);
        break;
    }
#endif
    case Proc_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        //不响应鼠标消息
        if (!cp->m_bMouseEnabled && event->Type > ZEVENT__MOUSEBEGIN && event->Type < ZEVENT__MOUSEEND) {
            if (cp->m_pParent != NULL)
                ZuiControlCall(Proc_OnEvent, cp->m_pParent, Param1, NULL, NULL);
            else
                ZuiDefaultControlProc(Proc_OnEvent, cp, 0, Param1, NULL, NULL);
            return;
        }

        if (event->Type == ZEVENT_SETFOCUS)
        {
            p->m_bFocused = TRUE;
            return NULL;
        }
        if (event->Type == ZEVENT_KILLFOCUS)
        {
            p->m_bFocused = FALSE;
            return NULL;
        }
        if (p->m_pVerticalScrollBar != NULL && p->m_pVerticalScrollBar->m_bVisible && p->m_pVerticalScrollBar->m_bEnabled)
        {
            if (event->Type == ZEVENT_KEYDOWN)
            {
                switch (event->chKey) {
                case VK_DOWN:
                    return ZuiControlCall(Proc_Layout_LineDown, cp, NULL, NULL, NULL);
                case VK_UP:
                    return ZuiControlCall(Proc_Layout_LineUp, cp, NULL, NULL, NULL);
                case VK_NEXT:
                    return ZuiControlCall(Proc_Layout_PageDown, cp, NULL, NULL, NULL);
                case VK_PRIOR:
                    return ZuiControlCall(Proc_Layout_PageUp, cp, NULL, NULL, NULL);
                case VK_HOME:
                    return ZuiControlCall(Proc_Layout_HomeUp, cp, NULL, NULL, NULL);
                case VK_END:
                    return ZuiControlCall(Proc_Layout_EndDown, cp, NULL, NULL, NULL);
                case VK_LEFT:
                    return ZuiControlCall(Proc_Layout_LineLeft, cp, NULL, NULL, NULL);
                case VK_RIGHT:
                    return ZuiControlCall(Proc_Layout_LineRight, cp, NULL, NULL, NULL);
                }
            }
            else if (event->Type == ZEVENT_SCROLLWHEEL)
            {
                switch (LOWORD(event->wParam)) {
                case SB_LINEUP:
                    return ZuiControlCall(Proc_Layout_LineUp, cp, NULL, NULL, NULL);
                case SB_LINEDOWN:
                    return ZuiControlCall(Proc_Layout_LineDown, cp, NULL, NULL, NULL);
                }
            }
        }
        else if (p->m_pHorizontalScrollBar != NULL && p->m_pHorizontalScrollBar->m_bVisible && p->m_pHorizontalScrollBar->m_bEnabled) {
            if (event->Type == ZEVENT_KEYDOWN)
            {
                switch (event->chKey) {
                case VK_DOWN:
                    return ZuiControlCall(Proc_Layout_LineRight, cp, NULL, NULL, NULL);
                case VK_UP:
                    return ZuiControlCall(Proc_Layout_LineLeft, cp, NULL, NULL, NULL);
                case VK_NEXT:
                    return ZuiControlCall(Proc_Layout_PageRight, cp, NULL, NULL, NULL);
                case VK_PRIOR:
                    return ZuiControlCall(Proc_Layout_PageLeft, cp, NULL, NULL, NULL);
                case VK_HOME:
                    return ZuiControlCall(Proc_Layout_HomeLeft, cp, NULL, NULL, NULL);
                case VK_END:
                    return ZuiControlCall(Proc_Layout_EndRight, cp, NULL, NULL, NULL);
                }
            }
            else if (event->Type == ZEVENT_SCROLLWHEEL)
            {
                switch (LOWORD(event->wParam)) {
                case SB_LINEUP:
                    return ZuiControlCall(Proc_Layout_LineLeft, cp, NULL, NULL, NULL);
                case SB_LINEDOWN:
                    return ZuiControlCall(Proc_Layout_LineRight, cp, NULL, NULL, NULL);
                }
            }
        }
        break;
    }
    case Proc_SetAttribute: {
        if (wcscmp(Param1, _T("inset")) == 0) {
            ZRect rcInset = { 0 };
            ZuiText pstr = NULL;
            rcInset.left = _tcstol(Param2, &pstr, 10);  ASSERT(pstr);
            rcInset.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcInset.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcInset.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZuiControlCall(Proc_Layout_SetInset, cp, &rcInset, NULL, NULL);
        }
        else if (wcscmp(Param1, _T("vscrollbar")) == 0) {
            ZuiControlCall(Proc_Layout_EnableScrollBar, cp, (ZuiAny)(_tcscmp(Param2, _T("true")) == 0), (ZuiAny)ZuiControlCall(Proc_Layout_GetHorizontalScrollBar, cp, NULL, NULL, NULL) != NULL, NULL);
        }
        else if (wcscmp(Param1, _T("hscrollbar")) == 0) {
            ZuiControlCall(Proc_Layout_EnableScrollBar, cp, (ZuiAny)(ZuiControlCall(Proc_Layout_GetVerticalScrollBar, cp, NULL, NULL, NULL) != NULL), (ZuiAny)(_tcscmp(Param2, _T("true"))) == 0, NULL);
        }
        break;
    }
    case Proc_SetOs: {
        for (int it = 0; it < darray_len(p->m_items); it++) {
            ZuiControlCall(Proc_SetOs, (ZuiControl)(p->m_items->data[it]), Param1, cp, Param3);
        }
        if (p->m_pVerticalScrollBar != NULL) ZuiControlCall(Proc_SetOs, p->m_pVerticalScrollBar, Param1, cp, Param3);
        if (p->m_pHorizontalScrollBar != NULL) ZuiControlCall(Proc_SetOs, p->m_pHorizontalScrollBar, Param1, cp, Param3);
        break;
    }
    case Proc_SetVisible: {
        p->old_call(Proc_SetVisible, cp, 0, Param1, Param2, Param3);
        //for (int it = darray_len(p->m_items) - 1; it >= 0; it--) {
        //    ZuiControlCall(Proc_SetVisible, p->m_items->data[it], Param1, Param2, Param3);
        //}
        return 0;
        break;
    }
    case Proc_Layout_Add: {
        if (Param1 == NULL) return FALSE;

        if (cp->m_pOs != NULL)
            ZuiControlCall(Proc_SetOs, (ZuiControl)Param1, cp->m_pOs, cp, (void*)TRUE);
        if (cp->m_bVisible)
            ZuiControlNeedUpdate(cp);

        return (void *)darray_append(p->m_items, Param1);
        break;
    }
    case Proc_Layout_AddAt: {
        if (Param1 == NULL) return FALSE;

        if (cp->m_pOs != NULL)
            ZuiControlCall(Proc_SetOs, (ZuiControl)Param1, cp->m_pOs, cp->m_pParent, (void*)TRUE);
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
        return (ZuiAny)darray_len(p->m_items);
        break;
    }
    case Proc_Layout_GetItemIndex: {
        for (int it = 0; it < darray_len(p->m_items); it++) {
            if (p->m_items->data[it] == Param1) {
                return (ZuiAny)it;
            }
        }
        return (ZuiAny)-1;
        break;
    }
    case Proc_Layout_SetItemIndex: {
        for (int it = 0; it < darray_len(p->m_items); it++) {
            if (p->m_items->data[it] == Param1) {
                ZuiControlNeedUpdate(cp);
                darray_delete(p->m_items, it);
                return (ZuiAny)darray_insert(p->m_items, (int)Param2, Param1);
            }
        }

        return FALSE;
        break;
    }
    case Proc_Layout_GetItemAt: {
        if ((int)Param1 < 0 || (int)Param1 >= darray_len(p->m_items)) return NULL;
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
        ZSize *szXY = (ZSize *)ZuiControlCall(Proc_GetFixedXY, pControl, 0, 0, 0);
        ZSize sz = { (LONG)ZuiControlCall(Proc_GetFixedWidth, pControl, 0, 0, 0), (LONG)ZuiControlCall(Proc_GetFixedHeight, pControl, 0, 0, 0) };
        ZRectR rcPercent = { 0 };// pControl->GetFloatPercent();
        LONG width = cp->m_rcItem.right - cp->m_rcItem.left;
        LONG height = cp->m_rcItem.bottom - cp->m_rcItem.top;
        ZRect rcCtrl = { 0 };
        rcCtrl.left = (LONG)(width*rcPercent.left) + szXY->cx;
        rcCtrl.top = (LONG)(height*rcPercent.top) + szXY->cy;
        rcCtrl.right = (LONG)(width*rcPercent.right) + szXY->cx + sz.cx;
        rcCtrl.bottom = (LONG)(height*rcPercent.bottom) + szXY->cy + sz.cy;
        ZuiControlCall(Proc_SetPos, pControl, &rcCtrl, FALSE, 0);

        break;
    }
    case Proc_Layout_SetInset: {
        memcpy(&p->m_rcInset, Param1, sizeof(ZRect));
        ZuiControlNeedParentUpdate(cp);
        break;
    }
    case Proc_Layout_GetScrollPos: {
        ZSize sz = { 0, 0 };
        if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) sz.cy = ZuiControlCall(Proc_ScrollBar_GetScrollPos, p->m_pVerticalScrollBar, NULL, NULL, NULL);
        if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) sz.cx = ZuiControlCall(Proc_ScrollBar_GetScrollPos, p->m_pHorizontalScrollBar, NULL, NULL, NULL);
        return &sz;
        break;
    }
    case Proc_Layout_GetScrollRange: {
        ZSize sz = { 0, 0 };
        if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) sz.cy = ZuiControlCall(Proc_ScrollBar_GetScrollRange, p->m_pVerticalScrollBar, NULL, NULL, NULL);
        if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) sz.cx = ZuiControlCall(Proc_ScrollBar_GetScrollRange, p->m_pHorizontalScrollBar, NULL, NULL, NULL);
        return &sz;
        break;
    }
    case Proc_Layout_SetScrollPos: {
        ZSize *szPos = Param1;
        ZRect rcPos;
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
            ZRect *prcPos = (ZRect *)ZuiControlCall(Proc_GetPos, pControl, NULL, NULL, NULL);
            rcPos = *prcPos;
            rcPos.left -= cx;
            rcPos.right -= cx;
            rcPos.top -= cy;
            rcPos.bottom -= cy;
            ZuiControlCall(Proc_SetPos, pControl, &rcPos, NULL, NULL);
        }

        ZuiControlInvalidate(cp, TRUE);
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

        ZSize *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        ZSize sz = { psz->cx, psz->cy };
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

        ZSize *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        ZSize sz = { psz->cx, psz->cy };
        sz.cy += cyLine;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_PageUp: {
        ZSize *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        ZSize sz = { psz->cx, psz->cy };
        int iOffset = cp->m_rcItem.bottom - cp->m_rcItem.top - p->m_rcInset.top - p->m_rcInset.bottom;
        if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) iOffset -= (ZuiInt)ZuiControlCall(Proc_GetFixedHeight, p->m_pHorizontalScrollBar, NULL, NULL, NULL);
        sz.cy -= iOffset;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_PageDown: {
        ZSize *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        ZSize sz = { psz->cx, psz->cy };
        int iOffset = cp->m_rcItem.bottom - cp->m_rcItem.top - p->m_rcInset.top - p->m_rcInset.bottom;
        if (p->m_pHorizontalScrollBar && p->m_pHorizontalScrollBar->m_bVisible) iOffset -= (ZuiInt)ZuiControlCall(Proc_GetFixedHeight, p->m_pHorizontalScrollBar, NULL, NULL, NULL);
        sz.cy += iOffset;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_HomeUp: {
        ZSize *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        ZSize sz = { psz->cx, psz->cy };
        sz.cy = 0;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_EndDown: {
        ZSize *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        ZSize sz = { psz->cx, psz->cy };
        sz.cy = ((SIZE *)ZuiControlCall(Proc_Layout_GetScrollRange, cp, NULL, NULL, NULL))->cy;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_LineLeft: {
        int cxLine = p->m_nScrollStepSize == 0 ? 8 : p->m_nScrollStepSize;

        ZSize *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        ZSize sz = { psz->cx, psz->cy };
        sz.cx -= cxLine;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_LineRight: {
        int cxLine = p->m_nScrollStepSize == 0 ? 8 : p->m_nScrollStepSize;

        ZSize *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        ZSize sz = { psz->cx, psz->cy };
        sz.cx += cxLine;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_PageLeft: {
        ZSize *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        ZSize sz = { psz->cx, psz->cy };
        int iOffset = cp->m_rcItem.right - cp->m_rcItem.left - p->m_rcInset.left - p->m_rcInset.right;
        if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) iOffset -= (ZuiInt)ZuiControlCall(Proc_GetFixedWidth, p->m_pVerticalScrollBar, NULL, NULL, NULL);
        sz.cx -= iOffset;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_PageRight: {
        ZSize *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        ZSize sz = { psz->cx, psz->cy };
        int iOffset = cp->m_rcItem.right - cp->m_rcItem.left - p->m_rcInset.left - p->m_rcInset.right;
        if (p->m_pVerticalScrollBar && p->m_pVerticalScrollBar->m_bVisible) iOffset -= (ZuiInt)ZuiControlCall(Proc_GetFixedWidth, p->m_pVerticalScrollBar, NULL, NULL, NULL);
        sz.cx += iOffset;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_HomeLeft: {
        ZSize *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        ZSize sz = { psz->cx, psz->cy };
        sz.cx = 0;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_EndRight: {
        ZSize *psz = ZuiControlCall(Proc_Layout_GetScrollPos, cp, NULL, NULL, NULL);
        ZSize sz = { psz->cx, psz->cy };
        sz.cx = ((ZSize *)ZuiControlCall(Proc_Layout_GetScrollRange, cp, NULL, NULL, NULL))->cx;
        ZuiControlCall(Proc_Layout_SetScrollPos, cp, &sz, NULL, NULL);
        break;
    }
    case Proc_Layout_EnableScrollBar: {
        BOOL bEnableVertical = Param1;
        BOOL bEnableHorizontal = Param2;
        if (bEnableVertical && !p->m_pVerticalScrollBar) {
            p->m_pVerticalScrollBar = NewZuiControl(L"scrollbar", NULL, NULL, NULL);//创建滚动条
            ZuiControlCall(Proc_ScrollBar_SetOwner, p->m_pVerticalScrollBar, cp, NULL, NULL);
            p->m_pVerticalScrollBar->m_pOs = cp->m_pOs;
            p->m_pVerticalScrollBar->m_pParent = cp;
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
            p->m_pHorizontalScrollBar->m_pOs = cp->m_pOs;
            p->m_pHorizontalScrollBar->m_pParent = cp;
            //  m_pHorizontalScrollBar->SetVisible(false);

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
    }
    case Proc_Layout_GetHorizontalScrollBar: {
        return p->m_pHorizontalScrollBar;
    }
    case Proc_Layout_ProcessScrollBar: {
        ZRect *rc = Param1;
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
                ZRect rcScrollBarPos = { rc->left, rc->bottom, rc->right, rc->bottom + (ZuiInt)ZuiControlCall(Proc_GetFixedHeight, p->m_pHorizontalScrollBar, NULL, NULL, NULL) };
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
                ZRect rcScrollBarPos = { rc->right, rc->top, rc->right + (ZuiInt)ZuiControlCall(Proc_GetFixedWidth, p->m_pVerticalScrollBar, NULL, NULL, NULL), rc->bottom };
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
        break;
    }
    case Proc_OnCreate: {
        p = (ZuiLayout)malloc(sizeof(ZLayout));
        memset(p, 0, sizeof(ZLayout));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        p->old_call = cp->call;
        p->m_bMouseChildEnabled = TRUE;
        p->m_iChildPadding = 0;
        p->m_items = darray_create();
        return p;
    }
    case Proc_OnDestroy: {
        for (int it = darray_len(p->m_items) - 1; it >= 0; it--) {
            ZuiControlCall(Proc_OnDestroy, p->m_items->data[it], (ZuiAny)TRUE, Param2, Param3);
        }
        ZCtlProc old_call = p->old_call;
        if (p->m_pHorizontalScrollBar)
            FreeZuiControl(p->m_pHorizontalScrollBar, NULL);
        if (p->m_pVerticalScrollBar)
            FreeZuiControl(p->m_pVerticalScrollBar, NULL);
        darray_destroy(p->m_items);

        old_call(ProcId, cp, 0, Param1, Param2, Param3);
        free(p);
        return;
    }
    case Proc_GetObject:
        if (Param1 == Type_Layout)
            return (ZuiAny)p;
        break;
    case Proc_GetType:
        return Type_Layout;
    case Proc_CoreInit:
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit:
        return NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
}


