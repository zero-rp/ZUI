#include <stdio.h>
#include <ZUI.h>


 __declspec(dllexport) void __stdcall DLLDebug(){

	ZuiInit();
	

	HRESULT Hr = CoInitialize(NULL);
	if (FAILED(Hr)) return;

	ZuiControl pRoot = NewCControlUI("window", NULL, NULL, NULL);
	
	
	ZuiControl playout1 = NewCControlUI("HorizontalLayout", NULL, NULL, NULL);
	//ZuiControlCall(Proc_SetFixedWidth, playout1, (void *)100, 0, 0);

	ZuiControl pBut1 = NewCControlUI("button", NULL, NULL, NULL);
	ZuiControlCall(Proc_SetText, pBut1, L"扩展按钮1", NULL, NULL);
	pBut1->m_sToolTip = L"按钮1提示";
	ZuiControlCall(Proc_SetMaxWidth, pBut1, (void *)200, 0, 0);
	ZuiControl pBut2 = NewCControlUI("button", NULL, NULL, NULL);
	ZuiControlCall(Proc_SetText, pBut2, L"扩展按钮2", NULL, NULL);
	pBut2->m_sToolTip = L"按钮2提示";

	ZuiControl pBut3 = NewCControlUI("button", NULL, NULL, NULL);
	ZuiControlCall(Proc_SetText, pBut3, L"扩展按钮3", NULL, NULL);
	pBut3->m_sToolTip = L"按钮3提示";


	ZuiControl pBut11 = NewCControlUI("button", NULL, NULL, NULL);
	ZuiControlCall(Proc_SetText, pBut11, L"扩展按钮11", NULL, NULL);
	pBut11->m_sToolTip = L"按钮11提示";
	ZuiControlCall(Proc_SetFixedWidth, pBut11, (void *)50, 0, 0);
	ZuiControlCall(Proc_SetFixedHeight, pBut11, (void *)50, 0, 0);
	ZuiControl pBut12 = NewCControlUI("button", NULL, NULL, NULL);
	ZuiControlCall(Proc_SetText, pBut12, L"扩展按钮12", NULL, NULL);
	pBut12->m_sToolTip = L"按钮12提示";
	ZuiControl pBut13 = NewCControlUI("button", NULL, NULL, NULL);
	ZuiControlCall(Proc_SetText, pBut13, L"扩展按钮13", NULL, NULL);
	pBut13->m_sToolTip = L"按钮13提示";

	ZuiControl pBut14 = NewCControlUI("button", NULL, NULL, NULL);
	ZuiControlCall(Proc_SetText, pBut14, L"扩展按钮14", NULL, NULL);
	pBut14->m_sToolTip = L"按钮14提示";
	//pBut14->m_bFloat = TRUE;
	ZuiControlCall(Proc_SetFixedWidth, pBut14, (void *)500, 0, 0);
	ZuiControlCall(Proc_SetFixedHeight, pBut14, (void *)50, 0, 0);
	ZuiControlCall(Proc_SetFixedXY, pBut14, (void *)25, (void *)50, 0);

	ZuiControlCall(Proc_Layout_Add, pRoot, pBut1, NULL, NULL);
	ZuiControlCall(Proc_Layout_Add, playout1, pBut11, NULL, NULL);
	ZuiControlCall(Proc_Layout_Add, playout1, pBut12, NULL, NULL);
	ZuiControlCall(Proc_Layout_Add, playout1, pBut13, NULL, NULL);
	ZuiControlCall(Proc_Layout_Add, playout1, pBut14, NULL, NULL);
	ZuiControlCall(Proc_Layout_Add, pRoot, playout1, NULL, NULL);

	ZuiControlCall(Proc_Layout_Add, pRoot, pBut2, NULL, NULL);
	ZuiControlCall(Proc_Layout_Add, pRoot, pBut3, NULL, NULL);
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