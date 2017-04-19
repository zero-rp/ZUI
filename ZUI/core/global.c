#include <ZUI.h>

rb_root *Global_MenuClass;
ZuiImage Global_Res_Ctl[4][3];          //窗口控制按钮资源
ZuiFont Global_Font;                    //默认字体风格
#ifdef PLATFORM_OS_WIN
    HINSTANCE m_hInstance = NULL;           //模块句柄
#endif // PLATFORM_OS_WIN
DArray *m_aPreMessages;                 //所有ZuiPaintManager实例数组
ZuiReal	Global_DPI_X = 0;               //DPI
ZuiReal	Global_DPI_Y = 0;               //DPI
ZuiText Global_DefaultFontName;         //系统默认字体名称
#if ((defined HAVE_UV) && (HAVE_UV == 1)) || ((defined HAVE_DUV) && (HAVE_DUV == 1))
    void *Global_loop;                      //全局loop句柄
#endif
ZuiBool ZuiGlobalInit() {

    LOGFONT lf;
    SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lf, 0);
    Global_DefaultFontName = ZuiWcsdup(lf.lfFaceName);
    Global_Font = ZuiCreateFont(Global_DefaultFontName, 12, FALSE, FALSE);

    Global_MenuClass = rb_new();
#if ((defined HAVE_UV) && (HAVE_UV == 1)) || ((defined HAVE_DUV) && (HAVE_DUV == 1))
    Global_loop = uv_default_loop();
#endif
    return TRUE;
}

ZuiVoid ZuiGlobalUnInit() {
    ZuiFree(Global_DefaultFontName);
    ZuiDestroyStringFormat(Global_Font);

}
