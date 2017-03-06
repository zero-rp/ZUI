#include <stdio.h>
#include <ZUI.h>
#include "CodeEdit.h"
ZuiControl Designer;

int _stdcall _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    ZInitConfig config = { 0 };
    config.debug = TRUE;
    config.m_hInstance = GetModuleHandle(NULL);
    ZuiInit(&config);
	ZuiControlRegisterAdd(L"codeedit", (ZCtlProc)&ZuiCodeEditProc);

	//加载系统资源包
	ZuiResDBGetRes(L"pe_zui:zip:105", ZREST_ZIP);

	Designer = ZuiWindowFindName(L"window_main");


	ZuiMsgLoop();
}
int main() {
	_tWinMain(0, 0, 0, 0);

}