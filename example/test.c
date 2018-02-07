#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <ZUI.h>

ZuiAny ZCALL Main_Notify_ctl_clos(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
ZuiAny ZCALL Main_Notify_ctl_max(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
ZuiAny ZCALL Main_Notify(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
ZuiAny ZCALL Main_Notify_ctl_min(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);
ZuiAny ZCALL msgbox_Notify(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);

ZuiControl win;

int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	TCHAR tmp[1024];
	TCHAR *ps;
	GetModuleFileName(NULL, tmp, 1024);
	ps = _tcsrchr(tmp, _T('\\'));
	*++ps = 0;
	SetCurrentDirectory(tmp);

	ZInitConfig config;
	config.m_hInstance = hInstance;
	config.default_res = L"file:default.zip";
	if (!ZuiInit(&config)) return 0;

	ZuiRes	res = ZuiResDBGetRes(L"default:default_msgbox.xml", ZREST_STREAM);
	ZuiInt	len = 0;
	ZuiAny	xml = ZuiResGetData(res, &len);
	ZuiLayoutLoad(xml, len);


	res = ZuiResDBGetRes(L"file:test.xml", ZREST_STREAM);
	len = 0;
	xml = ZuiResGetData(res, &len);
	win = ZuiLayoutLoad(xml, len);
	if (!win) return 0;
	ZuiControlRegNotify(win, Main_Notify);

	ZuiControl p = ZuiControlFindName(win, L"WindowCtl_clos");
	ZuiControlRegNotify(p, Main_Notify_ctl_clos);

	p = ZuiControlFindName(win, L"WindowCtl_max");
	ZuiControlRegNotify(p, Main_Notify_ctl_max);

	p = ZuiControlFindName(win, L"WindowCtl_min");
	ZuiControlRegNotify(p, Main_Notify_ctl_min);

	p = ZuiControlFindName(win, L"msgbox");
	ZuiControlRegNotify(p, msgbox_Notify);

	ZuiMsgLoop();
	printf("Loop done!!\n");
	Sleep(2000);
	ZuiUnInit();
	return 0;
}
int main() {
	WinMain(GetModuleHandle(NULL), 0, 0, 0);
}

ZuiAny ZCALL Main_Notify_ctl_clos(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	if (wcscmp(msg, L"onclick") == 0)
	{
		ZuiControlCall(Proc_OnClose, win, 0, 0, 0);
	}
	return 0;
}

ZuiAny ZCALL Main_Notify_ctl_min(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	if (wcscmp(msg, L"onclick") == 0)
	{
		ZuiControlCall(Proc_Window_SetWindowMin, win, 0, 0, 0);
	}
	return 0;
}

ZuiAny ZCALL Main_Notify_ctl_max(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	if (wcscmp(msg, L"selectchanged") == 0)
	{

		if (ZuiControlCall(Proc_Option_GetSelected, p, 0, 0, 0))
			ZuiControlCall(Proc_Window_SetWindowMax, win, 0, 0, 0);
		else
			ZuiControlCall(Proc_Window_SetWindowRestor, win, 0, 0, 0);

	}
	return 0;
}
ZuiAny ZCALL Main_Notify(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	if (wcscmp(msg, L"onclose") == 0)
	{
		ZuiInt ret = ZuiMsgBox(win, L"是否退出程序？", L"提示!!");
		if (ret == ZuiOK) {
            FreeZuiControl(win, 1);
		}
	}
	else if (wcscmp(msg, L"ondestroy") == 0)
	{
		ZuiMsgLoop_exit(0);
	}
	return 0;
}
ZuiAny ZCALL msgbox_Notify(ZuiText msg, ZuiControl p, ZuiAny UserData, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	if (wcscmp(msg, L"onclick") == 0)
	{
		ZuiMsgBox(win, L"HELLO WORLD!!", L"Hello World!!");
	}
	return 0;
}
