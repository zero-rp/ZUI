#include <ZUI.h>

ZEXPORT ZuiAny ZCALL ZuiScrollBarProc(ZuiInt ProcId, ZuiControl cp, ZuiScrollBar p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        if (!cp->m_bMouseEnabled && event->Type > ZEVENT__MOUSEBEGIN && event->Type < ZEVENT__MOUSEEND) {
            if (p->m_pOwner != NULL)
                ZuiControlCall(Proc_OnEvent, p->m_pOwner, Param1, NULL, NULL);
            else
                ZuiDefaultControlProc(Proc_OnEvent, cp, 0, Param1, NULL, NULL);
            return;
        }

        if (event->Type == ZEVENT_SETFOCUS)
        {
            return;
        }
        if (event->Type == ZEVENT_KILLFOCUS)
        {
            return;
        }
        if (event->Type == ZEVENT_LBUTTONDOWN || event->Type == ZEVENT_LDBLCLICK)
        {
            if (!cp->m_bEnabled) return;

            p->m_nLastScrollOffset = 0;
            p->m_nScrollRepeatDelay = 0;
            ZuiPaintManagerSetTimer(cp, DEFAULT_TIMERID, 50U);
            if (ZuiIsPointInRect(&p->m_rcButton1, &event->ptMouse)) {
                p->m_uButton1State |= ZSTATE_PUSHED;
                if (!p->m_bHorizontal) {
                    if (p->m_pOwner != NULL)
                        ZuiControlCall(Proc_Layout_LineUp, p->m_pOwner, NULL, NULL, NULL);
                    else
                        ZuiControlCall(Proc_ScrollBar_SetScrollPos, cp, p->m_nScrollPos - p->m_nLineSize, NULL, NULL);
                }
                else {
                    if (p->m_pOwner != NULL)
                        ZuiControlCall(Proc_Layout_LineLeft, p->m_pOwner, NULL, NULL, NULL);
                    else
                        ZuiControlCall(Proc_ScrollBar_SetScrollPos, cp, p->m_nScrollPos - p->m_nLineSize, NULL, NULL);
                }
            }
            else if (ZuiIsPointInRect(&p->m_rcButton2, &event->ptMouse)) {
                p->m_uButton2State |= ZSTATE_PUSHED;
                if (!p->m_bHorizontal) {
                    if (p->m_pOwner != NULL)
                        ZuiControlCall(Proc_Layout_LineDown, p->m_pOwner, NULL, NULL, NULL);
                    else
                        ZuiControlCall(Proc_ScrollBar_SetScrollPos, cp, p->m_nScrollPos + p->m_nLineSize, NULL, NULL);
                }
                else {
                    if (p->m_pOwner != NULL)
                        ZuiControlCall(Proc_Layout_LineRight, p->m_pOwner, NULL, NULL, NULL);
                    else
                        ZuiControlCall(Proc_ScrollBar_SetScrollPos, cp, p->m_nScrollPos + p->m_nLineSize, NULL, NULL);
                }
            }
            else if (ZuiIsPointInRect(&p->m_rcThumb, &event->ptMouse)) {
                p->m_uThumbState |= ZSTATE_CAPTURED | ZSTATE_PUSHED;
                p->ptLastMouse = event->ptMouse;
                p->m_nLastScrollPos = p->m_nScrollPos;
            }
            else {
                if (!p->m_bHorizontal) {
                    if (event->ptMouse.y < p->m_rcThumb.top) {
                        if (p->m_pOwner != NULL)
                            ZuiControlCall(Proc_Layout_PageUp, p->m_pOwner, NULL, NULL, NULL);
                        else
                            ZuiControlCall(Proc_ScrollBar_SetScrollPos, cp, p->m_nScrollPos + cp->m_rcItem.top - cp->m_rcItem.bottom, NULL, NULL);
                    }
                    else if (event->ptMouse.y > p->m_rcThumb.bottom) {
                        if (p->m_pOwner != NULL)
                            ZuiControlCall(Proc_Layout_PageDown, p->m_pOwner, NULL, NULL, NULL);
                        else
                            ZuiControlCall(Proc_ScrollBar_SetScrollPos, cp, p->m_nScrollPos - cp->m_rcItem.top + cp->m_rcItem.bottom, NULL, NULL);
                    }
                }
                else {
                    if (event->ptMouse.x < p->m_rcThumb.left) {
                        if (p->m_pOwner != NULL)
                            ZuiControlCall(Proc_Layout_PageLeft, p->m_pOwner, NULL, NULL, NULL);
                        else
                            ZuiControlCall(Proc_ScrollBar_SetScrollPos, cp, p->m_nScrollPos + cp->m_rcItem.left - cp->m_rcItem.right, NULL, NULL);
                    }
                    else if (event->ptMouse.x > p->m_rcThumb.right) {
                        if (p->m_pOwner != NULL)
                            ZuiControlCall(Proc_Layout_PageRight, p->m_pOwner, NULL, NULL, NULL);
                        else
                            ZuiControlCall(Proc_ScrollBar_SetScrollPos, cp, p->m_nScrollPos - cp->m_rcItem.left + cp->m_rcItem.right, NULL, NULL);
                    }
                }
            }
            //if (m_pManager != NULL && p->m_pOwner == NULL) m_pManager->SendNotify(this, DUI_MSGTYPE_SCROLL);
            return;
        }
        if (event->Type == ZEVENT_LBUTTONUP)
        {
            p->m_nScrollRepeatDelay = 0;
            p->m_nLastScrollOffset = 0;
            ZuiPaintManagerKillTimer_Id(cp, DEFAULT_TIMERID);

            if ((p->m_uThumbState & ZSTATE_CAPTURED) != 0) {
                p->m_uThumbState &= ~(ZSTATE_CAPTURED | ZSTATE_PUSHED);
                ZuiControlInvalidate(cp, TRUE);
            }
            else if ((p->m_uButton1State & ZSTATE_PUSHED) != 0) {
                p->m_uButton1State &= ~ZSTATE_PUSHED;
                ZuiControlInvalidate(cp, TRUE);
            }
            else if ((p->m_uButton2State & ZSTATE_PUSHED) != 0) {
                p->m_uButton2State &= ~ZSTATE_PUSHED;
                ZuiControlInvalidate(cp, TRUE);
            }
            return;
        }
        if (event->Type == ZEVENT_MOUSEMOVE)
        {
            if ((p->m_uThumbState & ZSTATE_CAPTURED) != 0) {
                if (!p->m_bHorizontal) {

                    int vRange = cp->m_rcItem.bottom - cp->m_rcItem.top - p->m_rcThumb.bottom + p->m_rcThumb.top - 2 * cp->m_cxyFixed.cx;

                    if (vRange != 0)
                        p->m_nLastScrollOffset = (event->ptMouse.y - p->ptLastMouse.y) * p->m_nRange / vRange;

                }
                else {

                    int hRange = cp->m_rcItem.right - cp->m_rcItem.left - p->m_rcThumb.right + p->m_rcThumb.left - 2 * cp->m_cxyFixed.cy;

                    if (hRange != 0)
                        p->m_nLastScrollOffset = (event->ptMouse.x - p->ptLastMouse.x) * p->m_nRange / hRange;
                }
            }
            else {
                if ((p->m_uThumbState & ZSTATE_HOT) != 0) {
                    if (!ZuiIsPointInRect(&p->m_rcThumb, &event->ptMouse)) {
                        p->m_uThumbState &= ~ZSTATE_HOT;
                        ZuiControlInvalidate(cp, TRUE);
                    }
                }
                else {
                    if (!cp->m_bEnabled) return;
                    if (ZuiIsPointInRect(&p->m_rcThumb, &event->ptMouse)) {
                        p->m_uThumbState |= ZSTATE_HOT;
                        ZuiControlInvalidate(cp, TRUE);
                    }
                }
            }
            return;
        }
        if (event->Type == ZEVENT_CONTEXTMENU)
        {
            return;
        }
        if (event->Type == ZEVENT_TIMER && event->wParam == DEFAULT_TIMERID)
        {
            ++p->m_nScrollRepeatDelay;
            if ((p->m_uThumbState & ZSTATE_CAPTURED) != 0) {
                if (!p->m_bHorizontal) {
                    if (p->m_pOwner != NULL) {
                        SIZE sz = { ((SIZE *)ZuiControlCall(Proc_Layout_GetScrollPos, p->m_pOwner, NULL, NULL, NULL))->cx, p->m_nLastScrollPos + p->m_nLastScrollOffset };
                        ZuiControlCall(Proc_Layout_SetScrollPos, p->m_pOwner, &sz, NULL, NULL);
                    }
                    else
                        ZuiControlCall(Proc_ScrollBar_SetScrollPos, cp, p->m_nLastScrollPos + p->m_nLastScrollOffset, NULL, NULL);
                }
                else {
                    if (p->m_pOwner != NULL) {
                        SIZE sz = { p->m_nLastScrollPos + p->m_nLastScrollOffset, ((SIZE *)ZuiControlCall(Proc_Layout_GetScrollPos, p->m_pOwner, NULL, NULL, NULL))->cy };
                        ZuiControlCall(Proc_Layout_SetScrollPos, p->m_pOwner, &sz, NULL, NULL);
                    }
                    else
                        ZuiControlCall(Proc_ScrollBar_SetScrollPos, cp, p->m_nLastScrollPos + p->m_nLastScrollOffset, NULL, NULL);
                }
                ZuiControlInvalidate(cp, TRUE);
            }
            else if ((p->m_uButton1State & ZSTATE_PUSHED) != 0) {
                if (p->m_nScrollRepeatDelay <= 5) return;
                if (!p->m_bHorizontal) {
                    if (p->m_pOwner != NULL)
                        ZuiControlCall(Proc_Layout_LineUp, p->m_pOwner, NULL, NULL, NULL);
                    else
                        ZuiControlCall(Proc_ScrollBar_SetScrollPos, cp, p->m_nScrollPos - p->m_nLineSize, NULL, NULL);
                }
                else {
                    if (p->m_pOwner != NULL)
                        ZuiControlCall(Proc_Layout_LineLeft, p->m_pOwner, NULL, NULL, NULL);
                    else
                        ZuiControlCall(Proc_ScrollBar_SetScrollPos, cp, p->m_nScrollPos - p->m_nLineSize, NULL, NULL);
                }
            }
            else if ((p->m_uButton2State & ZSTATE_PUSHED) != 0) {
                if (p->m_nScrollRepeatDelay <= 5) return;
                if (!p->m_bHorizontal) {
                    if (p->m_pOwner != NULL)
                        ZuiControlCall(Proc_Layout_LineDown, p->m_pOwner, NULL, NULL, NULL);
                    else
                        ZuiControlCall(Proc_ScrollBar_SetScrollPos, cp, p->m_nScrollPos + p->m_nLineSize, NULL, NULL);
                }
                else {
                    if (p->m_pOwner != NULL)
                        ZuiControlCall(Proc_Layout_LineRight, p->m_pOwner, NULL, NULL, NULL);
                    else
                        ZuiControlCall(Proc_ScrollBar_SetScrollPos, cp, p->m_nScrollPos + p->m_nLineSize, NULL, NULL);
                }
            }
            else {
                ZPoint pt = { 0 };
                if (p->m_nScrollRepeatDelay <= 5) return;
                GetCursorPos(&pt);
                ZuiScreenToClient(cp, &pt);
                if (!p->m_bHorizontal) {
                    if (pt.y < p->m_rcThumb.top) {
                        if (p->m_pOwner != NULL)
                            ZuiControlCall(Proc_Layout_PageUp, p->m_pOwner, NULL, NULL, NULL);
                        else
                            ZuiControlCall(Proc_ScrollBar_SetScrollPos, cp, p->m_nScrollPos + cp->m_rcItem.top - cp->m_rcItem.bottom, NULL, NULL);
                    }
                    else if (pt.y > p->m_rcThumb.bottom) {
                        if (p->m_pOwner != NULL)
                            ZuiControlCall(Proc_Layout_PageDown, p->m_pOwner, NULL, NULL, NULL);
                        else
                            ZuiControlCall(Proc_ScrollBar_SetScrollPos, cp, p->m_nScrollPos - cp->m_rcItem.top + cp->m_rcItem.bottom, NULL, NULL);
                    }
                }
                else {
                    if (pt.x < p->m_rcThumb.left) {
                        if (p->m_pOwner != NULL)
                            ZuiControlCall(Proc_Layout_PageLeft, p->m_pOwner, NULL, NULL, NULL);
                        else
                            ZuiControlCall(Proc_ScrollBar_SetScrollPos, cp, p->m_nScrollPos + cp->m_rcItem.left - cp->m_rcItem.right, NULL, NULL);
                    }
                    else if (pt.x > p->m_rcThumb.right) {
                        if (p->m_pOwner != NULL)
                            ZuiControlCall(Proc_Layout_PageRight, p->m_pOwner, NULL, NULL, NULL);
                        else
                            ZuiControlCall(Proc_ScrollBar_SetScrollPos, cp, p->m_nScrollPos - cp->m_rcItem.left + cp->m_rcItem.right, NULL, NULL);
                    }
                }
            }
            //if (m_pManager != NULL && p->m_pOwner == NULL) m_pManager->SendNotify(this, DUI_MSGTYPE_SCROLL);
            return;
        }
        if (event->Type == ZEVENT_MOUSEENTER)
        {
            if (cp->m_bEnabled) {
                p->m_uButton1State |= ZSTATE_HOT;
                p->m_uButton2State |= ZSTATE_HOT;
                if (ZuiIsPointInRect(&p->m_rcThumb, &event->ptMouse)) p->m_uThumbState |= ZSTATE_HOT;
                ZuiControlInvalidate(cp, TRUE);
            }
            return;
        }
        if (event->Type == ZEVENT_MOUSELEAVE)
        {
            if (cp->m_bEnabled) {
                p->m_uButton1State &= ~ZSTATE_HOT;
                p->m_uButton2State &= ~ZSTATE_HOT;
                p->m_uThumbState &= ~ZSTATE_HOT;
                ZuiControlInvalidate(cp, TRUE);
            }
            return;
        }

        if (p->m_pOwner != NULL)
            ZuiControlCall(Proc_OnEvent, p->m_pOwner, Param1, NULL, NULL);
        else
            ZuiDefaultControlProc(Proc_OnEvent, cp, 0, Param1, NULL, NULL);
        break;
    }
    case Proc_OnPaint: {
        //调整绘制顺序
        ZuiControlCall(Proc_OnPaintBkColor, cp, Param1, Param2, NULL);
        ZuiControlCall(Proc_OnPaintStatusImage, cp, Param1, Param2, NULL);
        ZuiControlCall(Proc_OnPaintBkImage, cp, Param1, Param2, NULL);
        ZuiControlCall(Proc_OnPaintText, cp, Param1, Param2, NULL);
        ZuiControlCall(Proc_OnPaintBorder, cp, Param1, Param2, NULL);
        return 0;
    }
    case Proc_OnPaintBkColor: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        RECT *rc = (RECT *)&p->m_rcThumb;
        ZuiDrawFillRect(gp, ARGB(200, 0, 3, 255), rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
            
    }
    case Proc_SetPos: {
        ZuiDefaultControlProc(ProcId, cp, 0, Param1, Param2, Param3);
        RECT rc = cp->m_rcItem;

        if (p->m_bHorizontal) {
            int cx = rc.right - rc.left;
            if (p->m_bShowButton1) cx -= cp->m_cxyFixed.cy;
            if (p->m_bShowButton2) cx -= cp->m_cxyFixed.cy;
            if (cx > cp->m_cxyFixed.cy) {
                p->m_rcButton1.left = rc.left;
                p->m_rcButton1.top = rc.top;
                if (p->m_bShowButton1) {
                    p->m_rcButton1.right = rc.left + cp->m_cxyFixed.cy;
                    p->m_rcButton1.bottom = rc.top + cp->m_cxyFixed.cy;
                }
                else {
                    p->m_rcButton1.right = p->m_rcButton1.left;
                    p->m_rcButton1.bottom = p->m_rcButton1.top;
                }

                p->m_rcButton2.top = rc.top;
                p->m_rcButton2.right = rc.right;
                if (p->m_bShowButton2) {
                    p->m_rcButton2.left = rc.right - cp->m_cxyFixed.cy;
                    p->m_rcButton2.bottom = rc.top + cp->m_cxyFixed.cy;
                }
                else {
                    p->m_rcButton2.left = p->m_rcButton2.right;
                    p->m_rcButton2.bottom = p->m_rcButton2.top;
                }

                p->m_rcThumb.top = rc.top;
                p->m_rcThumb.bottom = rc.top + cp->m_cxyFixed.cy;
                if (p->m_nRange > 0) {
                    int cxThumb = cx * (rc.right - rc.left) / (p->m_nRange + rc.right - rc.left);
                    if (cxThumb < cp->m_cxyFixed.cy) cxThumb = cp->m_cxyFixed.cy;

                    p->m_rcThumb.left = p->m_nScrollPos * (cx - cxThumb) / p->m_nRange + p->m_rcButton1.right;
                    p->m_rcThumb.right = p->m_rcThumb.left + cxThumb;
                    if (p->m_rcThumb.right > p->m_rcButton2.left) {
                        p->m_rcThumb.left = p->m_rcButton2.left - cxThumb;
                        p->m_rcThumb.right = p->m_rcButton2.left;
                    }
                }
                else {
                    p->m_rcThumb.left = p->m_rcButton1.right;
                    p->m_rcThumb.right = p->m_rcButton2.left;
                }
            }
            else {
                int cxButton = (rc.right - rc.left) / 2;
                if (cxButton > cp->m_cxyFixed.cy) cxButton = cp->m_cxyFixed.cy;
                p->m_rcButton1.left = rc.left;
                p->m_rcButton1.top = rc.top;
                if (p->m_bShowButton1) {
                    p->m_rcButton1.right = rc.left + cxButton;
                    p->m_rcButton1.bottom = rc.top + cp->m_cxyFixed.cy;
                }
                else {
                    p->m_rcButton1.right = p->m_rcButton1.left;
                    p->m_rcButton1.bottom = p->m_rcButton1.top;
                }

                p->m_rcButton2.top = rc.top;
                p->m_rcButton2.right = rc.right;
                if (p->m_bShowButton2) {
                    p->m_rcButton2.left = rc.right - cxButton;
                    p->m_rcButton2.bottom = rc.top + cp->m_cxyFixed.cy;
                }
                else {
                    p->m_rcButton2.left = p->m_rcButton2.right;
                    p->m_rcButton2.bottom = p->m_rcButton2.top;
                }

                memset(&p->m_rcThumb, 0, sizeof(p->m_rcThumb));
            }
        }
        else {
            int cy = rc.bottom - rc.top;
            if (p->m_bShowButton1) cy -= cp->m_cxyFixed.cx;
            if (p->m_bShowButton2) cy -= cp->m_cxyFixed.cx;
            if (cy > cp->m_cxyFixed.cx) {
                p->m_rcButton1.left = rc.left;
                p->m_rcButton1.top = rc.top;
                if (p->m_bShowButton1) {
                    p->m_rcButton1.right = rc.left + cp->m_cxyFixed.cx;
                    p->m_rcButton1.bottom = rc.top + cp->m_cxyFixed.cx;
                }
                else {
                    p->m_rcButton1.right = p->m_rcButton1.left;
                    p->m_rcButton1.bottom = p->m_rcButton1.top;
                }

                p->m_rcButton2.left = rc.left;
                p->m_rcButton2.bottom = rc.bottom;
                if (p->m_bShowButton2) {
                    p->m_rcButton2.top = rc.bottom - cp->m_cxyFixed.cx;
                    p->m_rcButton2.right = rc.left + cp->m_cxyFixed.cx;
                }
                else {
                    p->m_rcButton2.top = p->m_rcButton2.bottom;
                    p->m_rcButton2.right = p->m_rcButton2.left;
                }

                p->m_rcThumb.left = rc.left;
                p->m_rcThumb.right = rc.left + cp->m_cxyFixed.cx;
                if (p->m_nRange > 0) {
                    int cyThumb = cy * (rc.bottom - rc.top) / (p->m_nRange + rc.bottom - rc.top);
                    if (cyThumb < cp->m_cxyFixed.cx) cyThumb = cp->m_cxyFixed.cx;

                    p->m_rcThumb.top = p->m_nScrollPos * (cy - cyThumb) / p->m_nRange + p->m_rcButton1.bottom;
                    p->m_rcThumb.bottom = p->m_rcThumb.top + cyThumb;
                    if (p->m_rcThumb.bottom > p->m_rcButton2.top) {
                        p->m_rcThumb.top = p->m_rcButton2.top - cyThumb;
                        p->m_rcThumb.bottom = p->m_rcButton2.top;
                    }
                }
                else {
                    p->m_rcThumb.top = p->m_rcButton1.bottom;
                    p->m_rcThumb.bottom = p->m_rcButton2.top;
                }
            }
            else {
                int cyButton = (rc.bottom - rc.top) / 2;
                if (cyButton > cp->m_cxyFixed.cx) cyButton = cp->m_cxyFixed.cx;
                p->m_rcButton1.left = rc.left;
                p->m_rcButton1.top = rc.top;
                if (p->m_bShowButton1) {
                    p->m_rcButton1.right = rc.left + cp->m_cxyFixed.cx;
                    p->m_rcButton1.bottom = rc.top + cyButton;
                }
                else {
                    p->m_rcButton1.right = p->m_rcButton1.left;
                    p->m_rcButton1.bottom = p->m_rcButton1.top;
                }

                p->m_rcButton2.left = rc.left;
                p->m_rcButton2.bottom = rc.bottom;
                if (p->m_bShowButton2) {
                    p->m_rcButton2.top = rc.bottom - cyButton;
                    p->m_rcButton2.right = rc.left + cp->m_cxyFixed.cx;
                }
                else {
                    p->m_rcButton2.top = p->m_rcButton2.bottom;
                    p->m_rcButton2.right = p->m_rcButton2.left;
                }

                memset(&p->m_rcThumb, 0, sizeof(p->m_rcThumb));
            }
        }
        break;
    }
    case Proc_ScrollBar_SetHorizontal: {
        if (p->m_bHorizontal == Param1) return;

        p->m_bHorizontal = Param1;
        if (p->m_bHorizontal) {
            if (cp->m_cxyFixed.cy == 0) {
                cp->m_cxyFixed.cx = 0;
                cp->m_cxyFixed.cy = DEFAULT_SCROLLBAR_SIZE;
            }
        }
        else {
            if (cp->m_cxyFixed.cx == 0) {
                cp->m_cxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
                cp->m_cxyFixed.cy = 0;
            }
        }

        if (p->m_pOwner != NULL)
            ZuiControlNeedUpdate(p->m_pOwner);
        else
            ZuiControlNeedParentUpdate(cp);
        break;
    }
    case Proc_ScrollBar_SetScrollPos: {
        if (p->m_nScrollPos == Param1) return;

        p->m_nScrollPos = Param1;
        if (p->m_nScrollPos < 0) p->m_nScrollPos = 0;
        if (p->m_nScrollPos > p->m_nRange) p->m_nScrollPos = p->m_nRange;
        ZuiControlCall(Proc_SetPos, cp, &cp->m_rcItem, NULL, NULL);
        break;
    }
    case Proc_ScrollBar_GetScrollPos: {
        return p->m_nScrollPos;
    }
    case Proc_ScrollBar_GetScrollRange: {
        return p->m_nRange;
    }
    case Proc_ScrollBar_SetOwner: {
        p->m_pOwner = Param1;
        break;
    }
    case Proc_ScrollBar_SetScrollRange: {
        if (p->m_nRange == Param1) return;

        p->m_nRange = Param1;
        if (p->m_nRange < 0) p->m_nRange = 0;
        if (p->m_nScrollPos > p->m_nRange) p->m_nScrollPos = p->m_nRange;
        ZuiControlCall(Proc_SetPos, cp, &cp->m_rcItem, NULL, NULL);
        break;
    }
    case Proc_OnCreate: {
        p = (ZuiScrollBar)ZuiMalloc(sizeof(ZScrollBar));
        memset(p, 0, sizeof(ZScrollBar));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        p->old_call = cp->call;

        p->m_nRange = 100;
        p->m_nLineSize = 8;
        p->m_bShowButton1 = TRUE;
        p->m_bShowButton2 = TRUE;
        cp->m_cxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;

        ZuiFree(p);

        return old_call(ProcId, cp, 0, Param1, Param2, Param3);
    }
    case Proc_GetObject:
        if (Param1 == Type_ScrollBar)
            return (ZuiAny)p;
        break;
    case Proc_GetType:
        return (ZuiAny)Type_ScrollBar;
    case Proc_CoreInit:
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
}
