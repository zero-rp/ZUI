#include "control.h"
#include "tree.h"
#include "function.h"
#include "resdb.h"
#include "template.h"
#include "builder.h"
#include <control/Register.h>
#include <platform/platform.h>
//创建控件
ZEXPORT ZuiControl ZCALL NewZuiControl(ZuiText classname, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    ZuiControl p = (ZuiControl)malloc(sizeof(ZControl));
    if (p) {
        memset(p, 0, sizeof(ZControl));
        p->m_pParent = NULL;
        p->m_bUpdateNeeded = TRUE;
        p->m_bVisible = TRUE;
        p->m_bFocused = FALSE;
        p->m_bEnabled = TRUE;
        p->m_bMouseEnabled = TRUE;
        p->m_bKeyboardEnabled = TRUE;
        p->m_bFloat = FALSE;
        p->m_bSetPos = FALSE;
        p->m_chShortcut = '\0';
        p->m_nTooltipWidth = 300;
        p->m_aAnime = NULL;

        p->m_cXY.cx = p->m_cXY.cy = 0;
        p->m_cxyFixed.cx = p->m_cxyFixed.cy = 0;
        p->m_cxyMin.cx = p->m_cxyMin.cy = 0;
        p->m_cxyMax.cx = p->m_cxyMax.cy = 9999;


        p->m_piFloatPercent.left = p->m_piFloatPercent.top = p->m_piFloatPercent.right = p->m_piFloatPercent.bottom = 0.0f;

        p->call = ZuiDefaultControlProc;

        //p->m_dwBorderColor = ARGB(255, 255, 0, 0);
#if RUN_DEBUG
        p->m_sClassName = ZuiWcsdup(classname);
#endif // RUN_DEBUG

        //通知重载的对象开始创建
        //没有重载的
        /*查找类名*/
        wchar_t name[256];
        int l = (int)wcslen(classname);
        if (l > 255)
            return p;
        memset(name, 0, 256 * sizeof(wchar_t));
        memcpy(name, classname, l * sizeof(wchar_t));
        wcslwr(name);

        ZClass theNode = { 0 };
        ZClass *node;
        theNode.key = Zui_Hash(name);
        node = RB_FIND(_ZClass_Tree, Global_ControlClass, &theNode);
        if (node) {
            p->m_sUserData = node->cb(Proc_OnCreate, p, 0, Param1, Param2, Param3);
            p->call = node->cb;
            return p;
        }
        //在模版内查找
        ZTemplate theTemp = { 0 };
        ZTemplate *temp;
        theTemp.key = Zui_Hash(name);
        temp = RB_FIND(_ZTemplate_Tree, Global_TemplateClass, &theTemp);
        if (temp) {
            ZuiLoadTemplate(temp->node, p, Param1, Param2, Param3);
        }
        return p;
    }
    return NULL;
}
//销毁控件
ZEXPORT void ZCALL FreeZuiControl(ZuiControl p, ZuiBool Delayed)
{
    if (!Delayed)
        ZuiControlCall(Proc_OnDestroy, p, NULL, NULL, NULL);
    else
        ZuiOsAddDelayedCleanup(p->m_pOs, p);
}
//-------------------------------------------------------------------------------------------------

