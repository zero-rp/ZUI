#include <ZUI.h>

ZEXPORT ZuiAny ZCALL ZuiEditProc(ZuiInt ProcId, ZuiControl cp, ZuiEdit p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        switch (event->Type)
        {
        case ZEVENT_TIMER: {
            p->type = !p->type;
            ZuiControlInvalidate(cp, TRUE);
            break;
        }
        case ZEVENT_MOUSELEAVE: {
            p->MouseType = 0;
            ZuiControlInvalidate(cp, TRUE);
            break;
        }
        case ZEVENT_MOUSEENTER: {
            p->MouseType = 1;
            ZuiControlInvalidate(cp, TRUE);
            return;
            break;
        }
        case ZEVENT_SETCURSOR: {
            SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
            return;
            break;
        }
        case ZEVENT_LBUTTONDOWN: {
            ZuiControlInvalidate(cp, TRUE);
            break;
        }
        case ZEVENT_LBUTTONUP: {

            ZuiControlInvalidate(cp, TRUE);
            break;
        }
        case ZEVENT_KILLFOCUS: {
            ZuiPaintManagerKillTimer_Id(cp, 1000);
            ZuiPaintManagerSetImeStatus(cp->m_pManager, FALSE);
            p->type = FALSE;
            ZuiControlInvalidate(cp, TRUE);
            break;
        }
        case ZEVENT_SETFOCUS: {
            ZuiPaintManagerSetTimer(cp, 1000, 600);
            ZuiPaintManagerSetImeStatus(cp->m_pManager, TRUE);
            ZuiControlInvalidate(cp, TRUE);
            break;
        }
        case ZEVENT_CHAR: {
            //过滤回车
            if (event->wParam == L'\r') {
                break;
            }

            if (event->wParam == L'\b' && p->pos == 0) {
                break;
            }
            if (event->wParam == L'\b') {
                p->pos--;
                p->StrLen--;
                cp->m_sText[p->pos] = L'\0';
            }
            else
            {
                cp->m_sText[p->pos] = (_ZuiText)event->wParam;
                p->pos++;
                p->StrLen++;
                if (p->StrLen == p->pos)
                    cp->m_sText[p->pos] = L'\0';
            }
            ZRectR r = { 0 };
            ZuiMeasureStringRect(cp->m_pManager->m_hDcOffscreen, p->sf, cp->m_sText, &r, 0);
            p->x = r.Width;
            ZuiControlInvalidate(cp, TRUE);
            break;
        }
        default:
            break;
        }
        break;
    }
    case Proc_OnPaint: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        RECT *rc = &cp->m_rcItem;

        if (p->MouseType == 1)
        {
            ZuiDrawRect(gp, ARGB(200, 0, 0, 0), rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 1);//鼠标悬停
        }
        else if (cp->m_bFocused)
        {
            ZuiDrawRect(gp, ARGB(200, 34, 255, 255), rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 1);//焦点
        }
        else
        {
            ZuiDrawRect(gp, ARGB(200, 0, 30, 255), rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 1);//通常
        }
        //画文本
        ZRect r;
        MAKEZRECT(r, rc->left + 5, rc->top + 5, rc->right - rc->left - 10, rc->bottom - rc->top - 10);
        ZuiDrawString(gp, p->sf, cp->m_sText, &r);

        //画光标
        if (p->type)
            ZuiDrawLine(gp, ARGB(255, 0, 0, 0), rc->left + p->x + 5, rc->top + 2, rc->left + p->x + 5, rc->bottom - 4, 1);
        return 0;
        break;
    }
    case Proc_SetAttribute: {
        break;
    }
    case Proc_GetImePoint: {
        ZPoint pt;
        pt.x = p->x + 7;
        pt.y = 5;
        return &pt;
    }
    case Proc_OnInit: {
        break;
    }
    case Proc_GetControlFlags: {
        return ZFLAG_SETCURSOR;
        break;
    }
    case Proc_OnCreate: {
        p = (ZuiEdit)ZuiMalloc(sizeof(ZEdit));
        memset(p, 0, sizeof(ZEdit));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        p->old_call = cp->call;
        cp->m_sText = ZuiMalloc(sizeof(_ZuiText) * 1024);
        p->StrBufLen = 1024;
        cp->m_sText[0] = L'\0';
        p->sf = ZuiCreateStringFormat(L"微软雅黑", 12, ARGB(255, 255, 0, 0), ARGB(255, 255, 255, 255), ZTS_VALIGN_MIDDLE | ZTS_SHADOW);
        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;

        ZuiFree(p);

        return old_call(ProcId, cp, 0, Param1, Param2, Param3);
    }
    case Proc_GetType:
        return (ZuiAny)Type_Edit;
    case Proc_CoreInit:
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit:
        return (ZuiAny)TRUE;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
}




