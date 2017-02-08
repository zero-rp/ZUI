#include <ZUI.h>



ZEXPORT ZuiAny ZCALL ZuiHtmlProc(ZuiInt ProcId, ZuiControl cp, ZuiHtml p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_CoreInit: {
        return (ZuiAny)TRUE;
        break;
    }
    case Proc_OnCreate: {
        p = (ZuiHtml)malloc(sizeof(ZHtml));
        memset(p, 0, sizeof(ZHtml));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        p->old_call = cp->call;
        p->cp = cp;

        return p;
        break;
    }
    case Proc_OnDestroy: {
        break;
    }
    case Proc_OnSize: {
        break;
    }
    case Proc_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        switch (event->Type)
        {
        case ZEVENT_TIMER: {
            break;
        }
        case ZEVENT_SETFOCUS:
            break;
        case ZEVENT_KILLFOCUS:
            break;
        case ZEVENT_CHAR: {
            break;
        }
        case ZEVENT_KEYUP: {
            break;
        }
        case ZEVENT_KEYDOWN: {
            break;
        }
        case ZEVENT_LBUTTONDOWN:
        case ZEVENT_LBUTTONUP:
        case ZEVENT_RBUTTONDOWN:
        case ZEVENT_LDBLCLICK:
        case ZEVENT_MOUSEMOVE: {
            break;
        }
        case ZEVENT_SCROLLWHEEL: {
            break;
        }
        case ZEVENT_SETCURSOR:
            break;
        default:
            break;
        }
        break;
    }
    case Proc_OnPaint: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        RECT *rc = &cp->m_rcItem;
        if (p->init) {
            //BitBlt(gp->hdc, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, wkeGetViewDC(p->view), 0, 0, SRCCOPY);
        }
        break;
    }
    case Proc_SetPos: {
        
        break;
    }
    case Proc_GetImePoint: {
        ZPoint pt;
        pt.x = 0;
        pt.y = 0;
        return &pt;
        break;
    }
    case Proc_OnInit: {

        break;
    }
    case Proc_GetControlFlags: {
        return (ZuiAny)ZFLAG_SETCURSOR;
        break;
    }
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2, Param3);
}

