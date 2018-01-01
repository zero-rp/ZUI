#include <ZUI.h>
#include <stdlib.h>
#include <platform/platform.h>
#include <control/Register.h>
#include "control.h"
#include "function.h"
#include "resdb.h"
#include "template.h"
#include "builder.h"
ZuiText Global_DefaultFontName;     //系统默认字体名称
ZuiFont Global_Font;                //默认字体

void Rect_Join(ZRect *rc, ZRect *rc1)
{
    if (rc1->left < rc->left) rc->left = rc1->left;
    if (rc1->top < rc->top) rc->top = rc1->top;
    if (rc1->right > rc->right) rc->right = rc1->right;
    if (rc1->bottom > rc->bottom) rc->bottom = rc1->bottom;
}

uint32_t Zui_Hash(wchar_t* str) {
    uint32_t hash = 0;
    wchar_t ch;
    for (uint32_t i = 0; ch = (uint32_t)*str++; i++)
    {
        if ((i & 1) == 0)
        {
            hash ^= ((hash << 7) ^ ch ^ (hash >> 3));
        }
        else
        {
            hash ^= (~((hash << 11) ^ ch ^ (hash >> 5)));
        }
    }
    return hash;
}
ZuiText ZuiCharNext(ZuiText str) {
    return str + 1;
}
ZEXPORT ZuiBool ZCALL ZuiInit(ZuiInitConfig config) {
    if (!config->default_res)
        return FALSE;
#if (defined PLATFORM_OS_WIN)
    if (config && config->m_hInstance) {
        m_hInstance = config->m_hInstance;
    }
    else
    {
        m_hInstance = GetModuleHandle(NULL);
    }
#endif
#if RUN_DEBUG
    if (config && config->debug) {
        //启动调试器
        ZuiStartDebug();
    }
#endif
    /*初始化图形层*/
    if (!ZuiGraphInitialize()) {
        return FALSE;
    }
    /*初始化系统层*/
    if (!ZuiOsInitialize()) {
        return FALSE;
    }
    /*初始化全局变量*/
    {
#if (defined PLATFORM_OS_WIN)
        LOGFONT lf;
        SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lf, 0);
        Global_DefaultFontName = wcsdup(lf.lfFaceName);
        Global_Font = ZuiCreateFont(Global_DefaultFontName, 12, FALSE, FALSE);
#endif
    }
    /*初始化模版管理器*/
    if (!ZuiTemplateInit())
    {
        return FALSE;
    }
#if (defined HAVE_JS) && (HAVE_JS == 1)
    /*初始化导出接口*/
    if (!ZuiInitZuvFunc())
    {
        return FALSE;
    }
    /*初始化绑定器*/
    if (!ZuiBuilderInit())
    {
        return FALSE;
    }
#endif
    /*初始化资源池*/
    if (!ZuiResDBInit()) {
        return FALSE;
    }
    /*加载默认资源*/
    ZuiResDBGetRes(config->default_res, ZREST_ZIP);
    /*注册全局控件*/
    if (!ZuiClassInit())
    {
        return FALSE;
    }
    return TRUE;
}
ZEXPORT ZuiBool ZCALL ZuiUnInit() {
    /*反注册全局控件*/
    ZuiClassUnInit();
    /*反初始化模版管理器*/
    ZuiTemplateUnInit();
#if (defined HAVE_JS) && (HAVE_JS == 1)
    /*反初始化绑定器*/
    ZuiBuilderUnInit();
#endif
    /*反初始化资源池*/
    ZuiResDBUnInit();
    /*反初始化全局变量*/
    {
#if (defined PLATFORM_OS_WIN)
        if (Global_DefaultFontName) {
            free(Global_DefaultFontName);
            Global_DefaultFontName = NULL;
        }
        if (Global_Font) {
            ZuiDestroyFont(Global_Font);
            Global_Font = NULL;
        }
#endif
    }
    /*反初始化系统层*/
    ZuiOsUnInitialize();
    /*反初始化图形层*/
    ZuiGraphUnInitialize();
    return TRUE;
}
ZEXPORT ZuiInt ZCALL ZuiMsgLoop() {
    return ZuiOsMsgLoop();
}
ZEXPORT ZuiVoid ZCALL ZuiMsgLoop_exit() {
    ZuiOsMsgLoopExit();
}
ZEXPORT ZuiVoid ZCALL ZuiPostTask(ZuiTask task) {
    ZuiOsPostTask(task);
}

