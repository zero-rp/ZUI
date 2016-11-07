#include <stdio.h>
#include <ZUI.h>
#if !(defined NDEBUG)

//ZuiControl win,other;
//ZuiControl BrowserTab;
//ZuiControl BrowserTabHead;
//ZuiControl AddBrowserTab(ZuiText url);
//
////重载HorizontalLayout实现定制化效果---------------------------------------------
//ZEXPORT ZuiAny ZCALL ZuiHorizontalLayoutProcEx(ZuiInt ProcId, ZuiControl cp, ZuiOption p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
//	switch (ProcId)
//	{
//	case Proc_OnPaint: {
//		ZuiGraphics gp = (ZuiGraphics)Param1;
//		RECT *rc = &cp->m_rcItem;
//		ZuiDrawLine(gp, ARGB(255, 50, 255, 0), rc->left, rc->bottom-1, rc->right, rc->bottom-1, 1);
//		break;
//	}
//	default:
//		break;
//	}
//	return ZuiHorizontalLayoutProc(ProcId, cp, p, Param1, Param2, Param3);
//}
////重载option实现定制化效果---------------------------------------------
//ZEXPORT ZuiAny ZCALL ZuiOptionProcEx(ZuiInt ProcId, ZuiControl cp, ZuiOption p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
//	switch (ProcId)
//	{
//	case Proc_OnPaint: {
//		ZuiGraphics gp = (ZuiGraphics)Param1;
//		RECT *rc = &cp->m_rcItem;
//		if (p->m_bSelected) {//选中
//			ZuiDrawFillRect(gp, ARGB(255, 242, 242, 242), rc->left + 1, rc->top, rc->right - rc->left - 1, rc->bottom - rc->top);
//			 //画激活边线
//			ZuiDrawLine(gp, ARGB(255, 50, 255, 0), rc->left, rc->top, rc->left, rc->bottom - 1, 1);
//			ZuiDrawLine(gp, ARGB(255, 50, 255, 0), rc->right-1, rc->top, rc->right-1, rc->bottom - 1, 1);
//
//		}
//		else {//非选中项
//			if (((ZuiButton)p->old_udata)->type == 0) {
//				ZuiDrawFillRect(gp, ARGB(255, 204, 204, 204), rc->left, rc->top + 3, rc->right - rc->left, rc->bottom - rc->top - 4);
//			}
//			else if (((ZuiButton)p->old_udata)->type == 1) {
//				ZuiDrawFillRect(gp, ARGB(255, 0, 255, 255), rc->left, rc->top + 3, rc->right - rc->left, rc->bottom - rc->top - 4);
//			}
//			else if (((ZuiButton)p->old_udata)->type == 2) {
//				ZuiDrawFillRect(gp, ARGB(255, 255, 255, 255), rc->left, rc->top + 3, rc->right - rc->left, rc->bottom - rc->top - 4);
//			}
//
//			int index = ZuiControlCall(Proc_Layout_GetItemIndex, cp->m_pParent, cp, NULL, NULL);
//			ZuiControl pControl = ZuiControlCall(Proc_Layout_GetItemAt, cp->m_pParent, index + 1, NULL, NULL);
//			if(!pControl)
//				ZuiDrawLine(gp, ARGB(255, 50, 255, 0), rc->right - 1, rc->top + 3, rc->right - 1, rc->bottom - 1, 1);//右边
//			pControl = ZuiControlCall(Proc_Layout_GetItemAt, cp->m_pParent, index - 1, NULL, NULL);
//			if(!ZuiControlCall(Proc_Option_GetSelected, pControl, NULL, NULL, NULL) || !index)
//				ZuiDrawLine(gp, ARGB(255, 50, 255, 0), rc->left, rc->top + 3, rc->left, rc->bottom, 1);//左边
//
//
//			  
//			ZuiDrawLine(gp, ARGB(255, 50, 255, 0), rc->left, rc->top+3, rc->right, rc->top + 3, 1);
//		}
//		//画文本
//		ZRect r;
//		MAKEZRECT(r, rc->left + 5, rc->top + 5, rc->right - rc->left - 10, rc->bottom - rc->top - 10);
//		ZuiDrawString(gp, Global_StringFormat, cp->m_sText, &r);
//		return 0;//重载全部默认绘制
//		break;
//	}
//	case Proc_Option_SetSelected: {
//		if (!Param1)
//		{
//			ZuiBool noret=FALSE;
//			for (size_t i = 0; i < ZuiControlCall(Proc_Layout_GetCount, cp->m_pParent, NULL, NULL, NULL); i++)
//			{
//				ZuiControl pControl;
//				if ((pControl = ZuiControlCall(Proc_Layout_GetItemAt, cp->m_pParent, i, NULL, NULL)) != cp)
//				{
//					if (ZuiControlCall(Proc_Option_GetSelected, pControl, NULL, NULL, NULL))
//					{
//						noret = TRUE;
//					}
//				}
//
//			}
//			if (!noret)
//				return;
//		}
//	}
//	default:
//		break;
//	}
//	return ZuiOptionProc(ProcId, cp, p, Param1, Param2, Param3);
//}
////---------------------------------------------
//
//ZuiAny ZCALL Notify_browser(ZuiText msg, ZuiControl cp, ZuiBrowser p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
//	if (wcscmp(msg, L"newwindow") == 0) {
//		return AddBrowserTab(Param1);
//	}
//	else if (wcscmp(msg, L"titlechanged") == 0) {
//		int index = ZuiControlCall(Proc_Layout_GetItemIndex, BrowserTab, p->cp, NULL, NULL);
//		ZuiControl phead = ZuiControlCall(Proc_Layout_GetItemAt, BrowserTabHead, index, NULL, NULL);
//		ZuiControlCall(Proc_SetText, phead, Param1, NULL, NULL);
//	}
//
//	return 0;
//}
//ZuiAny ZCALL Notify_tabhead(ZuiText msg, ZuiControl cp, ZuiOption p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
//	if (wcscmp(msg, L"selectchanged") == 0 && Param1) {
//		int index = ZuiControlCall(Proc_Layout_GetItemIndex, BrowserTabHead, cp, NULL, NULL);
//		ZuiControlCall(Proc_Layout_Tab_SelectItem, BrowserTab, index, NULL, NULL);
//	}
//	return 0;
//}
//ZuiControl AddBrowserTab(ZuiText url) {
//	ZuiControl TabHead = NewZuiControl(L"Option", NULL, NULL, NULL);
//	ZuiControlCall(Proc_SetMinWidth, TabHead, 60, NULL, NULL);
//	ZuiControlCall(Proc_SetMaxWidth, TabHead, 130, NULL, NULL);
//	ZuiControlCall(Proc_Option_SetGroup, TabHead, TRUE, NULL, NULL);
//	ZuiControlRegNotify(TabHead, Notify_tabhead);
//	TabHead->call = ZuiOptionProcEx;
//	ZuiControlCall(Proc_Layout_Add, BrowserTabHead, TabHead, NULL, NULL);
//	ZuiControlCall(Proc_Option_SetSelected, TabHead, TRUE, NULL, NULL);
//
//	ZuiControl Browser = NewZuiControl(L"Browser", NULL, NULL, NULL);
//	ZuiControlCall(Proc_Browser_LoadUrl, Browser, url, NULL, NULL);
//	ZuiControlCall(Proc_Browser_GetView, Browser, NULL, NULL, NULL);
//	ZuiControlRegNotify(Browser, Notify_browser);
//	
//	ZuiControlCall(Proc_Layout_Add, BrowserTab, Browser, NULL, NULL);
//	ZuiControlCall(Proc_Layout_Tab_SelectItem, BrowserTab, ZuiControlCall(Proc_Layout_GetItemIndex, BrowserTab, Browser, NULL, NULL), NULL, NULL);
//
//	return Browser;
//}

 __declspec(dllexport) void __stdcall DLLDebug(){
	ZuiInit();
	

	HRESULT Hr = CoInitialize(NULL);
	if (FAILED(Hr)) return;

	FILE*fp;
	long flen;
	void *p;

	fp = fopen("test.xml", "rb");
	fseek(fp, 0L, SEEK_END);
	flen = ftell(fp); /* 得到文件大小 */
	p = malloc(flen); /* 根据文件大小动态分配内存空间 */
	fseek(fp, 0L, SEEK_SET); /* 定位到文件开头 */
	fread(p, flen, 1, fp); /* 一次性读取全部文件内容 */
	fclose(fp);
	
	
	ZuiLayoutLoad(p,flen);
	
	ZuiMsgLoop();

	CoUninitialize();

	return;
}
int _stdcall _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow){
	DLLDebug();
}
int  main(){
	DLLDebug();
}

