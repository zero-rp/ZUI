#include <ZUI.h>
//默认数据
//横向
ZuiRes m_sButton1NormalImage;
ZuiRes m_sButton1HotImage;
ZuiRes m_sButton1PushedImage;
ZuiRes m_sButton1DisabledImage;

ZuiRes m_sButton2NormalImage;
ZuiRes m_sButton2HotImage;
ZuiRes m_sButton2PushedImage;
ZuiRes m_sButton2DisabledImage;

ZuiRes m_sThumbNormalImage;
ZuiRes m_sThumbHotImage;
ZuiRes m_sThumbPushedImage;
ZuiRes m_sThumbDisabledImage;


//纵向
ZuiRes _m_sButton1NormalImage;
ZuiRes _m_sButton1HotImage;
ZuiRes _m_sButton1PushedImage;
ZuiRes _m_sButton1DisabledImage;

ZuiRes _m_sButton2NormalImage;
ZuiRes _m_sButton2HotImage;
ZuiRes _m_sButton2PushedImage;
ZuiRes _m_sButton2DisabledImage;

ZuiRes _m_sThumbNormalImage;
ZuiRes _m_sThumbHotImage;
ZuiRes _m_sThumbPushedImage;
ZuiRes _m_sThumbDisabledImage;
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
                if (p->m_bShowButton1 && ZuiIsPointInRect(&p->m_rcButton1, &event->ptMouse)) {
                    if ((p->m_uButton1State & ZSTATE_HOT) == 0) {
                        p->m_uButton1State |= ZSTATE_HOT;
                        p->m_uButton2State &= ~ZSTATE_HOT;
                        p->m_uThumbState &= ~ZSTATE_HOT;
                        ZuiControlInvalidate(cp, TRUE);
                    }
                }
                else if (p->m_bShowButton2 && ZuiIsPointInRect(&p->m_rcButton2, &event->ptMouse)) {
                    if ((p->m_uButton2State & ZSTATE_HOT) == 0) {
                        p->m_uButton2State |= ZSTATE_HOT;
                        p->m_uThumbState &= ~ZSTATE_HOT;
                        p->m_uButton1State &= ~ZSTATE_HOT;
                        ZuiControlInvalidate(cp, TRUE);
                    }
                }
                else if (ZuiIsPointInRect(&p->m_rcThumb, &event->ptMouse)) {
                    if ((p->m_uThumbState & ZSTATE_HOT) == 0) {
                        p->m_uThumbState |= ZSTATE_HOT;
                        p->m_uButton1State &= ~ZSTATE_HOT;
                        p->m_uButton2State &= ~ZSTATE_HOT;
                        ZuiControlInvalidate(cp, TRUE);
                    }
                }
                else if((p->m_uButton1State & ZSTATE_HOT) == 0 || (p->m_uButton2State & ZSTATE_HOT) == 0 || (p->m_uThumbState & ZSTATE_HOT) == 0)
                {
                    p->m_uThumbState &= ~ZSTATE_HOT;
                    p->m_uButton1State &= ~ZSTATE_HOT;
                    p->m_uButton2State &= ~ZSTATE_HOT;
                    ZuiControlInvalidate(cp, TRUE);
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
                if (p->m_bShowButton1 && ZuiIsPointInRect(&p->m_rcButton1, &event->ptMouse)) {
                    p->m_uButton1State |= ZSTATE_HOT;
                }
                else if (p->m_bShowButton2 && ZuiIsPointInRect(&p->m_rcButton2, &event->ptMouse)) {
                    p->m_uButton2State |= ZSTATE_HOT;
                }
                else if (ZuiIsPointInRect(&p->m_rcThumb, &event->ptMouse)) {
                    p->m_uThumbState |= ZSTATE_HOT;
                }
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
    case Proc_OnPaintStatusImage: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        RECT *rc = (RECT *)Param2;
        ZuiImage img;
        //绘制第一个按钮
        if (p->m_bShowButton1) {

            if (!cp->m_bEnabled) p->m_uButton1State |= ZSTATE_DISABLED;
            else p->m_uButton1State &= ~ZSTATE_DISABLED;

            //int d1 = MulDiv(p->m_rcButton1.left - cp->m_rcItem.left, 100, Global_DPI_X);
            //int d2 = MulDiv(p->m_rcButton1.top - cp->m_rcItem.top, 100, Global_DPI_Y);
            //int d3 = MulDiv(p->m_rcButton1.right - cp->m_rcItem.left, 100, Global_DPI_X);
            //int d4 = MulDiv(p->m_rcButton1.bottom - cp->m_rcItem.top, 100, Global_DPI_Y);
            rc = &p->m_rcButton1;
            if ((p->m_uButton1State & ZSTATE_DISABLED) != 0) {
                if (p->m_sButton1DisabledImage)
                    img = p->m_sButton1DisabledImage->p;
                else
                    if (!p->m_bHorizontal)
                        img = _m_sButton1DisabledImage->p;
                    else
                        img = m_sButton1DisabledImage->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            else if ((p->m_uButton1State & ZSTATE_PUSHED) != 0) {
                if (p->m_sButton1PushedImage)
                    img = p->m_sButton1PushedImage->p;
                else
                    if (!p->m_bHorizontal)
                        img = _m_sButton1PushedImage->p;
                    else
                        img = m_sButton1PushedImage->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            else if ((p->m_uButton1State & ZSTATE_HOT) != 0) {
                if (p->m_sButton1HotImage)
                    img = p->m_sButton1HotImage->p;
                else
                    if (!p->m_bHorizontal)
                        img = _m_sButton1HotImage->p;
                    else
                        img = m_sButton1HotImage->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            else {
                if (p->m_sButton1NormalImage)
                    img = p->m_sButton1NormalImage->p;
                else
                    if (!p->m_bHorizontal)
                        img = _m_sButton1NormalImage->p;
                    else
                        img = m_sButton1NormalImage->p;

                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            //绘制按钮1边框
            //DWORD dwBorderColor = 0xFF85E4FF;
            //int nBorderSize = 2;
            //CRenderEngine::DrawRect(hDC, m_rcButton1, nBorderSize, dwBorderColor);
        }
        //绘制第二个按钮
        if (p->m_bShowButton2) {

            if (!cp->m_bEnabled) p->m_uButton2State |= ZSTATE_DISABLED;
            else p->m_uButton2State &= ~ZSTATE_DISABLED;
            rc = &p->m_rcButton2;
            if ((p->m_uButton2State & ZSTATE_DISABLED) != 0) {
                if (p->m_sButton2DisabledImage)
                    img = p->m_sButton2DisabledImage->p;
                else
                    if (!p->m_bHorizontal)
                        img = _m_sButton2DisabledImage->p;
                    else
                        img = m_sButton2DisabledImage->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            else if ((p->m_uButton2State & ZSTATE_PUSHED) != 0) {
                if (p->m_sButton2PushedImage)
                    img = p->m_sButton2PushedImage->p;
                else
                    if (!p->m_bHorizontal)
                        img = _m_sButton2PushedImage->p;
                    else
                        img = m_sButton2PushedImage->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            else if ((p->m_uButton2State & ZSTATE_HOT) != 0) {
                if (p->m_sButton2HotImage)
                    img = p->m_sButton2HotImage->p;
                else
                    if (!p->m_bHorizontal)
                        img = _m_sButton2HotImage->p;
                    else
                        img = m_sButton2HotImage->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            else {
                if (p->m_sButton2NormalImage)
                    img = p->m_sButton2NormalImage->p;
                else
                    if (!p->m_bHorizontal)
                        img = _m_sButton2NormalImage->p;
                    else
                        img = m_sButton2NormalImage->p;

                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            //绘制按钮1边框
            //DWORD dwBorderColor = 0xFF85E4FF;
            //int nBorderSize = 2;
            //CRenderEngine::DrawRect(hDC, m_rcButton1, nBorderSize, dwBorderColor);
        }

        if (p->m_rcThumb.left == 0 && p->m_rcThumb.top == 0 && p->m_rcThumb.right == 0 && p->m_rcThumb.bottom == 0) return;
        if (!cp->m_bEnabled) p->m_uThumbState |= ZSTATE_DISABLED;
        else p->m_uThumbState &= ~ZSTATE_DISABLED;

        rc = &p->m_rcThumb;
        if ((p->m_uThumbState & ZSTATE_DISABLED) != 0) {
            if (p->m_sThumbDisabledImage)
                img = p->m_sThumbDisabledImage->p;
            else
                if (!p->m_bHorizontal)
                    img = _m_sThumbDisabledImage->p;
                else
                    img = m_sThumbDisabledImage->p;
            ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
        }
        else if ((p->m_uThumbState & ZSTATE_PUSHED) != 0) {
            if (p->m_sThumbPushedImage)
                img = p->m_sThumbPushedImage->p;
            else
                if (!p->m_bHorizontal)
                    img = _m_sThumbPushedImage->p;
                else
                    img = m_sThumbPushedImage->p;
            ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
        }
        else if ((p->m_uThumbState & ZSTATE_HOT) != 0) {
            if (p->m_sThumbHotImage)
                img = p->m_sThumbHotImage->p;
            else
                if (!p->m_bHorizontal)
                    img = _m_sThumbHotImage->p;
                else
                    img = m_sThumbHotImage->p;
            ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
        }
        else {
            if (p->m_sThumbNormalImage)
                img = p->m_sThumbNormalImage->p;
            else
                if (!p->m_bHorizontal)
                    img = _m_sThumbNormalImage->p;
                else
                    img = m_sThumbNormalImage->p;

            ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
        }

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
    case Proc_JsSet:
    case Proc_JsGet:
    case Proc_JsCall: {
        break;
    }
    case Proc_JsInit: {
        //ZuiBuilderControlInit(Param1, "top", Js_Id_top, TRUE);
        //ZuiBuilderControlInit(Param1, "top", Js_Id_top, TRUE);
        //ZuiBuilderControlInit(Param1, "top", Js_Id_top, TRUE);
        //ZuiBuilderControlInit(Param1, "top", Js_Id_top, TRUE);
        //ZuiBuilderControlInit(Param1, "top", Js_Id_top, TRUE);
        //ZuiBuilderControlInit(Param1, "top", Js_Id_top, TRUE);
        //ZuiBuilderControlInit(Param1, "top", Js_Id_top, TRUE);
        break;
    }
    case Proc_SetAttribute: {
        //if (_tcsicmp(pstrName, _T("button1normalimage")) == 0) SetButton1NormalImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("button1hotimage")) == 0) SetButton1HotImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("button1pushedimage")) == 0) SetButton1PushedImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("button1disabledimage")) == 0) SetButton1DisabledImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("button2normalimage")) == 0) SetButton2NormalImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("button2hotimage")) == 0) SetButton2HotImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("button2pushedimage")) == 0) SetButton2PushedImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("button2disabledimage")) == 0) SetButton2DisabledImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("thumbnormalimage")) == 0) SetThumbNormalImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("thumbhotimage")) == 0) SetThumbHotImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("thumbpushedimage")) == 0) SetThumbPushedImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("thumbdisabledimage")) == 0) SetThumbDisabledImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("railnormalimage")) == 0) SetRailNormalImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("railhotimage")) == 0) SetRailHotImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("railpushedimage")) == 0) SetRailPushedImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("raildisabledimage")) == 0) SetRailDisabledImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("bknormalimage")) == 0) SetBkNormalImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("bkhotimage")) == 0) SetBkHotImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("bkpushedimage")) == 0) SetBkPushedImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("bkdisabledimage")) == 0) SetBkDisabledImage(pstrValue);
        //else if (_tcsicmp(pstrName, _T("hor")) == 0) SetHorizontal(_tcsicmp(pstrValue, _T("true")) == 0);
        //else if (_tcsicmp(pstrName, _T("linesize")) == 0) SetLineSize(_ttoi(pstrValue));
        //else if (_tcsicmp(pstrName, _T("range")) == 0) SetScrollRange(_ttoi(pstrValue));
        //else if (_tcsicmp(pstrName, _T("value")) == 0) SetScrollPos(_ttoi(pstrValue));
        //else if (_tcsicmp(pstrName, _T("showbutton1")) == 0) SetShowButton1(_tcsicmp(pstrValue, _T("true")) == 0);
        //else if (_tcsicmp(pstrName, _T("showbutton2")) == 0) SetShowButton2(_tcsicmp(pstrValue, _T("true")) == 0);
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
    case Proc_CoreInit: {
        m_sButton1NormalImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='0,0,16,16'", ZREST_IMG);
        m_sButton1HotImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='18,0,16,16'", ZREST_IMG);
        m_sButton1PushedImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='36,0,16,16'", ZREST_IMG);
        m_sButton1DisabledImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='54,0,16,16'", ZREST_IMG);

        _m_sButton1NormalImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='0,90,16,16'", ZREST_IMG);
        _m_sButton1HotImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='18,90,16,16'", ZREST_IMG);
        _m_sButton1PushedImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='36,90,16,16'", ZREST_IMG);
        _m_sButton1DisabledImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='54,90,16,16'", ZREST_IMG);

        m_sButton2NormalImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='0,18,16,16'", ZREST_IMG);
        m_sButton2HotImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='18,18,16,16'", ZREST_IMG);
        m_sButton2PushedImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='36,18,16,16'", ZREST_IMG);
        m_sButton2DisabledImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='54,18,16,16'", ZREST_IMG);

        _m_sButton2NormalImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='0,108,16,16'", ZREST_IMG);
        _m_sButton2HotImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='18,108,16,16'", ZREST_IMG);
        _m_sButton2PushedImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='36,108,16,16'", ZREST_IMG);
        _m_sButton2DisabledImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='54,108,16,16'", ZREST_IMG);

        m_sThumbNormalImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='0,54,16,16'", ZREST_IMG);
        m_sThumbHotImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='18,54,16,16'", ZREST_IMG);
        m_sThumbPushedImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='36,54,16,16'", ZREST_IMG);
        m_sThumbDisabledImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='54,54,16,16'", ZREST_IMG);

        _m_sThumbNormalImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='0,144,16,16'", ZREST_IMG);
        _m_sThumbHotImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='18,144,16,16'", ZREST_IMG);
        _m_sThumbPushedImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='36,144,16,16'", ZREST_IMG);
        _m_sThumbDisabledImage = ZuiResDBGetRes(L"default:default/scrollbar.bmp:src='54,144,16,16'", ZREST_IMG);
        return (ZuiAny)TRUE;
    }
    case Proc_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
}
