#include <ZUI.h>

ZEXPORT ZuiAny ZCALL ZuiButtonProc(ZuiInt ProcId, ZuiControl cp, ZuiButton p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        switch (event->Type)
        {
        case ZEVENT_MOUSELEAVE: {
            p->type = 0;
            ZuiControlInvalidate(cp, TRUE);
        }
                                break;
        case ZEVENT_MOUSEENTER: {
            p->type = 1;
            ZuiControlInvalidate(cp, TRUE);
        }
                                break;
        case ZEVENT_LBUTTONDOWN: {
            p->type = 2;
            ZuiControlInvalidate(cp, TRUE);
        }
                                 break;
        case ZEVENT_LBUTTONUP: {
            p->type = 1;
            ZuiControlInvalidate(cp, TRUE);
            //return ZuiControlNotify(L"onclick", cp, event->ptMouse.x, JS_TNUMBER, event->ptMouse.y, JS_TNUMBER, NULL, NULL);
        }
                               break;
        default:
            break;
        }
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
    case Proc_JsSet: {
        duk_context *ctx = (duk_context *)Param1;
        switch ((ZuiInt)Param2)
        {
        case Js_Id_Button_normalimage: {
            ZuiControlCall(Proc_Button_SetResNormal, cp, ZuiResDBGetRes(duk_get_string_w(ctx, 0), ZREST_IMG), NULL, NULL);
            return 0;
        }
        case Js_Id_Button_hotimage: {
            ZuiControlCall(Proc_Button_SetResHot, cp, ZuiResDBGetRes(duk_get_string_w(ctx, 0), ZREST_IMG), NULL, NULL);
            return 0;
        }
        case Js_Id_Button_pushedimage: {
            ZuiControlCall(Proc_Button_SetResPushed, cp, ZuiResDBGetRes(duk_get_string_w(ctx, 0), ZREST_IMG), NULL, NULL);
            return 0;
        }
        case Js_Id_Button_focusedimage: {
            ZuiControlCall(Proc_Button_SetResFocused, cp, ZuiResDBGetRes(duk_get_string_w(ctx, 0), ZREST_IMG), NULL, NULL);
            return 0;
        }
        case Js_Id_Button_disabledimage: {
            ZuiControlCall(Proc_Button_SetResDisabled, cp, ZuiResDBGetRes(duk_get_string_w(ctx, 0), ZREST_IMG), NULL, NULL);
            return 0;
        }
        default:
            break;
        }
        break;
    }
    case Proc_JsInit: {
        ZuiBuilderControlInit(Param1, "normalimage", Js_Id_Button_normalimage, TRUE);
        ZuiBuilderControlInit(Param1, "hotimage", Js_Id_Button_hotimage, TRUE);
        ZuiBuilderControlInit(Param1, "pushedimage", Js_Id_Button_pushedimage, TRUE);
        ZuiBuilderControlInit(Param1, "focusedimage", Js_Id_Button_focusedimage, TRUE);
        ZuiBuilderControlInit(Param1, "disabledimage", Js_Id_Button_disabledimage, TRUE);
        break;
    }
    case Proc_OnPaintStatusImage: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        RECT *rc = (RECT *)Param2;
        HPEN hPen = 0;
        ZuiImage img;
        if (p->type == 0) {
            if (p->m_ResNormal) {
                img = p->m_ResNormal->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            else {
                ZuiDrawFillRect(gp, ARGB(200, 0, 3, 255), rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
            }
        }
        else if (p->type == 1) {
            if (p->m_ResHot) {
                img = p->m_ResHot->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            else {
                ZuiDrawFillRect(gp, ARGB(200, 0, 255, 255), rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
            }
        }
        else if (p->type == 2) {
            if (p->m_ResPushed) {
                img = p->m_ResPushed->p;
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            else {
                ZuiDrawFillRect(gp, ARGB(200, 255, 255, 255), rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
            }
        }
        break;
    }
    case Proc_Button_SetResNormal: {
        if (p->m_ResNormal)
            ZuiResDBDelRes(p->m_ResNormal);
        p->m_ResNormal = Param1;
        break;
    }
    case Proc_Button_SetResHot: {
        if (p->m_ResHot)
            ZuiResDBDelRes(p->m_ResHot);
        p->m_ResHot = Param1;
        break;
    }
    case Proc_Button_SetResPushed: {
        if (p->m_ResPushed)
            ZuiResDBDelRes(p->m_ResPushed);
        p->m_ResPushed = Param1;
        break;
    }
    case Proc_Button_SetResFocused: {
        if (p->m_ResFocused)
            ZuiResDBDelRes(p->m_ResFocused);
        p->m_ResFocused = Param1;
        break;
    }
    case Proc_Button_SetResDisabled: {
        if (p->m_ResDisabled)
            ZuiResDBDelRes(p->m_ResDisabled);
        p->m_ResDisabled = Param1;
        break;
    }
    case Proc_SetAnimation: {
        if (cp->m_aAnime)
            ZuiAnimationFree(cp->m_aAnime);
        cp->m_aAnime = ZuiAnimationNew(Param1, Param2, Param3);
        break;
    }
    case Proc_SetAttribute: {
        if (wcscmp(Param1, L"normalimage") == 0) ZuiControlCall(Proc_Button_SetResNormal, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
        else if (wcscmp(Param1, L"hotimage") == 0) ZuiControlCall(Proc_Button_SetResHot, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
        else if (wcscmp(Param1, L"pushedimage") == 0) ZuiControlCall(Proc_Button_SetResPushed, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
        else if (wcscmp(Param1, L"focusedimage") == 0) ZuiControlCall(Proc_Button_SetResFocused, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
        else if (wcscmp(Param1, L"disabledimage") == 0) ZuiControlCall(Proc_Button_SetResDisabled, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
        break;
    }
    case Proc_OnCreate: {
        p = (ZuiButton)ZuiMalloc(sizeof(ZButton));
        memset(p, 0, sizeof(ZButton));

        //保存原来的回调地址,创建成功后回调地址指向当前函数
        p->old_udata = ZuiLabelProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiLabelProc;
        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;
        if (p->m_ResNormal)
            ZuiResDBDelRes(p->m_ResNormal);
        if (p->m_ResHot)
            ZuiResDBDelRes(p->m_ResHot);
        if (p->m_ResPushed)
            ZuiResDBDelRes(p->m_ResPushed);
        if (p->m_ResFocused)
            ZuiResDBDelRes(p->m_ResFocused);
        if (p->m_ResDisabled)
            ZuiResDBDelRes(p->m_ResDisabled);

        ZuiFree(p);

        return old_call(ProcId, cp, old_udata, Param1, Param2, Param3);
    }
    case Proc_GetObject:
        if (Param1 == Type_Button)
            return (ZuiAny)p;
        break;
    case Proc_GetType:
        return (ZuiAny)Type_Button;
    case Proc_CoreInit:
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit:
        return (ZuiAny)TRUE;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}




