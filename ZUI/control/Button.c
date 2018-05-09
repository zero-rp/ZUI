#include "Button.h"
#include "Label.h"
#include <core/control.h>
#include <core/resdb.h>
#include <core/builder.h>
#include <platform/platform.h>
#include <stdlib.h>
#if (defined HAVE_JS) && (HAVE_JS == 1)
#include <duktape.h>
#endif
ZEXPORT ZuiAny ZCALL ZuiButtonProc(ZuiInt ProcId, ZuiControl cp, ZuiButton p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        switch (event->Type)
        {
		case ZEVENT_KILLFOCUS:
		{
			p->type = 0;
			break;
		}
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
        ZuiControlCall(Proc_OnPaintBkImage, cp, Param1, Param2, NULL);
        ZuiControlCall(Proc_OnPaintStatusImage, cp, Param1, Param2, NULL);
        ZuiControlCall(Proc_OnPaintText, cp, Param1, Param2, NULL);
        ZuiControlCall(Proc_OnPaintBorder, cp, Param1, Param2, NULL);
        return 0;
    }
#if (defined HAVE_JS) && (HAVE_JS == 1)
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
#endif
    case Proc_SetEnabled: {
        if (0 == (ZuiBool)Param1)
            p->type = -1;
        else
            p->type = 0;
        break;
    }
    case Proc_OnPaintStatusImage: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = (ZRect *)&cp->m_rcItem;
        ZuiImage img;
        if (p->type == 0) {
            if (p->m_BtnRes) {
                img = p->m_BtnRes->p;
                memcpy(&img->src, &p->m_rcBtn[0], sizeof(ZRect));
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            else {
                ZuiDrawFillRect(gp, p->m_ColorNormal, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
            }
        }
        else if (p->type == 1) {
            if (p->m_BtnRes) {
                img = p->m_BtnRes->p;
                memcpy(&img->src, &p->m_rcBtn[1], sizeof(ZRect));
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            else {
                ZuiDrawFillRect(gp, p->m_ColorHot, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
            }
        }
        else if (p->type == 2) {
            if (p->m_BtnRes) {
                img = p->m_BtnRes->p;
                memcpy(&img->src, &p->m_rcBtn[2], sizeof(ZRect));
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            else {
                ZuiDrawFillRect(gp, p->m_ColorPushed, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
            }
        }
        else  {
            if (p->m_BtnRes) {
                img = p->m_BtnRes->p;
                memcpy(&img->src, &p->m_rcBtn[3], sizeof(ZRect));
                ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
            }
            else {
                ZuiDrawFillRect(gp, p->m_ColorDisabled, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
            }
        }
        return 0;
    }
    case Proc_Button_SetRes: {
        if (p->m_BtnRes)
            ZuiResDBDelRes(p->m_BtnRes);
        p->m_BtnRes = (ZuiRes)Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case Proc_Button_SetColor: {
        switch ((int)Param1) {
        case Button_N_Color: {
            p->m_ColorNormal = (ZuiColor)Param2;
            break;
        }
        case Button_H_Color: {
            p->m_ColorHot = (ZuiColor)Param2;
            break;
        }
        case Button_P_Color: {
            p->m_ColorPushed = (ZuiColor)Param2;
            break;
        }
        case Button_F_Color: {
            p->m_ColorFocused = (ZuiColor)Param2;
            break;
        }
        case Button_D_Color: {
            p->m_ColorDisabled = (ZuiColor)Param2;
            break;
        }
        default:
            break;
        }
        ZuiControlInvalidate(cp,TRUE);
        return 0;
    }
    case Proc_SetAnimation: {
        if (cp->m_aAnime)
            ZuiAnimationFree(cp->m_aAnime);
        cp->m_aAnime = ZuiAnimationNew(Param1, Param2, Param3);
        return 0;
    }
    case Proc_SetAttribute: {
        if (wcscmp(Param1, L"btnsrc") == 0) {
            ZRect rcTmp = { 0 };
            ZuiText pstr = NULL;
            rcTmp.left = _tcstol(Param2, &pstr, 10);  ASSERT(pstr);
            rcTmp.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcTmp.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcTmp.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);

            for (int i = 0; i < 4; i++) {
                p->m_rcBtn[i].left = rcTmp.left + i * rcTmp.right;
                p->m_rcBtn[i].top = rcTmp.top;
                p->m_rcBtn[i].right = rcTmp.right;
                p->m_rcBtn[i].bottom = rcTmp.bottom;
            }
        }
        else if (wcscmp(Param1, L"btnres") == 0)
            ZuiControlCall(Proc_Button_SetRes, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
        
        else if (wcscmp(Param1, L"normalcolor") == 0)
            ZuiControlCall(Proc_Button_SetColor, cp, (ZuiAny)Button_N_Color, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (wcscmp(Param1, L"hotcolor") == 0)
            ZuiControlCall(Proc_Button_SetColor, cp, (ZuiAny)Button_H_Color, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (wcscmp(Param1, L"pushedcolor") == 0)
            ZuiControlCall(Proc_Button_SetColor, cp, (ZuiAny)Button_P_Color, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (wcscmp(Param1, L"focusedcolor") == 0)
            ZuiControlCall(Proc_Button_SetColor, cp, (ZuiAny)Button_F_Color, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (wcscmp(Param1, L"disabledcolor") == 0)
            ZuiControlCall(Proc_Button_SetColor, cp, (ZuiAny)Button_D_Color, (ZuiAny)ZuiStr2Color(Param2), NULL);
        break;
    }
    case Proc_OnCreate: {
        p = (ZuiButton)malloc(sizeof(ZButton));
        memset(p, 0, sizeof(ZButton));

        //保存原来的回调地址,创建成功后回调地址指向当前函数
        p->old_udata = ZuiLabelProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiLabelProc;

        p->m_ColorNormal = ARGB(200, 0, 3, 255);
        p->m_ColorHot = ARGB(200, 0, 255, 255);
        p->m_ColorPushed = ARGB(200, 255, 255, 255);
        p->m_ColorDisabled = ARGB(140, 140, 140, 255);

        ((ZuiLabel)p->old_udata)->m_uTextStyle |= ZDT_CENTER;

        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;
        if (p->m_BtnRes)
            ZuiResDBDelRes(p->m_BtnRes);

        old_call(ProcId, cp, old_udata, Param1, Param2, Param3);

        free(p);

        return 0;
    }
    case Proc_GetObject:
        if (Param1 == (ZuiAny)Type_Button)
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




