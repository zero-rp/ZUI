#include <stdio.h>
#include <windows.h>
#include <ZUI.h>

#if (defined NDEBUG)
int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, ZuiText lpCmdLine, int nCmdShow) {
    ZInitConfig config;
    config.m_hInstance = hInstance;
    config.default_res = L"file:default.zip";
    ZuiInit(&config);

    ZuiRes res = ZuiResDBGetRes(L"file:test.xml", ZREST_STREAM);
    ZuiInt len = 0;
    ZuiAny xml = ZuiResGetData(res, &len);
    ZuiControl c = ZuiControlFindName(ZuiLayoutLoad(xml, len), L"list");
    ZuiResDBDelRes(res);

    ZuiResDBGetRes(L"file:res.zip", ZREST_ZIP);
    
    //ZuiControl win = NewZuiControl(L"Window", 0, 0, 0);
    //ZuiControlCall(Proc_Window_SetSize, win, 500, 500, NULL);
    ////ZuiControlCall(Proc_Window_SetNoBox, win, TRUE, NULL, NULL);
    //ZuiControlCall(Proc_SetBkColor, win, 0, NULL, NULL);

    //ZuiControl bt1 = NewZuiControl(L"Button", 0, 0, 0);
    //ZuiControlCall(Proc_SetFixedHeight, bt1, 50, NULL, NULL);
    //ZuiLayoutAdd(win, bt1);

    //ZuiControl null1 = NewZuiControl(L"Button", 0, 0, 0);
    //ZuiControlCall(Proc_SetFixedHeight, null1, 50, NULL, NULL);
    //ZuiLayoutAdd(win, null1);

    //ZuiControl bt2 = NewZuiControl(L"Button", 0, 0, 0);
    //ZuiControlSetFixedHeight(bt2, 50);
    //ZuiLayoutAdd(win, bt2);


    ////ZuiControl List = NewZuiControl(L"List", 0, 0, 0);
    ////ZuiLayoutAdd(win, List);

    //ZuiControlSetDrag(null1, TRUE);

    ZuiMsgLoop();
    return 1;
}
main() {
    WinMain(GetModuleHandle(NULL), 0, 0, 0);
}
#endif