ZuiControl MsgBox_pRoot;
ZuiAny ZCALL MsgBox_Notify_ctl(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
    if (wcscmp(msg, L"onclick") == 0)
    {
        if (wcscmp(p->m_sName, L"WindowCtl_clos") == 0) {
            ZuiControlCall(Proc_OnClose,p->m_pOs->m_pRoot, (ZuiAny)ZuiCANCEL,NULL,NULL);
            //PostMessage(0, WM_APP + 10, 0, 0);
        }
		else if (wcscmp(p->m_sName, L"ok") == 0) {
			ZuiControlCall(Proc_OnClose, p->m_pOs->m_pRoot, (ZuiAny)ZuiOK, NULL, NULL);
		}
		else if (wcscmp(p->m_sName, L"cancel") == 0) {
			ZuiControlCall(Proc_OnClose, p->m_pOs->m_pRoot, (ZuiAny)ZuiCANCEL, NULL, NULL);
		}
    }
    return (ZuiAny)TRUE;
}

ZuiAny ZCALL MsgBox_Notify(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	if (wcscmp(msg, L"ondestroy") == 0) {
		ZuiMsgLoop_exit();
	}
	return (ZuiAny)TRUE;
}

ZEXPORT ZuiInt ZCALL ZuiMsgBox(ZuiControl rp, ZuiText text, ZuiText title) {
    ZuiControl p;
    MsgBox_pRoot = NewZuiControl(L"MessageBox", NULL, NULL, NULL);
	ZuiControlRegNotify(MsgBox_pRoot, MsgBox_Notify);
    //取消最小化按钮
    p = ZuiControlFindName(MsgBox_pRoot, L"WindowCtl_min");
    ZuiControlCall(Proc_SetVisible, p, FALSE, NULL, NULL);
    //取消最大化按钮
    p = ZuiControlFindName(MsgBox_pRoot, L"WindowCtl_max");
    ZuiControlCall(Proc_SetVisible, p, FALSE, NULL, NULL);
    //挂接关闭按钮事件
    p = ZuiControlFindName(MsgBox_pRoot, L"WindowCtl_clos");
    ZuiControlRegNotify(p, MsgBox_Notify_ctl);
    //挂接确认按钮事件
    p = ZuiControlFindName(MsgBox_pRoot, L"ok");
    ZuiControlRegNotify(p, MsgBox_Notify_ctl);
    //挂接取消按钮事件
    p = ZuiControlFindName(MsgBox_pRoot, L"cancel");
    ZuiControlRegNotify(p, MsgBox_Notify_ctl);

    p = ZuiControlFindName(MsgBox_pRoot, L"text");
    ZuiControlCall(Proc_SetText, p, text, NULL, NULL);
    p = ZuiControlFindName(MsgBox_pRoot, L"title");
    ZuiControlCall(Proc_SetText, p, title, NULL, NULL);
	ZuiInt nRet;
        //禁用掉父窗口
        EnableWindow(rp->m_pOs->m_hWnd, FALSE);
        MSG Msg;
        while (1)
        {
            GetMessage(&Msg, NULL, 0, 0);
			if (Msg.message == WM_CLOSE)
			{
				nRet = Msg.wParam;
			}
			if (Msg.hwnd == MsgBox_pRoot->m_pOs->m_hWnd || Msg.message == WM_PAINT) {
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}
			if (Msg.message == WM_QUIT) {
				break;
			}
        }
        //重新开启父窗口
        EnableWindow(rp->m_pOs->m_hWnd, TRUE);
		SetFocus(rp->m_pOs->m_hWnd);
    return nRet;
}
ZuiBool ZuiIsPointInRect(ZuiRect Rect, ZuiPoint pt) {
    int xl, xr, yt, yb;

    if (Rect->left < Rect->right)
    {
        xl = Rect->left;
        xr = Rect->right;
    }
    else
    {
        xl = Rect->right;
        xr = Rect->left;
    }

    if (Rect->top < Rect->bottom)
    {
        yt = Rect->bottom;
        yb = Rect->top;
    }
    else
    {
        yt = Rect->top;
        yb = Rect->bottom;
    }

    return ((pt->x >= xl && pt->x <= xr) && (pt->y >= yb && pt->y <= yt));
}




