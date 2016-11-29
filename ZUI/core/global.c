#include <ZUI.h>

rb_root *Global_MenuClass;
ZuiImage Global_Res_Ctl[4][3];			//窗口控制按钮资源
ZuiStringFormat Global_StringFormat;	//默认字体风格
HINSTANCE m_hInstance = NULL;			//模块句柄
DArray *m_aPreMessages;					//所有ZuiPaintManager实例数组
ZuiReal	Global_DPI_X = 0;				//DPI
ZuiReal	Global_DPI_Y = 0;				//DPI
ZuiText Global_DefaultFontName;			//系统默认字体名称
ZuiBool ZuiGlobalInit(){

	LOGFONT lf;
	SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lf, 0);
	Global_DefaultFontName = _wcsdup(lf.lfFaceName);
	Global_StringFormat = ZuiCreateStringFormat(Global_DefaultFontName, 12, ARGB(254, 0, 0, 0), ARGB(255, 255, 255, 255), ZTS_ALIGN_CENTER | ZTS_VALIGN_MIDDLE);

	Global_MenuClass = rb_new();
	return TRUE;
}
