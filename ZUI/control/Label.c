#include "Label.h"
#include <core/control.h>
#include <core/resdb.h>
#include <core/function.h>
#include <core/builder.h>
#include <platform/platform.h>
#include <stdlib.h>
#if (defined HAVE_JS) && (HAVE_JS == 1)
#include <duktape.h>
#endif
ZEXPORT ZuiAny ZCALL ZuiLabelProc(ZuiInt ProcId, ZuiControl cp, ZuiLabel p, ZuiAny Param1, ZuiAny Param2) {
    switch (ProcId)
    {
    case Proc_OnPaintText: {
        ZuiColor tmpTColor;
        if (!cp->m_sText)
            return 0;
        if (cp->m_bEnabled)
            tmpTColor = p->m_cTextColor;
        else
            tmpTColor = p->m_cTextColorDisabled;
        ZuiGraphics gp = (ZuiGraphics)Param1;
        ZRect *rc = &cp->m_rcItem;
        ZRectR pt;
        pt.left = rc->left + p->m_rcPadding.left + cp->m_dwBorderWidth;
        pt.top = rc->top + p->m_rcPadding.top + cp->m_dwBorderWidth;
        pt.right = rc->right - p->m_rcPadding.right - cp->m_dwBorderWidth;
        pt.bottom = rc->bottom - p->m_rcPadding.bottom - cp->m_dwBorderWidth;
        if (p->m_rFont)
            ZuiDrawString(gp, p->m_rFont->p, cp->m_sText, wcslen(cp->m_sText), &pt, tmpTColor, p->m_uTextStyle);
        else
            ZuiDrawString(gp, Global_Font, cp->m_sText, wcslen(cp->m_sText), &pt, tmpTColor, p->m_uTextStyle);
        return 0;
    }
#if (defined HAVE_JS) && (HAVE_JS == 1)
    case Proc_JsSet: {
        duk_context *ctx = (duk_context *)Param1;
        switch ((ZuiInt)Param2)
        {
        case Js_Id_Label_font: {
            ZuiControlCall(Proc_Label_SetFont, cp, ZuiResDBGetRes(duk_get_string_w(ctx, 0), ZREST_FONT), NULL, NULL);
            return 0;
        }
        case Js_Id_Label_align: {
            ZuiText align = duk_get_string_w(ctx, 0);
            //横向对齐方式
            if (wcscmp(align, L"left") == 0) {
                p->m_uTextStyle &= ~(ZDT_CENTER | ZDT_RIGHT);
                p->m_uTextStyle |= ZDT_LEFT;
            }
            if (wcscmp(align, L"center") == 0) {
                p->m_uTextStyle &= ~(ZDT_LEFT | ZDT_RIGHT);
                p->m_uTextStyle |= ZDT_CENTER;
            }
            if (wcscmp(align, L"right") == 0) {
                p->m_uTextStyle &= ~(ZDT_LEFT | ZDT_CENTER);
                p->m_uTextStyle |= ZDT_RIGHT;
            }
            return 0;
        }
        case Js_Id_Label_valign: {
            ZuiText valign = duk_get_string_w(ctx, 0);
            //纵向对齐方式
            if (wcscmp(valign, L"top") == 0) {
                p->m_uTextStyle &= ~(ZDT_BOTTOM | ZDT_VCENTER | ZDT_WORDBREAK);
                p->m_uTextStyle |= (ZDT_TOP | ZDT_SINGLELINE);
            }
            if (wcscmp(valign, L"vcenter") == 0) {
                p->m_uTextStyle &= ~(ZDT_TOP | ZDT_BOTTOM | ZDT_WORDBREAK);
                p->m_uTextStyle |= (ZDT_VCENTER | ZDT_SINGLELINE);
            }
            if (wcscmp(valign, L"bottom") == 0) {
                p->m_uTextStyle &= ~(ZDT_TOP | ZDT_VCENTER | ZDT_WORDBREAK);
                p->m_uTextStyle |= (ZDT_BOTTOM | ZDT_SINGLELINE);
            }
            return 0;
        }
        case Js_Id_Label_textcolor: {
            ZuiControlCall(Proc_Label_SetTextColor, cp, duk_to_uint(ctx, 0), NULL, NULL);
            return 0;
        }
        case Js_Id_Label_textpadding: {
            return 0;
        }
        case Js_Id_Label_wordbreak: {
            return 0;
        }
        case Js_Id_Label_endellipsis: {
            return 0;
        }
        default:
            break;
        }
        break;
    }
    case Proc_JsInit: {
        ZuiBuilderControlInit(Param1, "font", Js_Id_Label_font, TRUE);
        ZuiBuilderControlInit(Param1, "align", Js_Id_Label_align, TRUE);
        ZuiBuilderControlInit(Param1, "valign", Js_Id_Label_valign, TRUE);
        ZuiBuilderControlInit(Param1, "textcolor", Js_Id_Label_textcolor, TRUE);
        ZuiBuilderControlInit(Param1, "textpadding", Js_Id_Label_textpadding, TRUE);
        ZuiBuilderControlInit(Param1, "wordbreak", Js_Id_Label_wordbreak, TRUE);
        ZuiBuilderControlInit(Param1, "endellipsis", Js_Id_Label_endellipsis, TRUE);
        break;
    }
#endif
    case Proc_Label_SetFont: {
        if (p->m_rFont)
            ZuiResDBDelRes(p->m_rFont);
        p->m_rFont = Param1;
        ZuiControlNeedUpdate(cp);
        return 0;
    }
    case Proc_Label_SetTextColor: {
        p->m_cTextColor = (ZuiColor)Param1;
        ZuiControlNeedUpdate(cp);
        return 0;
    }
    case Proc_Label_SetTextColorDisabled: {
        p->m_cTextColorDisabled = (ZuiColor)Param1;
        ZuiControlNeedUpdate(cp);
        return 0;
    }
    case Proc_Label_SetTextPadding: {
        memcpy(&p->m_rcPadding, Param1, sizeof(ZRect));
        ZuiControlNeedUpdate(cp);
        return 0;
    }
    case Proc_SetAttribute: {
        if (wcscmp(Param1, L"font") == 0) ZuiControlCall(Proc_Label_SetFont, cp, ZuiResDBGetRes(Param2, ZREST_FONT), NULL);
        if (wcscmp(Param1, L"align") == 0) {
            //横向对齐方式
            if (wcscmp(Param2, L"left") == 0) {
                p->m_uTextStyle &= ~(ZDT_CENTER | ZDT_RIGHT | ZDT_WORDBREAK | ZDT_EDITCONTROL);
                p->m_uTextStyle |= ZDT_LEFT;
            }
            if (wcscmp(Param2, L"center") == 0) {
                p->m_uTextStyle &= ~(ZDT_LEFT | ZDT_RIGHT | ZDT_WORDBREAK | ZDT_EDITCONTROL);
                p->m_uTextStyle |= ZDT_CENTER;
            }
            if (wcscmp(Param2, L"right") == 0) {
                p->m_uTextStyle &= ~(ZDT_LEFT | ZDT_CENTER | ZDT_WORDBREAK | ZDT_EDITCONTROL);
                p->m_uTextStyle |= ZDT_RIGHT;
            }
            ZuiControlNeedUpdate(cp);
        }
        else if (wcscmp(Param1, L"valign") == 0) {
            //纵向对齐方式
            if (wcscmp(Param2, L"top") == 0) {
                p->m_uTextStyle &= ~(ZDT_BOTTOM | ZDT_VCENTER | ZDT_WORDBREAK | ZDT_EDITCONTROL);
                p->m_uTextStyle |= (ZDT_TOP | ZDT_SINGLELINE);
            }
            if (wcscmp(Param2, L"vcenter") == 0) {
                p->m_uTextStyle &= ~(ZDT_TOP | ZDT_BOTTOM | ZDT_WORDBREAK | ZDT_EDITCONTROL);
                p->m_uTextStyle |= (ZDT_VCENTER | ZDT_SINGLELINE);
            }
            if (wcscmp(Param2, L"bottom") == 0) {
                p->m_uTextStyle &= ~(ZDT_TOP | ZDT_VCENTER | ZDT_WORDBREAK | ZDT_EDITCONTROL);
                p->m_uTextStyle |= (ZDT_BOTTOM | ZDT_SINGLELINE);
            }
            ZuiControlNeedUpdate(cp);
        }
        else if (wcscmp(Param1, L"textcolor") == 0) {
            //字体颜色
 /*           ZuiText pstr = NULL;
            ZuiColor clrColor;
            while (*(wchar_t *)Param2 > L'\0' && *(wchar_t *)Param2 <= L' ') Param2 = ZuiCharNext((wchar_t *)Param2);
            if (*(wchar_t *)Param2 == L'#') Param2 = ZuiCharNext((wchar_t *)Param2);*/
            ZuiColor clrColor = ZuiStr2Color(Param2);
            ZuiControlCall(Proc_Label_SetTextColor, cp, (ZuiAny)clrColor, NULL);
        }
        else if (wcscmp(Param1, L"textcolordisabled") == 0) {
            ZuiColor clrColor = ZuiStr2Color(Param2);
            ZuiControlCall(Proc_Label_SetTextColorDisabled, cp, (ZuiAny)clrColor, NULL);
        }
        else if (wcscmp(Param1, L"textpadding") == 0) {
            //字体边距
            ZRect rcPadding = { 0 };
            ZuiText pstr = NULL;
            rcPadding.left = _tcstol(Param2, &pstr, 10);  ASSERT(pstr);
            rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            ZuiControlCall(Proc_Label_SetTextPadding, cp, &rcPadding, NULL);
        }
        else if (wcscmp(Param1, L"wordbreak") == 0) {
            //自动换行
            if (wcscmp(Param2, L"true") == 0) {
                p->m_uTextStyle &= 0;  //清除对齐方式。
                p->m_uTextStyle |= ZDT_WORDBREAK | ZDT_EDITCONTROL;
            }
            else {
                p->m_uTextStyle &= ~ZDT_WORDBREAK & ~ZDT_EDITCONTROL;
                p->m_uTextStyle |= ZDT_SINGLELINE;
            }
        }
        else if (wcscmp(Param1, L"endellipsis") == 0) {
            //替换超出部分为...
            if (wcscmp(Param2, L"true") == 0) {
                p->m_uTextStyle &= ~(ZDT_WORDBREAK | ZDT_EDITCONTROL); //自动换行和超出替换互斥。
                p->m_uTextStyle |= ZDT_END_ELLIPSIS;
            }
            else p->m_uTextStyle &= ~ZDT_END_ELLIPSIS;
        }
        else {
            break;
        }
        return 0;
    }
    case Proc_OnCreate: {
        p = (ZuiLabel)malloc(sizeof(ZLabel));
        memset(p, 0, sizeof(ZLabel));
        //保存原来的回调地址,创建成功后回调地址指向当前函数
        p->old_call = cp->call;

        p->m_uTextStyle = ZDT_VCENTER | ZDT_SINGLELINE;
        p->m_cTextColor = ARGB(255, 0, 0, 0);
        p->m_cTextColorDisabled = ARGB(255, 176, 176, 176);
        ZRect rctmp = { 2,1,2,1 };
        p->m_rcPadding = rctmp;
        return p;
    }
    case Proc_OnDestroy: {
        ZCtlProc old_call = p->old_call;

        old_call(ProcId, cp, 0, Param1, Param2);
        if (p->m_rFont && !Param1) ZuiResDBDelRes(p->m_rFont);
        free(p);

        return 0;
    }
    case Proc_GetObject:
        if (_wcsicmp(Param1, (ZuiAny)Type_Label) == 0)
            return (ZuiAny)p;
        break;
    case Proc_GetType:
        return (ZuiAny)Type_Label;
    case Proc_CoreInit:
        return (ZuiAny)TRUE;
    case Proc_CoreUnInit:
        return (ZuiAny)NULL;
    default:
        break;
    }
    return p->old_call(ProcId, cp, 0, Param1, Param2);
}