ZuiBool ZuiStingIsUtf8(ZuiAny str, ZuiInt length)
{
    int i;
    //UFT8可用1-6个字节编码,ASCII用一个字节
    int nBytes = 0;
    unsigned char chr;
    //如果全部都是ASCII, 说明不是UTF-8
    ZuiBool bAllAscii = TRUE;
    for (i = 0; i < length; i++)
    {
        chr = *((char *)str + i);
        // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
        if ((chr & 0x80) != 0)
        {
            bAllAscii = FALSE;
        }
        //如果不是ASCII码,应该是多字节符,计算字节数
        if (nBytes == 0)
        {
            if (chr >= 0x80)
            {
                if (chr >= 0xFC && chr <= 0xFD)
                {
                    nBytes = 6;
                }
                else if (chr >= 0xF8)
                {
                    nBytes = 5;
                }
                else if (chr >= 0xF0)
                {
                    nBytes = 4;
                }
                else if (chr >= 0xE0)
                {
                    nBytes = 3;
                }
                else if (chr >= 0xC0)
                {
                    nBytes = 2;
                }
                else
                {
                    return FALSE;
                }
                nBytes--;
            }
        }
        //多字节符的非首字节,应为 10xxxxxx
        else
        {
            if ((chr & 0xC0) != 0x80)
            {
                return FALSE;
            }
            nBytes--;
        }
    }
    //违返规则
    if (nBytes > 0)
    {
        return FALSE;
    }
    //如果全部都是ASCII, 也是字符串
    if (bAllAscii)
    {
        return TRUE;
    }
    return TRUE;
}

ZuiVoid ZuiStingSplitA(char* src, char* pSeparator, char **dest, ZuiInt *num)
{
    char* pStart, *pEnd;
    size_t sep_len;
    int count = 0;
    if (src == NULL || strlen(src) == 0) return;
    sep_len = strlen(pSeparator);
    pStart = src;
    while (1)
    {
        pEnd = strstr(pStart, pSeparator);
        if (pEnd != NULL)
        {
            memset(pEnd, '\0', sep_len);
            *dest++ = pStart;
            pEnd = pEnd + sep_len;
            pStart = pEnd;
            ++count;
        }
        else
        {
            *dest = pStart;
            ++count;
            break;
        }
    }
    *num = count;
}

ZuiVoid ZuiStingSplit(ZuiText src, ZuiText pSeparator, ZuiText *dest, ZuiInt *num)
{
    ZuiText pStart, pEnd;
    size_t sep_len;
    int count = 0;
    if (src == NULL || wcslen(src) == 0) return;
    sep_len = wcslen(pSeparator);
    pStart = src;
    while (1)
    {
        pEnd = wcsstr(pStart, pSeparator);
        if (pEnd != NULL)
        {
            memset(pEnd, '\0', sep_len * sizeof(_ZuiText));
            *dest++ = pStart;
            pEnd = pEnd + sep_len;
            pStart = pEnd;
            ++count;
        }
        else
        {
            *dest = pStart;
            ++count;
            break;
        }
    }
    *num = count;
}

ZuiInt ZuiUtf8ToUnicode(ZuiAny str, ZuiInt slen, ZuiText out, ZuiInt olen)
{
    return ZuiOsUtf8ToUnicode(str, slen, out, olen);
}

ZuiInt ZuiAsciiToUnicode(ZuiAny str, ZuiInt slen, ZuiText out, ZuiInt olen)
{
    return ZuiOsAsciiToUnicode(str, slen, out, olen);
}

ZuiInt ZuiUnicodeToAscii(ZuiText str, ZuiInt slen, ZuiAny out, ZuiInt olen)
{
    return ZuiOsUnicodeToAscii(str, slen, out, olen);
}

ZuiColor ZuiStr2Color(ZuiAny str)
{
	ZuiText pstr = NULL;
	ZuiColor clrColor;
	while (*(ZuiText)str > L'\0' && *(ZuiText)str <= L' ')
		str = ZuiCharNext((ZuiText)str);
	if (*(ZuiText)str == L'#')
		str = ZuiCharNext((ZuiText)str);
	clrColor = _tcstoul((ZuiText)str, &pstr, 16);
	return clrColor|0xFF000000;
}