//控件默认处理函数
ZEXPORT ZuiAny ZCALL ZuiDefaultControlProc(ZuiInt ProcId, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    switch (ProcId)
    {
    case Proc_Invalidate: {
        ZRect invalidateRc = p->m_rcItem;
        if (!p->m_bVisible) return 0;
        {
            ZuiControl pParent = p;
            ZRect rcTemp;
            ZRect *rcParent;
            while (pParent = pParent->m_pParent)
            {
                rcTemp = invalidateRc;
                rcParent = (ZRect *)ZuiControlCall(Proc_GetPos, pParent, NULL, NULL, NULL);
                if (!IntersectRect(&invalidateRc, &rcTemp, rcParent))
                {
                    return 0;
                }
            }
            //重置动画
            if (Param1 && p->m_aAnime)
                p->m_aAnime->steup = 0;
            if (p->m_pOs != NULL) ZuiOsInvalidateRect(p->m_pOs, &invalidateRc);
        }
        return 0;
    }
    case Proc_Activate: {
        if (!p->m_bVisible) return FALSE;
        if (!p->m_bEnabled) return FALSE;
    }
    case Proc_SetVisible: {
        BOOL v = p->m_bVisible;
        if (p->m_bVisible == (BOOL)Param1)
            return 0;
        p->m_bVisible = (BOOL)Param1;
        if (p->m_bFocused)
            p->m_bFocused = FALSE;
        if (!(BOOL)Param1 && p->m_pOs && p->m_pOs->m_pFocus == p) {
            ZuiOsSetFocus(p->m_pOs, NULL, TRUE);
        }
        if (p->m_bVisible != v) {
            ZuiControlNeedParentUpdate(p);
        }
        break;
    }
    case Proc_GetVisible: {
        return (ZuiAny)p->m_bVisible;
    }
    case Proc_SetText: {
        ZuiControlInvalidate(p, TRUE);
		CONTROL_SETSTR(p->m_sText, Param1,ZuiText);
        break;
    }
    case Proc_GetText: {
        return p->m_sText;
    }
    case Proc_SetName: {
		CONTROL_SETSTR(p->m_sName, Param1, ZuiText);
        break;
    }
    case Proc_SetTooltip: {
		CONTROL_SETSTR(p->m_sToolTip, Param1, ZuiText);
        break;
    }
    case Proc_GetPos: {
        return (void *)&p->m_rcItem;
        break;
    }
    case Proc_SetPos: {// 只有控件为float的时候，外部调用SetPos和Move才是有效的，位置参数是相对父控件的位置
        ZRect *rc = (ZRect *)Param1;
        BOOL bNeedInvalidate = (BOOL)Param2;
        ZRect invalidateRc = p->m_rcItem;
        ZuiBool bSize = FALSE;
        if (rc->right < rc->left) rc->right = rc->left;
        if (rc->bottom < rc->top) rc->bottom = rc->top;

        if (IsRectEmpty(&invalidateRc)) invalidateRc = *rc;
        //防止不必要的调用
        if (rc->right - rc->left != p->m_rcItem.right - p->m_rcItem.left ||
            rc->bottom - rc->top != p->m_rcItem.bottom - p->m_rcItem.top)
            bSize = TRUE;

        if (p->m_bFloat) {
            ZuiControl pParent = p->m_pParent;
            if (pParent != NULL) {
                ZRect *rcParentPos = (ZRect *)ZuiControlCall(Proc_GetPos, pParent, NULL, NULL, NULL);
                ZRect rcCtrl = { rcParentPos->left + rc->left, rcParentPos->top + rc->top,
                    rcParentPos->left + rc->right, rcParentPos->top + rc->bottom };
                p->m_rcItem = rcCtrl;

                ZuiInt width = rcParentPos->right - rcParentPos->left;
                ZuiInt height = rcParentPos->bottom - rcParentPos->top;
                ZRect rcPercent = { (ZuiInt)(width*p->m_piFloatPercent.left), (ZuiInt)(height*p->m_piFloatPercent.top),
                    (ZuiInt)(width*p->m_piFloatPercent.right), (ZuiInt)(height*p->m_piFloatPercent.bottom) };
                p->m_cXY.cx = rc->left - rcPercent.left;
                p->m_cXY.cy = rc->top - rcPercent.top;
                p->m_cxyFixed.cx = rc->right - rcPercent.right - p->m_cXY.cx;
                p->m_cxyFixed.cy = rc->bottom - rcPercent.bottom - p->m_cXY.cy;
            }
        }
        else {
            memcpy(&p->m_rcItem, rc, sizeof(ZRect));
        }



        if (p->m_pOs == NULL)
            return 0;

        if (!p->m_bSetPos && bSize) {
            p->m_bSetPos = TRUE;
            ZuiControlCall(Proc_OnSize, p, (ZuiAny)(p->m_rcItem.right - p->m_rcItem.left), (ZuiAny)(p->m_rcItem.bottom - p->m_rcItem.top), NULL);
            p->m_bSetPos = FALSE;
        }

        p->m_bUpdateNeeded = FALSE;

        if (bNeedInvalidate && p->m_bVisible) {
            Rect_Join(&invalidateRc, &p->m_rcItem);
            ZuiControl pParent = p;
            ZRect rcTemp;
            ZRect *rcParent;
            while (pParent = pParent->m_pParent) {
                if (!pParent->m_bVisible)
                    return 0;
                rcTemp = invalidateRc;
                rcParent = (ZRect *)ZuiControlCall(Proc_GetPos, pParent, NULL, NULL, NULL);
                if (!IntersectRect(&invalidateRc, &rcTemp, rcParent))
                    return 0;
            }
            ZuiOsInvalidateRect(p->m_pOs, &invalidateRc);
        }
        break;
    }
    case Proc_OnSize: {
        if (p->m_aAnime)
            p->m_aAnime->OnSize(p, Param1, Param2);
#if (defined HAVE_JS) && (HAVE_JS == 1)
        if (p->m_rOnsize) {
            duv_push_ref(p->m_pOs->m_ctx, p->m_rOnsize);
            ZuiBuilderJs_pushControl(p->m_pOs->m_ctx, p);
            duk_push_int(p->m_pOs->m_ctx, Param1);
            duk_push_int(p->m_pOs->m_ctx, Param2);
            if (duk_pcall_method(p->m_pOs->m_ctx, 3)) {
                LOG_DUK(p->m_pOs->m_ctx);
            }
            duk_pop(p->m_pOs->m_ctx);
        }
#endif
        ZuiControlNotify(L"onsize", p, Param1, Param2, NULL);
        break;
    }
    case Proc_SetOs: {
        p->m_pOs = (ZuiOsWindow)Param1;
        p->m_pParent = (ZuiControl)Param2;
        if (Param3 && p->m_pParent)
            ZuiControlCall(Proc_OnInit, p, NULL, NULL, NULL);
        break;
    }
    case Proc_OnEvent: {
        switch (((TEventUI *)Param1)->Type)
        {
        case ZEVENT_SETCURSOR:
        {
            ZuiOsSetCursor(ZDC_ARROW);
            return 0;
        }
        case ZEVENT_SETFOCUS:
        {
            p->m_bFocused = TRUE;
            ZuiControlInvalidate(p, TRUE);
            return 0;
        }
        case ZEVENT_KILLFOCUS:
        {
            p->m_bFocused = FALSE;
            ZuiControlInvalidate(p, TRUE);
            return 0;
        }
        case ZEVENT_MOUSELEAVE: {
#if (defined HAVE_JS) && (HAVE_JS == 1)
            if (p->m_rOnmouseleave) {
                duv_push_ref(p->m_pOs->m_ctx, p->m_rOnmouseleave);
                ZuiBuilderJs_pushControl(p->m_pOs->m_ctx, p);
                if(duk_pcall_method(p->m_pOs->m_ctx, 1)) {
                    LOG_DUK(p->m_pOs->m_ctx);
                }
                duk_pop(p->m_pOs->m_ctx);
            }
#endif
            ZuiControlNotify(L"onmouseleave", p, NULL, NULL, NULL);
        }
                                break;
        case ZEVENT_MOUSEENTER: {
#if (defined HAVE_JS) && (HAVE_JS == 1)
            if (p->m_rOnmouseenter) {
                duv_push_ref(p->m_pOs->m_ctx, p->m_rOnmouseenter);
                ZuiBuilderJs_pushControl(p->m_pOs->m_ctx, p);
                duk_push_int(p->m_pOs->m_ctx, ((TEventUI *)Param1)->ptMouse.x);
                duk_push_int(p->m_pOs->m_ctx, ((TEventUI *)Param1)->ptMouse.y);
                if (duk_pcall_method(p->m_pOs->m_ctx, 3)) {
                    LOG_DUK(p->m_pOs->m_ctx);
                }
                duk_pop(p->m_pOs->m_ctx);
            }
#endif
            ZuiControlNotify(L"onmouseenter", p, (ZuiAny)(((TEventUI *)Param1)->ptMouse.x), (ZuiAny)(((TEventUI *)Param1)->ptMouse.y), NULL);
        }
                                break;
        case ZEVENT_LBUTTONDOWN: {
#if (defined HAVE_JS) && (HAVE_JS == 1)
            if (p->m_rOnlbuttondown) {
                duv_push_ref(p->m_pOs->m_ctx, p->m_rOnlbuttondown);
                ZuiBuilderJs_pushControl(p->m_pOs->m_ctx, p);
                duk_push_int(p->m_pOs->m_ctx, ((TEventUI *)Param1)->ptMouse.x);
                duk_push_int(p->m_pOs->m_ctx, ((TEventUI *)Param1)->ptMouse.y);
                if (duk_pcall_method(p->m_pOs->m_ctx, 3)) {
                    LOG_DUK(p->m_pOs->m_ctx);
                }
                duk_pop(p->m_pOs->m_ctx);
            }
#endif
            ZuiControlNotify(L"onlbuttondown", p, (ZuiAny)(((TEventUI *)Param1)->ptMouse.x), (ZuiAny)(((TEventUI *)Param1)->ptMouse.y), NULL);
        }
                                 break;
        case ZEVENT_LBUTTONUP: {
#if (defined HAVE_JS) && (HAVE_JS == 1)
            if (p->m_rOnclick) {
                duv_push_ref(p->m_pOs->m_ctx, p->m_rOnclick);
                ZuiBuilderJs_pushControl(p->m_pOs->m_ctx, p);
                duk_push_int(p->m_pOs->m_ctx, ((TEventUI *)Param1)->ptMouse.x);
                duk_push_int(p->m_pOs->m_ctx, ((TEventUI *)Param1)->ptMouse.y);
                if (duk_pcall_method(p->m_pOs->m_ctx, 3)) {
                    LOG_DUK(p->m_pOs->m_ctx);
                }
                duk_pop(p->m_pOs->m_ctx);
            }
#endif
            ZuiControlNotify(L"onclick", p, (ZuiAny)(((TEventUI *)Param1)->ptMouse.x), (ZuiAny)(((TEventUI *)Param1)->ptMouse.y), NULL);
            break;
        }
        case ZEVENT_CHAR: {
#if (defined HAVE_JS) && (HAVE_JS == 1)
            if (p->m_rOnchar) {
                duv_push_ref(p->m_pOs->m_ctx, p->m_rOnchar);
                ZuiBuilderJs_pushControl(p->m_pOs->m_ctx, p);
                duk_push_int(p->m_pOs->m_ctx, ((TEventUI *)Param1)->wParam);
                if (duk_pcall_method(p->m_pOs->m_ctx, 2)) {
                    LOG_DUK(p->m_pOs->m_ctx);
                }
                duk_pop(p->m_pOs->m_ctx);
            }
#endif
            ZuiControlNotify(L"onchar", p, &((TEventUI *)Param1)->wParam, NULL, NULL);
        }
                          break;
        default:
            break;
        }
        if (p->m_pParent != NULL)
            ZuiControlCall(Proc_OnEvent, p->m_pParent, Param1, NULL, NULL);
        if (p->m_aAnime)
            p->m_aAnime->OnEvent(p, Param1);
        break;
    }
    case Proc_GetMinWidth: {
        return (void *)p->m_cxyMin.cx;
        break;
    }
    case Proc_SetMinWidth: {
        if (p->m_cxyMin.cx == (ZuiInt)Param1)
            return 0;
        if ((ZuiInt)Param1 < 0)
            return 0;
        p->m_cxyMin.cx = (ZuiInt)Param1;
        ZuiControlNeedParentUpdate(p);
        break;
    }
	case Proc_GetBorderWidth: {
		return (void *)p->m_dwBorderWidth;
		break;
	}
	case Proc_SetBorderWidth: {
		if (p->m_dwBorderWidth == (ZuiInt)Param1)
			return 0;
		if ((ZuiInt)Param1 < 0)
			return 0;
		p->m_dwBorderWidth = (ZuiInt)Param1;
		ZuiControlNeedParentUpdate(p);
		break;
	}
    case Proc_GetMaxWidth: {
        return (void *)p->m_cxyMax.cx;
        break;
    }
    case Proc_SetMaxWidth: {
        if (p->m_cxyMax.cx == (ZuiInt)Param1)
            return 0;
        if ((ZuiInt)Param1 < 0)
            return 0;
        p->m_cxyMax.cx = (ZuiInt)Param1;
        ZuiControlNeedParentUpdate(p);
        break;
    }
    case Proc_GetMinHeight: {
        return (void *)p->m_cxyMin.cy;
        break;
    }
    case Proc_SetMinHeight: {
        if (p->m_cxyMin.cy == (ZuiInt)Param1)
            return 0;
        if ((ZuiInt)Param1 < 0)
            return 0;
        p->m_cxyMin.cy = (ZuiInt)Param1;
        ZuiControlNeedParentUpdate(p);
        break;
    }
    case Proc_GetMaxHeight: {
        return (void *)p->m_cxyMax.cy;
        break;
    }
    case Proc_SetMaxHeight: {
        if (p->m_cxyMax.cy == (ZuiInt)Param1)
            return 0;
        if ((ZuiInt)Param1 < 0)
            return 0;
        p->m_cxyMax.cy = (ZuiInt)Param1;
        ZuiControlNeedParentUpdate(p);
        break;
    }
    case Proc_GetWidth: {
        return (void *)(p->m_rcItem.right - p->m_rcItem.left);
        break;
    }
    case Proc_GetHeight: {
        return (void *)(p->m_rcItem.bottom - p->m_rcItem.top);
        break;
    }
    case Proc_GetX: {
        return (void *)p->m_rcItem.left;
        break;
    }
    case Proc_GetY: {
        return (void *)p->m_rcItem.top;
        break;
    }
    case Proc_SetFloatPercent: {
        memcpy(&p->m_piFloatPercent, Param1, sizeof(ZRect));
        ZuiControlNeedParentUpdate(p);
        break;
    }
    case Proc_GetPadding: {
        return (void *)&p->m_rcPadding;
        break;
    }
    case Proc_SetPadding: {
        memcpy(&p->m_rcPadding, Param1, sizeof(ZRect));
        ZuiControlNeedParentUpdate(p);
        break;
    }
    case Proc_GetFixedXY: {
        return (void *)&p->m_cXY;
        break;
    }
    case Proc_SetFixedXY: {
        p->m_cXY.cx = (ZuiInt)Param1;
        p->m_cXY.cy = (ZuiInt)Param2;
        ZuiControlNeedParentUpdate(p);
        break;
    }
    case Proc_GetFixedWidth: {
        return (void *)p->m_cxyFixed.cx;
        break;
    }
    case Proc_SetFixedWidth: {
        if ((ZuiInt)Param1 < 0)
            return 0;
        p->m_cxyFixed.cx = (ZuiInt)Param1;
        if (!Param2)
            ZuiControlNeedParentUpdate(p);
        break;
    }
    case Proc_GetFixedHeight: {
        return (void *)p->m_cxyFixed.cy;
        break;
    }
    case Proc_SetFixedHeight: {
        if ((ZuiInt)Param1 < 0)
            return 0;
        p->m_cxyFixed.cy = (ZuiInt)Param1;
        if (!Param2)
            ZuiControlNeedParentUpdate(p);
        break;
    }
    case Proc_GetRelativePos: {
        ZuiControl pParent = p->m_pParent;
        if (pParent != NULL) {
            ZRect *rcParentPos = (ZRect *)ZuiControlCall(Proc_GetPos, pParent, NULL, NULL, NULL);
            ZRect rcRelativePos = p->m_rcItem;
            OffsetRect(&rcRelativePos, -rcParentPos->left, -rcParentPos->top);
            return &rcRelativePos;
        }
        break;
    }
    case Proc_SetFloat: {
        if (p->m_bFloat == (BOOL)Param1)
            return 0;
        p->m_bFloat = (BOOL)Param1;
        ZuiControlNeedParentUpdate(p);
        break;
    }
    case Proc_SetEnabled: {
        if (p->m_bEnabled == (ZuiBool)Param1)
            return 0;

        p->m_bEnabled = (ZuiBool)Param1;
        ZuiControlInvalidate(p, TRUE);
        break;
    }
    case Proc_SetFocus: {
        if (p->m_pOs != NULL)
            ZuiOsSetFocus(p->m_pOs, p, FALSE);
        break;
    }
    case Proc_SetDrag: {
        if (p->m_drag == (ZuiBool)Param1)
            return 0;

        p->m_drag = (ZuiBool)Param1;
        break;
    }
    case Proc_EstimateSize: {
        return (void *)&p->m_cxyFixed;
    }
    case Proc_FindControl: {
        if (((ZuiUInt)Param3 & ZFIND_VISIBLE) != 0 && !p->m_bVisible)
            return NULL;
        if (((ZuiUInt)Param3 & ZFIND_ENABLED) != 0 && !p->m_bEnabled)
            return NULL;//激活
        if (((ZuiUInt)Param3 & ZFIND_HITTEST) != 0 && (!p->m_bMouseEnabled || !ZuiIsPointInRect(&p->m_rcItem, Param2)))
            return NULL;
        if (((ZuiUInt)Param3 & ZFIND_UPDATETEST) != 0 && ((FINDCONTROLPROC)Param1)(p, Param2) != NULL)
            return NULL;
        return ((FINDCONTROLPROC)Param1)(p, Param2);
        break;
    }
    case Proc_SetBkColor: {
        p->m_BkgColor = (ZuiColor)Param1;
        ZuiControlInvalidate(p, TRUE);
        break;
    }
    case Proc_SetBorderColor: {
		if (!p->m_dwBorderColor && !p->m_dwBorderWidth)
			p->m_dwBorderWidth++;
        p->m_dwBorderColor = (ZuiColor)Param1;
        ZuiControlInvalidate(p, TRUE);
        break;
    }
    case Proc_SetBkImage: {
        if (p->m_BkgImg)
            ZuiResDBDelRes(p->m_BkgImg);
        p->m_BkgImg = Param1;
        ZuiControlInvalidate(p, TRUE);
        break;
    }
    case Proc_OnPaint: {
        //开始绘制
        ZuiControlCall(Proc_OnPaintBkColor, p, Param1, Param2, NULL);
        ZuiControlCall(Proc_OnPaintBkImage, p, Param1, Param2, NULL);
        ZuiControlCall(Proc_OnPaintStatusImage, p, Param1, Param2, NULL);
        ZuiControlCall(Proc_OnPaintText, p, Param1, Param2, NULL);
        ZuiControlCall(Proc_OnPaintBorder, p, Param1, Param2, NULL);
        break;
    }
    case Proc_OnPaintBkColor: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = (ZRect *)&p->m_rcItem;
        if (p->m_BkgColor)
            ZuiDrawFillRect(gp, p->m_BkgColor, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top);
        break;
    }
    case Proc_OnPaintBkImage: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = &p->m_rcItem;
        if (p->m_BkgImg) {
            ZuiImage img = p->m_BkgImg->p;
            ZuiDrawImageEx(gp, img, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, img->Width, img->Height, 255);
        }
        break;
    }
    case Proc_OnPaintBorder: {
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = &p->m_rcItem;
		if (p->m_dwBorderColor) {
			if(p->m_dwBorderWidth)
				ZuiDrawRect(gp, p->m_dwBorderColor, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, p->m_dwBorderWidth);
			else
				ZuiDrawRect(gp, p->m_dwBorderColor, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 1);
		}
        break;
    }
    case Proc_OnDestroy: {
        if (p->m_aAnime)
            ZuiAnimationFree(p->m_aAnime);
        if (p->m_sText)
            free(p->m_sText);
        if (p->m_sName)
            free(p->m_sName);
        if (p->m_sToolTip)
            free(p->m_sToolTip);
        if (p->m_BkgImg)
            ZuiResDBDelRes(p->m_BkgImg);
        if (p->m_pParent && !Param1)
            ZuiControlCall(Proc_Layout_Remove, p->m_pParent, p, (ZuiAny)TRUE, NULL);
        if (p->m_pOs != NULL)
            ZuiOsReapObjects(p->m_pOs, p);
#if (defined HAVE_JS) && (HAVE_JS == 1)
        if (p->m_rOnclick)
            duv_unref(p->m_pOs->m_ctx, p->m_rOnclick);
        if (p->m_rOnmouseleave)
            duv_unref(p->m_pOs->m_ctx, p->m_rOnmouseleave);
        if (p->m_rOnmouseenter)
            duv_unref(p->m_pOs->m_ctx, p->m_rOnmouseenter);
        if (p->m_rOnlbuttondown)
            duv_unref(p->m_pOs->m_ctx, p->m_rOnlbuttondown);
        if (p->m_rOnchar)
            duv_unref(p->m_pOs->m_ctx, p->m_rOnchar);
        if(p->m_rOnsize)
            duv_unref(p->m_pOs->m_ctx, p->m_rOnchar);
#endif
#if RUN_DEBUG
        free(p->m_sClassName);
#endif // RUN_DEBUG
        free(p);
        break;
    }
    case Proc_SetAttribute: {
        if (wcscmp(Param1, L"text") == 0) ZuiControlCall(Proc_SetText, p, Param2, NULL, NULL);
        else if (wcscmp(Param1, L"tooltip") == 0) ZuiControlCall(Proc_SetTooltip, p, Param2, NULL, NULL);
        else if (wcscmp(Param1, L"width") == 0) ZuiControlCall(Proc_SetFixedWidth, p, (ZuiAny)(_wtoi(Param2)), NULL, NULL);
		else if (wcscmp(Param1, L"borderwidth") == 0) ZuiControlCall(Proc_SetBorderWidth, p, (ZuiAny)(_wtoi(Param2)), NULL, NULL);
        else if (wcscmp(Param1, L"height") == 0) ZuiControlCall(Proc_SetFixedHeight, p, (ZuiAny)(_wtoi(Param2)), NULL, NULL);
        else if (wcscmp(Param1, L"minwidth") == 0) ZuiControlCall(Proc_SetMinWidth, p, (ZuiAny)(_wtoi(Param2)), NULL, NULL);
        else if (wcscmp(Param1, L"minheight") == 0) ZuiControlCall(Proc_SetMinHeight, p, (ZuiAny)(_wtoi(Param2)), NULL, NULL);
        else if (wcscmp(Param1, L"maxwidth") == 0) ZuiControlCall(Proc_SetMaxWidth, p, (ZuiAny)(_wtoi(Param2)), NULL, NULL);
        else if (wcscmp(Param1, L"maxheight") == 0) ZuiControlCall(Proc_SetMaxHeight, p, (ZuiAny)(_wtoi(Param2)), NULL, NULL);
        else if (wcscmp(Param1, L"bkcolor") == 0) {
			ZuiColor clrColor;
			clrColor = ZuiStr2Color(Param2);
            ZuiControlCall(Proc_SetBkColor, p, (ZuiAny)clrColor, NULL, NULL);
        }
        else if (wcscmp(Param1, L"drag") == 0) ZuiControlCall(Proc_SetDrag, p, (ZuiAny)(wcscmp(Param2, L"true") == 0 ? TRUE : FALSE), NULL, NULL);
        else if (wcscmp(Param1, L"bkimage") == 0) ZuiControlCall(Proc_SetBkImage, p, ZuiResDBGetRes(Param2, ZREST_IMG), NULL, NULL);
        else if (wcscmp(Param1, L"padding") == 0) {
            ZRect rcPadding = { 0 };
            ZuiText pstr = NULL;
            rcPadding.left = _tcstol(Param2, &pstr, 10);  ASSERT(pstr);
            rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZuiControlCall(Proc_SetPadding, p, &rcPadding, NULL, NULL);
        }
        else if (wcscmp(Param1, L"bordercolor") == 0) {
			ZuiColor clrColor;
			clrColor = ZuiStr2Color(Param2);
            ZuiControlCall(Proc_SetBorderColor, p, (ZuiAny)clrColor, NULL, NULL);
        }
        else if (wcscmp(Param1, L"name") == 0) ZuiControlCall(Proc_SetName, p, Param2, NULL, NULL);
        else if (wcscmp(Param1, L"float") == 0) {
            if (wcschr(Param2, ',') == 0) {
                ZuiControlCall(Proc_SetFloat, p, (ZuiAny)(wcscmp(Param2, L"true") == 0 ? TRUE : FALSE), NULL, NULL);
            }
            else {
                ZRectR piFloatPercent = { 0 };
                ZuiText pstr = NULL;
                piFloatPercent.left = _tcstod(Param2, &pstr);  ASSERT(pstr);
                piFloatPercent.top = _tcstod(pstr + 1, &pstr);    ASSERT(pstr);
                piFloatPercent.right = _tcstod(pstr + 1, &pstr);  ASSERT(pstr);
                piFloatPercent.bottom = _tcstod(pstr + 1, &pstr); ASSERT(pstr);
                ZuiControlCall(Proc_SetFloatPercent, p, &piFloatPercent, NULL, NULL);
                ZuiControlCall(Proc_SetFloat, p, (ZuiAny)TRUE, NULL, NULL);
            }
        }
        else if (wcscmp(Param1, L"visible") == 0) ZuiControlCall(Proc_SetVisible, p, (ZuiAny)(wcscmp(Param2, L"true") == 0 ? TRUE : FALSE), NULL, NULL);
        else if (wcscmp(Param1, L"pos") == 0) {
            ZRect rcPos = { 0 };
            ZuiText pstr = NULL;
            rcPos.left = _tcstol(Param2, &pstr, 10);  ASSERT(pstr);
            rcPos.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcPos.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcPos.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZuiControlCall(Proc_SetFixedXY, p, (ZuiAny)rcPos.left, (ZuiAny)rcPos.top, NULL);
            ZuiControlCall(Proc_SetFixedWidth, p, (ZuiAny)(rcPos.right - rcPos.left), NULL, NULL);
            ZuiControlCall(Proc_SetFixedHeight, p, (ZuiAny)(rcPos.bottom - rcPos.top), NULL, NULL);
        }
        else if (wcscmp(Param1, L"enabled") == 0) ZuiControlCall(Proc_SetEnabled, p, (ZuiAny)(wcscmp(Param2, L"true") == 0 ? TRUE : FALSE), NULL, NULL);
        break;
    }
    case Proc_GetAttribute: {
        break;
    }
