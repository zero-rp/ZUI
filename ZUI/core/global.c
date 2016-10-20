#include <ZUI.h>


ZuiImage Global_Res_Ctl[4][3];	//窗口控制按钮资源
ZuiStringFormat Global_StringFormat;	//默认字体风格
HINSTANCE m_hInstance = NULL;	//模块句柄
DArray *m_aPreMessages;	//所有ZuiPaintManager实例数组


ZuiBool ZuiGlobalInit(){


	
	Global_StringFormat = ZuiCreateStringFormat(L"微软雅黑", 12, ARGB(255, 255, 0, 0), ARGB(255, 255, 255, 255), ZTS_ALIGN_CENTER | ZTS_VALIGN_MIDDLE | ZTS_SHADOW);


	return TRUE;
}
