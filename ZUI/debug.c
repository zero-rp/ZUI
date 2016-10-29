 #include <stdio.h>
#include <ZUI.h>
ZuiControl win;
ZuiControl BrowserTab;
ZuiControl BrowserTabHead;
ZuiControl AddBrowserTab(ZuiText url);

//重载HorizontalLayout实现定制化效果---------------------------------------------
ZEXPORT ZuiAny ZCALL ZuiHorizontalLayoutProcEx(ZuiInt ProcId, ZuiControl cp, ZuiOption p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	switch (ProcId)
	{
	case Proc_OnPaint: {
		ZuiGraphics gp = (ZuiGraphics)Param1;
		RECT *rc = &cp->m_rcItem;
		ZuiDrawLine(gp, ARGB(255, 50, 255, 0), rc->left, rc->bottom-1, rc->right, rc->bottom-1, 1);
		break;
	}
	default:
		break;
	}
	return ZuiHorizontalLayoutProc(ProcId, cp, p, Param1, Param2, Param3);
}
//重载option实现定制化效果---------------------------------------------
ZEXPORT ZuiAny ZCALL ZuiOptionProcEx(ZuiInt ProcId, ZuiControl cp, ZuiOption p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	switch (ProcId)
	{
	case Proc_OnPaint: {
		ZuiGraphics gp = (ZuiGraphics)Param1;
		RECT *rc = &cp->m_rcItem;
		if (p->m_bSelected) {//选中
			ZuiDrawFillRect(gp, ARGB(255, 255, 255, 255), rc->left + 1, rc->top, rc->right - rc->left - 1, rc->bottom - rc->top);
			 //画激活边线
			ZuiDrawLine(gp, ARGB(255, 50, 255, 0), rc->left, rc->top, rc->left, rc->bottom - 1, 1);
			ZuiDrawLine(gp, ARGB(255, 50, 255, 0), rc->right-1, rc->top, rc->right-1, rc->bottom - 1, 1);

		}
		else {//非选中项
			if (((ZuiButton)p->old_udata)->type == 0) {
				ZuiDrawFillRect(gp, ARGB(255, 255, 255, 255), rc->left, rc->top + 3, rc->right - rc->left, rc->bottom - rc->top - 4);
			}
			else if (((ZuiButton)p->old_udata)->type == 1) {
				ZuiDrawFillRect(gp, ARGB(255, 0, 255, 255), rc->left, rc->top + 3, rc->right - rc->left, rc->bottom - rc->top - 4);
			}
			else if (((ZuiButton)p->old_udata)->type == 2) {
				ZuiDrawFillRect(gp, ARGB(255, 255, 255, 255), rc->left, rc->top + 3, rc->right - rc->left, rc->bottom - rc->top - 4);
			}

			int index = ZuiControlCall(Proc_Layout_GetItemIndex, cp->m_pParent, cp, NULL, NULL);
			ZuiControl pControl = ZuiControlCall(Proc_Layout_GetItemAt, cp->m_pParent, index + 1, NULL, NULL);
			if(!pControl)
				ZuiDrawLine(gp, ARGB(255, 50, 255, 0), rc->right - 1, rc->top + 3, rc->right - 1, rc->bottom - 1, 1);//右边
			pControl = ZuiControlCall(Proc_Layout_GetItemAt, cp->m_pParent, index - 1, NULL, NULL);
			if(!ZuiControlCall(Proc_Option_GetSelected, pControl, NULL, NULL, NULL) || !index)
				ZuiDrawLine(gp, ARGB(255, 50, 255, 0), rc->left, rc->top + 3, rc->left, rc->bottom, 1);//左边


			  
			ZuiDrawLine(gp, ARGB(255, 50, 255, 0), rc->left, rc->top+3, rc->right, rc->top + 3, 1);
		}
		//画文本
		ZRect r;
		MAKEZRECT(r, rc->left + 5, rc->top + 5, rc->right - rc->left - 10, rc->bottom - rc->top - 10);
		ZuiDrawString(gp, Global_StringFormat, cp->m_sText, &r);
		return 0;//重载全部默认绘制
		break;
	}
	case Proc_Option_SetSelected: {
		if (!Param1)
		{
			ZuiBool noret=FALSE;
			for (size_t i = 0; i < ZuiControlCall(Proc_Layout_GetCount, cp->m_pParent, NULL, NULL, NULL); i++)
			{
				ZuiControl pControl;
				if ((pControl = ZuiControlCall(Proc_Layout_GetItemAt, cp->m_pParent, i, NULL, NULL)) != cp)
				{
					if (ZuiControlCall(Proc_Option_GetSelected, pControl, NULL, NULL, NULL))
					{
						noret = TRUE;
					}
				}

			}
			if (!noret)
				return;
		}
	}
	default:
		break;
	}
	return ZuiOptionProc(ProcId, cp, p, Param1, Param2, Param3);
}
//---------------------------------------------