#if (defined HAVE_JS) && (HAVE_JS == 1)
    case Proc_JsGet: {
        duk_context *ctx = (duk_context *)Param1;
        switch ((ZuiInt)Param2)
        {
        case  Js_Id_root: {
            ZuiBuilderJs_pushControl(ctx, p->m_pOs->m_pRoot);
            return 1;
        }
        case  Js_Id_parent: {
            ZuiBuilderJs_pushControl(ctx, p->m_pParent);
            return 1;
        }
        case  Js_Id_text: {
            duk_push_string_w(ctx, ZuiControlCall(Proc_GetText, p, NULL, NULL, NULL));
            return 1;
        }
        case  Js_Id_tooltip: {
            duk_push_string_w(ctx, p->m_sToolTip);
            return 1;
        }
        case  Js_Id_width: {
            duk_push_int(ctx, p->m_rcItem.right - p->m_rcItem.left);
            return 1;
        }
        case  Js_Id_height: {
            duk_push_int(ctx, p->m_rcItem.bottom - p->m_rcItem.top);
            return 1;
        }
        case  Js_Id_minwidth: {
            duk_push_int(ctx, p->m_cxyMin.cx);
            return 1;
        }
        case  Js_Id_minheight: {
            duk_push_int(ctx, p->m_cxyMin.cy);
            return 1;
        }
        case  Js_Id_maxwidth: {
            duk_push_int(ctx, p->m_cxyMax.cx);
            return 1;
        }
        case  Js_Id_maxheight: {
            duk_push_int(ctx, p->m_cxyMax.cy);
            return 1;
        }
        case  Js_Id_bkcolor: {
            duk_push_int(ctx, p->m_BkgColor);
            return 1;
        }
        case  Js_Id_drag: {
            duk_push_boolean(ctx, p->m_drag);
            return;
        }
        case  Js_Id_rect: {
            //    else if (wcscmp(Param1, L"rect") == 0) {
            //        js_newobject(Param2);
            //        js_pushnumber(Param2, p->m_rcItem.left);
            //        js_setproperty(Param2, -2, L"left");
            //        js_pushnumber(Param2, p->m_rcItem.top);
            //        js_setproperty(Param2, -2, L"top");
            //        js_pushnumber(Param2, p->m_rcItem.bottom);
            //        js_setproperty(Param2, -2, L"bottom");
            //        js_pushnumber(Param2, p->m_rcItem.right);
            //        js_setproperty(Param2, -2, L"right");
            //    }
            return 1;
        }
        case  Js_Id_enabled: {
            duk_push_boolean(ctx, p->m_bEnabled);
            return 1;
        }
        case  Js_Id_visible: {
            duk_push_boolean(ctx, p->m_bVisible);
            return 1;
        }
        case Js_Id_left: {
            duk_push_int(ctx, ZuiControlCall(Proc_GetX, p, NULL, NULL, NULL));
            return 1;
        }
        case Js_Id_top: {
            duk_push_int(ctx, ZuiControlCall(Proc_GetY, p, NULL, NULL, NULL));
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
        case Js_Id_onmouseleave: {
            if (duk_is_function(ctx, 0)) {
                if (p->m_rOnmouseleave)
                    duv_unref(ctx, p->m_rOnmouseleave);
                duk_dup(ctx, 0);
                p->m_rOnmouseleave = duv_ref(ctx);
            }
            return 0;
        }
        case  Js_Id_onmouseenter: {
            if (duk_is_function(ctx, 0)) {
                if (p->m_rOnmouseenter)
                    duv_unref(ctx, p->m_rOnmouseenter);
                duk_dup(ctx, 0);
                p->m_rOnmouseenter = duv_ref(ctx);
            }
            return 0;
        }
        case  Js_Id_onlbuttondown: {
            if (duk_is_function(ctx, 0)) {
                if (p->m_rOnlbuttondown)
                    duv_unref(ctx, p->m_rOnlbuttondown);
                duk_dup(ctx, 0);
                p->m_rOnlbuttondown = duv_ref(ctx);
            }
            return 0;
        }
        case  Js_Id_onclick: {
            if (duk_is_function(ctx, 0)) {
                if (p->m_rOnclick)
                    duv_unref(ctx, p->m_rOnclick);
                duk_dup(ctx, 0);
                p->m_rOnclick = duv_ref(ctx);
            }
            return 0;
        }
        case  Js_Id_onchar: {
            if (duk_is_function(ctx, 0)) {
                if (p->m_rOnchar)
                    duv_unref(ctx, p->m_rOnchar);
                duk_dup(ctx, 0);
                p->m_rOnchar = duv_ref(ctx);
            }
            return 0;
        }
        case  Js_Id_onsize: {
            if (duk_is_function(ctx, 0)) {
                if (p->m_rOnsize)
                    duv_unref(ctx, p->m_rOnsize);
                duk_dup(ctx, 0);
                p->m_rOnsize = duv_ref(ctx);
            }
            return 0;
        }
        case  Js_Id_root: {
            return 0;
        }
        case  Js_Id_parent: {
            return 0;
        }
        case  Js_Id_text: {
            ZuiControlCall(Proc_SetText, p, duk_get_string_w(ctx, 0), NULL, NULL);
            return 0;
        }
        case Js_Id_name: {
            ZuiControlCall(Proc_SetName, p, duk_get_string_w(ctx, 0), NULL, NULL);
            return 0;
        }
        case  Js_Id_tooltip: {
            ZuiControlCall(Proc_SetTooltip, p, duk_get_string_w(ctx, 0), NULL, NULL);
            return 0;
        }
        case  Js_Id_width: {
            ZuiControlCall(Proc_SetFixedWidth, p, duk_to_int(ctx, 0), NULL, NULL);
            return 0;
        }
        case  Js_Id_height: {
            ZuiControlCall(Proc_SetFixedHeight, p, duk_to_int(ctx, 0), NULL, NULL);
            return 0;
        }
        case  Js_Id_minwidth: {
            ZuiControlCall(Proc_SetMinWidth, p, duk_to_int(ctx, 0), NULL, NULL);
            return 0;
        }
        case  Js_Id_minheight: {
            ZuiControlCall(Proc_SetMinHeight, p, duk_to_int(ctx, 0), NULL, NULL);
            return 0;
        }
        case  Js_Id_maxheight: {
            ZuiControlCall(Proc_SetMaxHeight, p, duk_to_int(ctx, 0), NULL, NULL);
            return 0;
        }
        case Js_Id_maxwidth: {
            ZuiControlCall(Proc_SetMaxWidth, p, duk_to_int(ctx, 0), NULL, NULL);
            return 0;
        }
        case Js_Id_padding: {
            ZRect rcPadding = { 0 };
            ZuiText pstr = NULL;
            rcPadding.left = _tcstol(duk_get_string_w(ctx, 0), &pstr, 10);  ASSERT(pstr);
            rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZuiControlCall(Proc_SetPadding, p, &rcPadding, NULL, NULL);
            return 0;
        }
        case  Js_Id_bkcolor: {
            ZuiControlCall(Proc_SetBkColor, p, duk_to_uint(ctx, 0), NULL, NULL);
            return 0;
        }
        case  Js_Id_drag: {
            ZuiControlCall(Proc_SetDrag, p, duk_to_boolean(ctx, 0), NULL, NULL);
            return 0;
        }
        case  Js_Id_rect: {
            return 0;
        }
        case  Js_Id_enabled: {
            ZuiControlCall(Proc_SetEnabled, p, duk_to_boolean(ctx, 0), NULL, NULL);
            return 0;
        }
        case  Js_Id_visible: {
            ZuiControlCall(Proc_SetVisible, p, duk_to_boolean(ctx, 0), NULL, NULL);
            return 0;
        }
        case Js_Id_left: {
            ZuiControlCall(Proc_SetFixedXY, p, duk_to_int(ctx, 0), ((ZSize *)ZuiControlCall(Proc_GetFixedXY, p, NULL, NULL, NULL))->cy, NULL);
            return 0;
        }
        case Js_Id_top: {
            ZuiControlCall(Proc_SetFixedXY, p, ((ZSize *)ZuiControlCall(Proc_GetFixedXY, p, NULL, NULL, NULL))->cx, duk_to_int(ctx, 0), NULL);
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
        case Js_Id_close: {
            ZuiOsAddDelayedCleanup(p->m_pOs, p);
            return 0;
        }
        default:
            break;
        }
        break;
    }
    case Proc_JsInit: {
        ZuiBuilderControlInit(Param1, "clos", Js_Id_close, FALSE);

        ZuiBuilderControlInit(Param1, "root", Js_Id_root, TRUE);
        ZuiBuilderControlInit(Param1, "parent", Js_Id_parent, TRUE);
        ZuiBuilderControlInit(Param1, "text", Js_Id_text, TRUE);
        ZuiBuilderControlInit(Param1, "name", Js_Id_name, TRUE);
        ZuiBuilderControlInit(Param1, "tooltip", Js_Id_tooltip, TRUE);
        ZuiBuilderControlInit(Param1, "width", Js_Id_width, TRUE);
        ZuiBuilderControlInit(Param1, "height", Js_Id_height, TRUE);
        ZuiBuilderControlInit(Param1, "minwidth", Js_Id_minwidth, TRUE);
        ZuiBuilderControlInit(Param1, "minheight", Js_Id_minheight, TRUE);
        ZuiBuilderControlInit(Param1, "maxwidth", Js_Id_maxwidth, TRUE);
        ZuiBuilderControlInit(Param1, "maxheight", Js_Id_maxheight, TRUE);
        ZuiBuilderControlInit(Param1, "bkcolor", Js_Id_bkcolor, TRUE);
        ZuiBuilderControlInit(Param1, "drag", Js_Id_drag, TRUE);
        ZuiBuilderControlInit(Param1, "rect", Js_Id_rect, TRUE);
        ZuiBuilderControlInit(Param1, "enabled", Js_Id_enabled, TRUE);
        ZuiBuilderControlInit(Param1, "visible", Js_Id_visible, TRUE);
        ZuiBuilderControlInit(Param1, "padding", Js_Id_padding, TRUE);
        ZuiBuilderControlInit(Param1, "left", Js_Id_left, TRUE);
        ZuiBuilderControlInit(Param1, "top", Js_Id_top, TRUE);

        ZuiBuilderControlInit(Param1, "onmouseleave", Js_Id_onmouseleave, TRUE);
        ZuiBuilderControlInit(Param1, "onmouseenter", Js_Id_onmouseenter, TRUE);
        ZuiBuilderControlInit(Param1, "onlbuttondown", Js_Id_onlbuttondown, TRUE);
        ZuiBuilderControlInit(Param1, "onclick", Js_Id_onclick, TRUE);
        ZuiBuilderControlInit(Param1, "onchar", Js_Id_onchar, TRUE);
        ZuiBuilderControlInit(Param1, "onsize", Js_Id_onsize, TRUE);
        break;
    }

                      //case Proc_JsPut: {

                      //    else if (wcscmp(Param1, L"bkimage") == 0) ZuiControlCall(Proc_SetBkImage, p, ZuiResDBGetRes(js_tostring(J, -1), ZREST_IMG), NULL, NULL);
                      //    else if (wcscmp(Param1, L"bordercolor") == 0) ZuiControlCall(Proc_SetBorderColor, p, js_toint32(J, -1), NULL, NULL);
                      //    else if (wcscmp(Param1, L"float") == 0) {

                      //    }
#endif
    case Proc_GetObject: {
        if (Param1 == (ZuiAny)Type_Null)
            return p;
        break;
    }
    case Proc_GetType: {
        return (ZuiAny)Type_Null;
    }
    case Proc_CoreInit: {
        return (ZuiAny)TRUE;
    }
    case Proc_GetControlFlags: {
        return 0;
    }
    default:
        break;
    }
    return 0;
}