#endif

#if (defined NDEBUG)
int __security_cookie = 0;     //比错误提示的名称少一个下划线

__declspec(naked) __int64 _ftol2_sse(double v)
{
	__asm {
		push        ebp
			mov         ebp, esp
			sub         esp, 20h
			and         esp, 0FFFFFFF0h
			fld         st(0)
			fst         dword ptr[esp + 18h]
			fistp       qword ptr[esp + 10h]
			fild        qword ptr[esp + 10h]
			mov         edx, dword ptr[esp + 18h]
			mov         eax, dword ptr[esp + 10h]
			test        eax, eax
			je          integer_QnaN_or_zero
			arg_is_not_integer_QnaN :
		fsubp       st(1), st
			test        edx, edx
			jns         positive
			fstp        dword ptr[esp]
			mov         ecx, dword ptr[esp]
			xor ecx, 80000000h
			add         ecx, 7FFFFFFFh
			adc         eax, 0
			mov         edx, dword ptr[esp + 14h]
			adc         edx, 0
			jmp         localexit

			positive :
		fstp        dword ptr[esp]
			mov         ecx, dword ptr[esp]
			add         ecx, 7FFFFFFFh
			sbb         eax, 0
			mov         edx, dword ptr[esp + 14h]
			sbb         edx, 0
			jmp         localexit
			integer_QnaN_or_zero :
		mov         edx, dword ptr[esp + 14h]
			test        edx, 7FFFFFFFh
			jne         arg_is_not_integer_QnaN
			fstp        dword ptr[esp + 18h]
			fstp        dword ptr[esp + 18h]
			localexit :
			leave
			ret
	}
}
#endif
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}