ZuiAny ZCALL Notify_ctl(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	if (wcscmp(p->m_sName, L"clos") == 0) {
		ZuiMsgLoop_exit();
		exit(0);
	}else if (wcscmp(p->m_sName, L"min") == 0) {
		ZuiControlCall(Proc_Window_SetWindowMin, win, NULL, NULL, NULL);
	}else if (wcscmp(p->m_sName, L"max") == 0) {
		if (ZuiControlCall(Proc_Option_GetSelected, p, NULL, NULL, NULL))
			ZuiControlCall(Proc_Window_SetWindowMax, win, NULL, NULL, NULL);
		else 
			ZuiControlCall(Proc_Window_SetWindowRestor, win, NULL, NULL, NULL);
	}
#if !(defined NDEBUG)
	printf("%ls,%ls\r\n", p->m_sName, msg);
#endif
	return 0;
}
ZuiAny ZCALL Notify_browser(ZuiText msg, ZuiControl cp, ZuiBrowser p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	if (wcscmp(msg, L"newwindow") == 0) {
		return AddBrowserTab(Param1);
	}
	else if (wcscmp(msg, L"titlechanged") == 0) {
		int index = ZuiControlCall(Proc_Layout_GetItemIndex, BrowserTab, p->cp, NULL, NULL);
		ZuiControl phead = ZuiControlCall(Proc_Layout_GetItemAt, BrowserTabHead, index, NULL, NULL);
		ZuiControlCall(Proc_SetText, phead, Param1, NULL, NULL);
	}

	return 0;
}
ZuiAny ZCALL Notify_tabhead(ZuiText msg, ZuiControl cp, ZuiOption p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	if (wcscmp(msg, L"selectchanged") == 0 && Param1) {
		int index = ZuiControlCall(Proc_Layout_GetItemIndex, BrowserTabHead, cp, NULL, NULL);
		ZuiControlCall(Proc_Layout_Tab_SelectItem, BrowserTab, index, NULL, NULL);
	}
	return 0;
}
ZuiControl AddBrowserTab(ZuiText url) {
	ZuiControl TabHead = NewZuiControl(L"Option", NULL, NULL, NULL);
	ZuiControlCall(Proc_SetMinWidth, TabHead, 60, NULL, NULL);
	ZuiControlCall(Proc_SetMaxWidth, TabHead, 130, NULL, NULL);
	ZuiControlCall(Proc_Option_SetGroup, TabHead, TRUE, NULL, NULL);
	ZuiControlRegNotify(TabHead, Notify_tabhead);
	TabHead->call = ZuiOptionProcEx;
	ZuiControlCall(Proc_Layout_Add, BrowserTabHead, TabHead, NULL, NULL);
	ZuiControlCall(Proc_Option_SetSelected, TabHead, TRUE, NULL, NULL);

	ZuiControl Browser = NewZuiControl(L"Browser", NULL, NULL, NULL);
	ZuiControlCall(Proc_Browser_LoadUrl, Browser, url, NULL, NULL);
	ZuiControlCall(Proc_Browser_GetView, Browser, NULL, NULL, NULL);
	ZuiControlRegNotify(Browser, Notify_browser);
	
	ZuiControlCall(Proc_Layout_Add, BrowserTab, Browser, NULL, NULL);
	ZuiControlCall(Proc_Layout_Tab_SelectItem, BrowserTab, ZuiControlCall(Proc_Layout_GetItemIndex, BrowserTab, Browser, NULL, NULL), NULL, NULL);

	return Browser;
}


 __declspec(dllexport) void __stdcall DLLDebug(){

	ZuiInit();
	

	HRESULT Hr = CoInitialize(NULL);
	if (FAILED(Hr)) return;

	FILE*fp;
	long flen;
	void *p;

	ZuiResDBCreateFromFile(L"res/res.zip", NULL);

	fp = fopen("test.xml", "rb");
	fseek(fp, 0L, SEEK_END);
	flen = ftell(fp); /* 得到文件大小 */
	p = malloc(flen); /* 根据文件大小动态分配内存空间 */
	fseek(fp, 0L, SEEK_SET); /* 定位到文件开头 */
	fread(p, flen, 1, fp); /* 一次性读取全部文件内容 */
	fclose(fp);
	
	
	win = ZuiLayoutLoad(p,flen);

	ZuiControl clos = ZuiControlFindName(win, L"clos");
	ZuiControlRegNotify(clos, Notify_ctl);
	ZuiControl min = ZuiControlFindName(win, L"min");
	ZuiControlRegNotify(min, Notify_ctl);
	ZuiControl max = ZuiControlFindName(win, L"max");
	ZuiControlRegNotify(max, Notify_ctl);

	BrowserTab = ZuiControlFindName(win, L"BrowserTab");
	BrowserTabHead = ZuiControlFindName(win, L"BrowserTabHead");
	ZuiControlFindName(win, L"head")->call = ZuiHorizontalLayoutProcEx;
	//添加默认页面
	AddBrowserTab(L"http://www.baidu.com/");
	AddBrowserTab(L"http://www.baidu.com/");
	AddBrowserTab(L"http://www.baidu.com/");
	AddBrowserTab(L"http://www.baidu.com/");

	//ZuiControl pRoot = NewZuiControl(L"window", NULL, NULL, NULL);
	////-------------------------------------------------------------------------
	//ZuiControl tabhead = NewZuiControl(L"HorizontalLayout", NULL, NULL, NULL);
	//ZuiControlCall(Proc_SetFixedHeight, tabhead, 32, 0, 0);
	////-------------------------------------------------------------------------
	//ZuiControl head = NewZuiControl(L"HorizontalLayout", NULL, NULL, NULL);
	//ZuiControlCall(Proc_SetFixedHeight, head, 40, 0, 0);

	//

	//ZuiControl back = NewZuiControl(L"button", NULL, NULL, NULL);
	//ZuiControl next = NewZuiControl(L"button", NULL, NULL, NULL);
	//ZuiControl reload = NewZuiControl(L"button", NULL, NULL, NULL);
	//ZuiControlCall(Proc_SetFixedWidth, back, 40, 0, 0);
	//ZuiControlCall(Proc_SetFixedWidth, next, 40, 0, 0);
	//ZuiControlCall(Proc_SetFixedWidth, reload, 40, 0, 0);

	//ZuiControlCall(Proc_Layout_Add, head, back, NULL, NULL);
	//ZuiControlCall(Proc_Layout_Add, head, next, NULL, NULL);
	//ZuiControlCall(Proc_Layout_Add, head, reload, NULL, NULL);

	////-------------------------------------------------------------------------
	//ZuiControl tab = NewZuiControl(L"TabLayout", NULL, NULL, NULL);
	//ZuiControl defbr = NewZuiControl(L"Browser", NULL, NULL, NULL);

	//ZuiControlCall(Proc_Layout_Add, tab, defbr, NULL, NULL);


	//ZuiControlCall(Proc_Layout_Add, pRoot, tabhead, NULL, NULL);
	//ZuiControlCall(Proc_Layout_Add, pRoot, head, NULL, NULL);
	//ZuiControlCall(Proc_Layout_Add, pRoot, tab, NULL, NULL);

	ZuiMsgLoop();

	//
	////ZuiControlCall(Proc_SetFixedWidth, playout1, (void *)100, 0, 0);

	//ZuiControl pBut1 = NewCControlUI("button", NULL, NULL, NULL);
	//ZuiControlCall(Proc_SetText, pBut1, L"扩展按钮1", NULL, NULL);
	//pBut1->m_sToolTip = L"按钮1提示";
	//ZuiControlCall(Proc_SetMaxWidth, pBut1, (void *)200, 0, 0);
	//ZuiControl pBut2 = NewCControlUI("button", NULL, NULL, NULL);
	//ZuiControlCall(Proc_SetText, pBut2, L"扩展按钮2", NULL, NULL);
	//pBut2->m_sToolTip = L"按钮2提示";

	//ZuiControl pBut3 = NewCControlUI("button", NULL, NULL, NULL);
	//ZuiControlCall(Proc_SetText, pBut3, L"扩展按钮3", NULL, NULL);
	//pBut3->m_sToolTip = L"按钮3提示";


	//ZuiControl pBut11 = NewCControlUI("button", NULL, NULL, NULL);
	//ZuiControlCall(Proc_SetText, pBut11, L"扩展按钮11", NULL, NULL);
	//pBut11->m_sToolTip = L"按钮11提示";
	//ZuiControlCall(Proc_SetFixedWidth, pBut11, (void *)50, 0, 0);
	//ZuiControlCall(Proc_SetFixedHeight, pBut11, (void *)50, 0, 0);
	//ZuiControl pBut12 = NewCControlUI("button", NULL, NULL, NULL);
	//ZuiControlCall(Proc_SetText, pBut12, L"扩展按钮12", NULL, NULL);
	//pBut12->m_sToolTip = L"按钮12提示";
	//ZuiControl pBut13 = NewCControlUI("button", NULL, NULL, NULL);
	//ZuiControlCall(Proc_SetText, pBut13, L"扩展按钮13", NULL, NULL);
	//pBut13->m_sToolTip = L"按钮13提示";

	//ZuiControl pBut14 = NewCControlUI("button", NULL, NULL, NULL);
	//ZuiControlCall(Proc_SetText, pBut14, L"扩展按钮14", NULL, NULL);
	//pBut14->m_sToolTip = L"按钮14提示";
	////pBut14->m_bFloat = TRUE;
	//ZuiControlCall(Proc_SetFixedWidth, pBut14, (void *)500, 0, 0);
	//ZuiControlCall(Proc_SetFixedHeight, pBut14, (void *)50, 0, 0);
	//ZuiControlCall(Proc_SetFixedXY, pBut14, (void *)25, (void *)50, 0);

	//ZuiControlCall(Proc_Layout_Add, pRoot, pBut1, NULL, NULL);
	//ZuiControlCall(Proc_Layout_Add, playout1, pBut11, NULL, NULL);
	//ZuiControlCall(Proc_Layout_Add, playout1, pBut12, NULL, NULL);
	//ZuiControlCall(Proc_Layout_Add, playout1, pBut13, NULL, NULL);
	//ZuiControlCall(Proc_Layout_Add, playout1, pBut14, NULL, NULL);
	//ZuiControlCall(Proc_Layout_Add, pRoot, playout1, NULL, NULL);

	//ZuiControlCall(Proc_Layout_Add, pRoot, pBut2, NULL, NULL);
	//ZuiControlCall(Proc_Layout_Add, pRoot, pBut3, NULL, NULL);
	
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
#if (defined NDEBUG)
int __security_cookie = 0;     //比错误提示的名称少一个下划线
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