//调用控件函数
ZEXPORT ZuiAny ZCALL ZuiControlCall(ZuiInt ProcId, ZuiControl p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    if (!p)
        return NULL;
    if (p->call)
    {
        return p->call(ProcId, p, p->m_sUserData, Param1, Param2, Param3);
    }
    return NULL;
}

ZEXPORT ZuiAny ZCALL ZuiControlNotify(ZuiText msg, ZuiControl p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    if (p->m_pNotify)
    {
        return p->m_pNotify(msg, p, p->m_sUserData, Param1, Param2, Param3);
    }
    return NULL;
}

ZEXPORT ZuiVoid ZCALL ZuiControlRegNotify(ZuiControl p, ZNotifyProc pNotify) {
    if (p)
    {
        p->m_pNotify = pNotify;
    }
}


//-------------------------------------------------------------------------------------------------

ZEXPORT ZuiControl ZCALL ZuiControlFindName(ZuiControl p, ZuiText Name) {
    if (!p)
        return NULL;
    if (!p->m_pOs)
        return NULL;
    if (!p->m_pOs->m_pRoot)
        return NULL;
    return (ZuiControl)ZuiControlCall(Proc_FindControl, p->m_pOs->m_pRoot, __FindControlFromName, Name, (void *)(ZFIND_ALL));
}

