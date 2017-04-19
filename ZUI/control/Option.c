#include <ZUI.h>

ZEXPORT ZuiAny ZCALL ZuiOptionProc(ZuiInt ProcId, ZuiControl cp, ZuiOption p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_OnEvent: {
        TEventUI *event = (TEventUI *)Param1;
        switch (event->Type)
        {
        case ZEVENT_LBUTTONUP: {
            ZuiControlCall(Proc_Option_SetSelected, cp, !ZuiControlCall(Proc_Option_GetSelected, cp, NULL, NULL, NULL), NULL, NULL);
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
                if (p->m_ResSelected) {
                    img = p->m_ResSelected->p;
                    ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, img->Width, img->Height, 255);
                }
                else {
                    ZuiDrawFillRect(gp, p->m_ColorSelected, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
                }
            }
            else if (((ZuiButton)p->old_udata)->type == 1) {
                if (p->m_ResSelectedHot) {
                    img = p->m_ResSelectedHot->p;
                    ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, img->Width, img->Height, 255);
                }
                else {
                    ZuiDrawFillRect(gp, p->m_ColorSelectedHot, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
                }
            }
            else if (((ZuiButton)p->old_udata)->type == 2) {
                if (p->m_ResSelectedPushed) {
                    img = p->m_ResSelectedPushed->p;
                    ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, img->Width, img->Height, 255);
                }
                else {
                    ZuiDrawFillRect(gp, p->m_ColorSelectedPushed, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
                }
            }
            ZuiControlCall(Proc_OnPaintText, cp, Param1, Param2, NULL);//绘制文本
            return 0;//选择状态下不由按钮控件绘制
        }
        break;
    }
    case Proc_Option_SetSelected: {
        if (p->m_bSelected == Param1) return 0;
        p->m_bSelected = Param1;

        if (p->m_bGroup) {
            if (Param1) {
                for (size_t i = 0; i < (size_t)ZuiControlCall(Proc_Layout_GetCount, cp->m_pParent, NULL, NULL, NULL); i++)
                {
                    ZuiControl pControl;
                    if ((pControl = ZuiControlCall(Proc_Layout_GetItemAt, cp->m_pParent, i, NULL, NULL)) != cp)
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
                    if ((pControl = ZuiControlCall(Proc_Layout_GetItemAt, cp->m_pParent, i, NULL, NULL)) != cp)
                    {
                        if (pControl != cp) {
                            select += ZuiControlCall(Proc_Option_GetSelected, pControl, NULL, NULL, NULL) ? 1 : 0;
                        }

                    }
                }
                if (!select) {
                    p->m_bSelected = !p->m_bSelected;
                    return;
                }
            }
        }
#if (defined HAVE_JS) && (HAVE_JS == 1)
        if (p->m_rOnselected) {
            duv_push_ref(cp->m_pManager->m_ctx, p->m_rOnselected);
            ZuiBuilderJs_pushControl(cp->m_pManager->m_ctx, p);
            duk_push_boolean(cp->m_pManager->m_ctx, Param1);
            if (duk_pcall_method(cp->m_pManager->m_ctx, 2)) {
                LOG_DUK(cp->m_pManager->m_ctx);
            }
            duk_pop(cp->m_pManager->m_ctx);
        }
#endif
        ZuiControlNotify(L"selectchanged", cp, Param1, NULL, NULL);
        ZuiControlInvalidate(cp, TRUE);
        break;
    }
    case Proc_Option_SetGroup: {
        p->m_bGroup = Param1;
        break;
    }
    case Proc_Option_GetSelected: {
        return p->m_bSelected;
    }
    case Proc_Option_SetResSelected: {
        if (p->m_ResSelected)
            ZuiResDBDelRes(p->m_ResSelected);
        p->m_ResSelected = Param1;
        break;
    }
    case Proc_Option_SetResSelectedHot: {
        if (p->m_ResSelectedHot)
            ZuiResDBDelRes(p->m_ResSelectedHot);
        p->m_ResSelectedHot = Param1;
        break;
    }
    case Proc_Option_SetResSelectedPushed: {
        if (p->m_ResSelectedPushed)
            ZuiResDBDelRes(p->m_ResSelectedPushed);
        p->m_ResSelectedPushed = Param1;
        break;
    }
    case Proc_SetAttribute: {
        if (_tcscmp(Param1, _T("group")) == 0) ZuiControlCall(Proc_Option_SetGroup, cp, (ZuiAny)(wcscmp(Param2, L"true") == 0 ? TRUE : FALSE), NULL, NULL);
        else if (_tcscmp(Param1, _T("selected")) == 0) ZuiControlCall(Proc_Option_SetSelected, cp, (ZuiAny)(wcscmp(Param2, L"true") == 0 ? TRUE : FALSE), NULL, NULL);
        else if (_tcscmp(Param1, _T("selectedimage")) == 0) ZuiControlCall(Proc_Option_SetResSelected, cp, (ZuiAny)ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
        else if (_tcscmp(Param1, _T("selectedhotimage")) == 0) ZuiControlCall(Proc_Option_SetResSelectedHot, cp, (ZuiAny)ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
        else if (_tcscmp(Param1, _T("selectedpushedimage")) == 0) ZuiControlCall(Proc_Option_SetResSelectedPushed, cp, (ZuiAny)ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
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
        p = (ZuiOption)ZuiMalloc(sizeof(ZOption));
        memset(p, 0, sizeof(ZOption));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        //创建继承的控件 保存数据指针
        p->old_udata = ZuiButtonProc(Proc_OnCreate, cp, 0, 0, 0, 0);
        p->old_call = (ZCtlProc)&ZuiButtonProc;
        p->m_ColorSelected = ARGB(200, 12, 111, 255);		//选中的普通状态
        p->m_ColorSelectedHot = ARGB(200, 0, 255, 255);		//选中的点燃状态
        p->m_ColorSelectedPushed = ARGB(200, 255, 255, 255);	//选中的按下状态
        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;
        ZuiAny old_udata = p->old_udata;

        ZuiFree(p);

        return old_call(ProcId, cp, old_udata, Param1, Param2, Param3);
    }
    case Proc_GetObject:
        if (Param1 == Type_Option)
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




