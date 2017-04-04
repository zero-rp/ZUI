#include <stdio.h>
#include <ZUI.h>
#if !(defined NDEBUG)
int _stdcall _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    ZInitConfig config = { 0 };
    //config.debug = TRUE;
    config.m_hInstance = hInstance;
    ZuiInit(&config);
	
#if 0
    ZuiControl win = NewZuiControl(L"Window", 0, 0, 0);
    ZProc(win, Window_SetSize, 500, 500);
    ZProc(win, Window_SetNoBox, TRUE);
    ZProc(win, Window_SetNoBox, TRUE);
    ZuiControlSetBkColor(win, -1);

    //ZuiControl html = NewZuiControl(L"Html", 0, 0, 0);
    //ZuiControlSetBkColor(html, -1);
    //ZuiControlSetDrag(html, TRUE);
    //ZuiLayoutAdd(win, html);
    //ZuiMsgLoop();
    ZuiControl bt1 = NewZuiControl(L"Button", 0, 0, 0);
    ZuiControlSetFixedHeight(bt1, 50);
    bt1->m_aAnime = ZuiAnimationNew();
    ZuiLayoutAdd(win, bt1);

    ZuiControl null1 = NewZuiControl(L"NULL", 0, 0, 0);
    ZuiControlSetFixedHeight(null1, 50);
    ZuiLayoutAdd(win, null1);

    ZuiControl bt2 = NewZuiControl(L"Button", 0, 0, 0);
    ZuiControlSetFixedHeight(bt2, 50);
    //bt2->m_aAnime = ZuiAnimationNew();
    ZuiLayoutAdd(win, bt2);


    ZuiControl List = NewZuiControl(L"List", 0, 0, 0);
    ZuiControlSetBkColor(List, -1);
    ZuiControlSetDrag(List, TRUE);
    ZuiLayoutAdd(win, List);

    ZuiControlSetDrag(null1, TRUE);
    //ZuiUnInit();
    ZuiMsgLoop();
#endif
    ZuiRes res = ZuiResDBGetRes(L"file:client.xml", ZREST_STREAM);

    srand(time(0));
    ZuiControl c = ZuiControlFindName(ZuiLayoutLoad(res->p, res->plen), L"list");
    for (size_t i = 0; i < 20; i++)
    {
        ZuiControl null = NewZuiControl(L"ListElement", 0, 0, 0);
        ZuiControlSetBkColor(null, ARGB(rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1));
        ZuiControlSetFixedHeight(null, 50);
        ZuiControlSetAnimation(null);
        ZuiLayoutAdd(c, null);
    }

    ZuiResDBDelRes(res);

	ZuiMsgLoop();
	return;
}
main() {
    _tWinMain(0, 0, 0, 0);
}
#endif

#ifdef _WIN32
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		m_hInstance = hinstDLL;
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
#endif