ZEXPORT ZuiVoid ZCALL ZuiControlInvalidate(ZuiControl p, ZuiBool ResetAnimation)
{
    ZuiControlCall(Proc_Invalidate, p, (ZuiAny)ResetAnimation, NULL, NULL);
}

ZEXPORT ZuiVoid ZCALL ZuiControlNeedUpdate(ZuiControl p)
{
    if (!p->m_bVisible) return;
    p->m_bUpdateNeeded = TRUE;
    ZuiControlInvalidate(p, TRUE);

    if (p->m_pOs != NULL)
        p->m_pOs->m_bUpdateNeeded = TRUE;
}

ZEXPORT ZuiVoid ZCALL ZuiControlNeedParentUpdate(ZuiControl p)
{
    if (p->m_pParent) {
        ZuiControlNeedUpdate(p->m_pParent);
        ZuiControlInvalidate(p->m_pParent, TRUE);
    }
    else {
        ZuiControlNeedUpdate(p);
    }

    if (p->m_pOs != NULL)
        p->m_pOs->m_bUpdateNeeded = TRUE;
}

ZEXPORT ZuiVoid ZCALL ZuiControlEvent(ZuiControl p, TEventUI *event)
{
    ZuiControlCall(Proc_OnEvent, p, event, NULL, NULL);
}

