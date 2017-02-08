#include <stdio.h>
#include <ZUI.h>
#if !(defined NDEBUG)
#ifdef _WIN32
/*
#include "play.h"
#pragma comment(lib, "play.lib")
ZuiPlay play;
ZEXPORT ZuiAny ZCALL ZuiFFTProcEx(ZuiInt ProcId, ZuiControl cp, ZuiOption p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3) {
	switch (ProcId)
	{
	case Proc_OnEvent: {
		TEventUI *event = (TEventUI *)Param1;
		if (event->Type == ZEVENT_TIMER) {
			ZuiControlInvalidate(cp);
		}
		break;
	}
	case Proc_OnPaint: {
		ZuiGraphics gp = (ZuiGraphics)Param1;
		RECT *rc = &cp->m_rcItem;
		float power[2048];
		if (ZPlayGetFFT(play, power)) {
			int fftnum = 80;

			int fpFen = 2048 / 2 / fftnum;

			float level[80];
			for (int i = 0; i < 80; i++)
			{
				int cPos = i * fpFen + fpFen / 2;
				double bandTotal = power[cPos - 2] * 0.1 + power[cPos - 1] * 0.15
					+ power[cPos] * 0.5 + power[cPos + 1] * 0.15 + power[cPos + 2] * 0.1;

				level[i] = (int)(bandTotal + 0.5);
			}
			for (int i = 0; i < fftnum; i++)
			{
				if (level[i] < 0)
					level[i] = 0;
				else if (level[i] > 128)
					level[i] = 128;
			}
			for (int i = 0; i < fftnum; i++)
			{
				if(level[i])
					level[i] =  level[i] / 64 * 23;
			}
			for (size_t i = 1; i < fftnum; i++)
			{
				ZuiDrawLine(gp, ARGB(255, 223, 112, 255), rc->left + i, rc->bottom - level[i-1], rc->left + i, rc->bottom, 1);
			}
		}
		break;
	}
	case Proc_OnInit: {
		break;
	}
	default:
		break;
	}
	return ZuiDefaultControlProc(ProcId, cp, p, Param1, Param2, Param3);
}
*/
#endif
void __stdcall DLLDebug(){
	ZuiInit();
	
#if 1
    ZuiControl win = NewZuiControl(L"Window", 0, 0, 0);
    ZProc(win, Window_SetSize, 500, 500);
    ZProc(win, Window_SetNoBox, TRUE);
    ZProc(win, Window_SetNoBox, TRUE);

    ZuiControl html = NewZuiControl(L"Html", 0, 0, 0);
    ZuiControlSetBkColor(html, -1);
    ZuiControlSetDrag(html, TRUE);
    ZuiLayoutAdd(win, html);
    ZuiMsgLoop();
#endif

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
	
	
	ZuiControl cp = ZuiControlFindName(ZuiLayoutLoad(p, flen), L"Play_FFT");
#ifdef _WIN32
	//cp->call = (ZCtlProc)ZuiFFTProcEx;
	//ZuiPaintManagerSetTimer(cp, 1000, 100);
	//play = ZPlayNew();
#endif
	ZuiMsgLoop();


	return;
}
int  main(){
	DLLDebug();
}

#endif
#ifdef _WIN32
#if !(defined NDEBUG)
int _stdcall _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	DLLDebug();
}
#endif


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