#include <stdio.h>
#include <ZUI.h>

ZuiControl Work;

int _stdcall _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    ZInitConfig config = { 0 };
    config.debug = TRUE;
    config.m_hInstance = GetModuleHandle(NULL);
    ZuiInit(&config);
    

    //登录帐号

    //加载系统资源包
    ZuiResDBGetRes(L"pe_zui:zip:105", ZREST_ZIP);

    Work = ZuiWindowFindName(L"window_main");

    ZuiMsgLoop();
}