//-------------------------------------------------------------------------------------------------


ZuiVoid ZuiClientToScreen(ZuiControl p, ZuiPoint pt) {
    ZuiOsClientToScreen(p, pt);
}

ZuiVoid ZuiScreenToClient(ZuiControl p, ZuiPoint pt) {
    ZuiOsScreenToClient(p, pt);
}



//-------------------------------------------------------------------------------------------------
//控件查找函数

ZuiControl ZCALL __FindControlFromCount(ZuiControl pThis, ZuiAny pData)
{
    int* pnCount = (int*)(pData);
    (*pnCount)++;
    return NULL;  // Count all controls
}

ZuiControl ZCALL __FindControlFromPoint(ZuiControl pThis, ZuiAny pData)
{
    return ZuiIsPointInRect(ZuiControlCall(Proc_GetPos, pThis, NULL, NULL, NULL), pData) ? pThis : NULL;
}

ZuiControl ZCALL __FindControlFromTab(ZuiControl pThis, ZuiAny pData)
{
    FINDTABINFO* pInfo = (FINDTABINFO*)(pData);
    if (pInfo->pFocus == pThis) {
        if (pInfo->bForward) pInfo->bNextIsIt = TRUE;
        return pInfo->bForward ? NULL : pInfo->pLast;
    }
    if (((int)ZuiControlCall(Proc_GetControlFlags, pThis, NULL, NULL, NULL) & ZFLAG_TABSTOP) == 0)
        return NULL;
    pInfo->pLast = pThis;
    if (pInfo->bNextIsIt) return pThis;
    if (pInfo->pFocus == NULL) return pThis;
    return NULL;  // Examine all controls
}

ZuiControl ZCALL __FindControlFromShortcut(ZuiControl pThis, ZuiAny pData)
{
    FINDSHORTCUT* pFS = (FINDSHORTCUT*)(pData);
    if (!pThis->m_bVisible) return NULL;
    if (pFS->ch == toupper(pThis->m_chShortcut)) pFS->bPickNext = TRUE;
    return pFS->bPickNext ? pThis : NULL;
}

ZuiControl ZCALL __FindControlsFromUpdate(ZuiControl pThis, ZuiAny pData)
{
    if (pThis->m_bUpdateNeeded) {
        darray_append(pThis->m_pOs->m_aFoundControls, (ZuiAny)pThis);
        return pThis;
    }
    return NULL;
}

ZuiControl ZCALL __FindControlFromName(ZuiControl pThis, ZuiAny pData)
{
    wchar_t* pstrName = (wchar_t *)(pData);
    if (!pThis->m_sName) return NULL;
    return (wcscmp(pThis->m_sName, pstrName) == 0) ? pThis : NULL;
}




