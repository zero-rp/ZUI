#include <ZUI.h>

rb_root *Global_MenuClass;
ZuiImage Global_Res_Ctl[4][3];			//窗口控制按钮资源
ZuiStringFormat Global_StringFormat;	//默认字体风格
#ifdef PLATFORM_OS_WIN
HINSTANCE m_hInstance = NULL;			//模块句柄
#endif // PLATFORM_OS_WIN
DArray *m_aPreMessages;					//所有ZuiPaintManager实例数组
ZuiReal	Global_DPI_X = 0;				//DPI
ZuiReal	Global_DPI_Y = 0;				//DPI
ZuiText Global_DefaultFontName;			//系统默认字体名称
void *Global_loop;                 //全局loop句柄
ZuiBool ZuiGlobalInit() {

    LOGFONT lf;
    SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lf, 0);
    Global_DefaultFontName = ZuiWcsdup(lf.lfFaceName);
    Global_StringFormat = ZuiCreateStringFormat(Global_DefaultFontName, 12, ARGB(254, 0, 0, 0), ARGB(255, 255, 255, 255), ZTS_ALIGN_CENTER | ZTS_VALIGN_MIDDLE);

    Global_MenuClass = rb_new();
    Global_loop = uv_default_loop();
    return TRUE;
}
ZuiVoid ZuiGlobalUnInit() {
    ZuiFree(Global_DefaultFontName);
    ZuiDestroyStringFormat(Global_StringFormat);

}
