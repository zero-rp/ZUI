﻿#include "Option.h"
#include <core/control.h>
#include <core/resdb.h>
#include "Button.h"
#if (defined HAVE_JS) && (HAVE_JS == 1)
#include <duktape.h>
#endif
ZEXPORT ZuiAny ZCALL ZuiOptionProc(ZuiInt ProcId, ZuiControl cp, ZuiOption p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        switch (event->Type)
        {
        case ZEVENT_LBUTTONUP: {
            ZuiControlCall(Proc_Option_SetSelected, cp, (ZuiAny)(!ZuiControlCall(Proc_Option_GetSelected, cp, NULL, NULL, NULL)), NULL, NULL);
            break;
        }
        default:
            break;
        }
        break;
    }
    case Proc_OnPaint: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = &cp->m_rcItem;
        if (p->m_bSelected) {
            ZuiImage img;
            if (((ZuiButton)p->old_udata)->type == 0) {
                if (p->m_OptRes) {
                    img = p->m_OptRes->p;
                    memcpy(&img->src, &p->m_rcOpt[0], sizeof(ZRect));
                    ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
                }
                else {
                    ZuiDrawFillRect(gp, p->m_ColorSelected, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
                }
            }
            else if (((ZuiButton)p->old_udata)->type == 1) {
                if (p->m_OptRes) {
                    img = p->m_OptRes->p;
                    memcpy(&img->src, &p->m_rcOpt[1], sizeof(ZRect));
                    ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
                }
                else {
                    ZuiDrawFillRect(gp, p->m_ColorSelectedHot, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
                }
            }
            else if (((ZuiButton)p->old_udata)->type == 2) {
                if (p->m_OptRes) {
                    img = p->m_OptRes->p;
                    memcpy(&img->src, &p->m_rcOpt[2], sizeof(ZRect));
                    ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
                }
                else {
                    ZuiDrawFillRect(gp, p->m_ColorSelectedPushed, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
                }
            }
            else {
                if (p->m_OptRes) {
                    img = p->m_OptRes->p;
                    memcpy(&img->src, &p->m_rcOpt[3], sizeof(ZRect));
                    ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0, 0, 255);
                }
                else {
                    ZuiDrawFillRect(gp, p->m_ColorSelectedDisabled, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
                }
            }
            ZuiControlCall(Proc_OnPaintText, cp, Param1, Param2, NULL);//绘制文本
            return 0;//选择状态下不由按钮控件绘制
        }
        break;
    }
    case Proc_Option_SetSelected: {
        if (p->m_bSelected == (ZuiBool)Param1) return 0;
        p->m_bSelected = (ZuiBool)Param1;

        if (p->m_bGroup) {
            if (Param1) {
                for (size_t i = 0; i < (size_t)ZuiControlCall(Proc_Layout_GetCount, cp->m_pParent, NULL, NULL, NULL); i++)
                {
                    ZuiControl pControl;
                    if ((pControl = ZuiControlCall(Proc_Layout_GetItemAt, cp->m_pParent, (ZuiAny)i, NULL, NULL)) != cp)
                    {
                        if (pControl != cp) {
                            ZuiControlCall(Proc_Option_SetSelected, pControl, FALSE, NULL, NULL);
                        }

                    }

                }
            }
            else {
                //在分组情况下反选,需要保证至少一个被选中
                int select = 0;
                for (size_t i = 0; i < (size_t)ZuiControlCall(Proc_Layout_GetCount, cp->m_pParent, NULL, NULL, NULL); i++)
                {
                    ZuiControl pControl;
                    if ((pControl = ZuiControlCall(Proc_Layout_GetItemAt, cp->m_pParent, (ZuiAny)i, NULL, NULL)) != cp)
                    {
                        if (pControl != cp) {
                            select += ZuiControlCall(Proc_Option_GetSelected, pControl, NULL, NULL, NULL) ? 1 : 0;
                        }

                    }
                }
                if (!select) {
                    p->m_bSelected = !p->m_bSelected;
                    return 0;
                }
            }
        }
#if (defined HAVE_JS) && (HAVE_JS == 1)
        if (p->m_rOnselected) {
            duv_push_ref(cp->m_pOs->m_ctx, p->m_rOnselected);
            ZuiBuilderJs_pushControl(cp->m_pOs->m_ctx, p);
            duk_push_boolean(cp->m_pOs->m_ctx, Param1);
            if (duk_pcall_method(cp->m_pOs->m_ctx, 2)) {
                LOG_DUK(cp->m_pOs->m_ctx);
            }
            duk_pop(cp->m_pOs->m_ctx);
        }
#endif
        ZuiControlNotify(L"selectchanged", cp, Param1, NULL, NULL);
        ZuiControlInvalidate(cp, TRUE);
        break;
    }
    case Proc_Option_SetGroup: {
        p->m_bGroup = (ZuiBool)Param1;
        break;
    }
    case Proc_Option_GetSelected: {
        return (ZuiAny)p->m_bSelected;
    }
    case Proc_Option_SetRes: {
        if (p->m_OptRes)
            ZuiResDBDelRes(p->m_OptRes);
        p->m_OptRes = (ZuiRes)Param1;
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case Proc_Option_SetColor: {
        switch ((int)Param1) {
        case Option_SN_Color: {
            p->m_ColorSelected = (ZuiColor)Param2;
            break;
        }
        case Option_SH_Color: {
            p->m_ColorSelectedHot = (ZuiColor)Param2;
            break;
        }
        case Option_SP_Color: {
            p->m_ColorSelectedPushed = (ZuiColor)Param2;
            break;
        }
        case Option_SF_Color: {
            p->m_ColorSelectedFocused = (ZuiColor)Param2;
            break;
        }
        case Option_SD_Color: {
            p->m_ColorSelectedDisabled = (ZuiColor)Param2;
            break;
        }
        default:
            break;
        }
        ZuiControlInvalidate(cp, TRUE);
        return 0;
    }
    case Proc_SetAttribute: {
        if (_tcscmp(Param1, _T("group")) == 0)
            ZuiControlCall(Proc_Option_SetGroup, cp, (ZuiAny)(wcscmp(Param2, L"true") == 0 ? TRUE : FALSE), NULL, NULL);
        else if (_tcscmp(Param1, _T("selected")) == 0)
            ZuiControlCall(Proc_Option_SetSelected, cp, (ZuiAny)(wcscmp(Param2, L"true") == 0 ? TRUE : FALSE), NULL, NULL);
        else if (wcscmp(Param1, L"optsrc") == 0) {
            ZRect rcTmp = { 0 };
            ZuiText pstr = NULL;
            rcTmp.left = _tcstol(Param2, &pstr, 10);  ASSERT(pstr);
            rcTmp.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcTmp.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcTmp.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);

            for (int i = 0; i < 4; i++) {
                p->m_rcOpt[i].left = rcTmp.left + i * rcTmp.right;
                p->m_rcOpt[i].top = rcTmp.top;
                p->m_rcOpt[i].right = rcTmp.right;
                p->m_rcOpt[i].bottom = rcTmp.bottom;
            }
        }
        else if (wcscmp(Param1, L"optres") == 0)
            ZuiControlCall(Proc_Option_SetRes, cp, ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
        else if (wcscmp(Param1, L"selectedcolor") == 0)
            ZuiControlCall(Proc_Button_SetColor, cp, (ZuiAny)Option_SN_Color, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (wcscmp(Param1, L"hotselectedcolor") == 0)
            ZuiControlCall(Proc_Button_SetColor, cp, (ZuiAny)Option_SH_Color, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (wcscmp(Param1, L"pushedselectedcolor") == 0)
            ZuiControlCall(Proc_Button_SetColor, cp, (ZuiAny)Option_SP_Color, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (wcscmp(Param1, L"focusedselectedcolor") == 0)
            ZuiControlCall(Proc_Button_SetColor, cp, (ZuiAny)Option_SF_Color, (ZuiAny)ZuiStr2Color(Param2), NULL);
        else if (wcscmp(Param1, L"disabledselectedcolor") == 0)
            ZuiControlCall(Proc_Button_SetColor, cp, (ZuiAny)Option_SD_Color, (ZuiAny)ZuiStr2Color(Param2), NULL);
        break;
    }
#if (defined HAVE_JS) && (HAVE_JS == 1)
    case Proc_JsGet: {
        duk_context *ctx = (duk_context *)Param1;
        switch ((ZuiInt)Param2)
        {
        case  Js_Id_Option_selected: {
            duk_push_boolean(ctx, p->m_bSelected);
            return 1;
        }
        case Js_Id_Option_group: {
            duk_push_boolean(ctx, p->m_bGroup);
            return 1;
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
        case Js_Id_Option_selected: {
            ZuiControlCall(Proc_Option_SetSelected, cp, (ZuiAny)duk_to_boolean(ctx, 0), NULL, NULL);
            return 0;
        }
        case Js_Id_Option_group: {
            ZuiControlCall(Proc_Option_SetGroup, cp, (ZuiAny)duk_to_boolean(ctx, 0), NULL, NULL);
            return 0;
        }
        case Js_Id_Option_onselected: {
            if (duk_is_function(ctx, 0)) {
                if (p->m_rOnselected)
                    duv_unref(ctx, p->m_rOnselected);
                duk_dup(ctx, 0);
                p->m_rOnselected = duv_ref(ctx);
            }
            return 0;
        }
        default:
            break;
        }
        break;
    }
    case Proc_JsInit: {
        ZuiBuilderControlInit(Param1, "selected", Js_Id_Option_selected, TRUE);
        ZuiBuilderControlInit(Param1, "group", Js_Id_Option_group, TRUE);

        ZuiBuilderControlInit(Param1, "onselected", Js_Id_Option_onselected, TRUE);
        break;
    }
#endif
    case Proc_OnCreate: {
        p = (ZuiOption)malloc(sizeof(ZOption));
        memset(p, 0, sizeof(ZOption));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiButtonProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiButtonProc;
        p->m_ColorSelected = ARGB(200, 12, 111, 255);		//选中的普通状态
        p->m_ColorSelectedHot = ARGB(200, 0, 255, 255);		//选中的点燃状态
        p->m_ColorSelectedPushed = ARGB(200, 255, 255, 255);	//选中的按下状态
        p->m_ColorSelectedDisabled = ARGB(173, 173, 173, 255);
        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;
        if (p->m_OptRes)
            ZuiResDBDelRes(p->m_OptRes);
        old_call(ProcId, cp, old_udata, Param1, Param2, Param3);
        free(p);

        return 0;
    }
    case Proc_GetObject:
        if (Param1 == (ZuiAny)Type_Option)
            return (ZuiAny)p;
        break;
    case Proc_GetType:
        return (ZuiAny)Type_Option;
    case Proc_CoreInit:
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit:
        return (ZuiAny)TRUE;
    default:
        break;
    }
    return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}




