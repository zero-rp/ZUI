#ifndef __ZUI_PLATFORM_GRAPH_AGG_H__
#define __ZUI_PLATFORM_GRAPH_AGG_H__

#include <ZUI.h>

#if (defined PLATFORM_GRAPH_AGG) && (PLATFORM_GRAPH_AGG == 1)

/**图像*/
typedef struct _ZImage {
    struct ZuiAggImage *image;	///图形句柄
    ZuiInt Width;
    ZuiInt Height;
    ZRect src;	//源矩形
}*ZuiImage, ZImage;

/**图形*/
typedef struct _ZGraphics {
    ZuiInt Width;
    ZuiInt Height;
    struct ZuiAggGraphics *graphics;	///图形句柄
#ifdef WIN32
    HDC hdc;		///内存场景
    HBITMAP HBitmap;///位图句柄
#endif
    void* Bits;		///内存指针
}*ZuiGraphics, ZGraphics;

/**字体*/
typedef struct _ZFont {
    ZuiColor TextColor;
    ZuiColor ShadowColor;
    ZuiReal FontSize;       ///字体大小
    struct ZuiAggFont *font;
}*ZuiFont, ZFont;

#endif //PLATFORM_GRAPH_AGG
#endif //__ZUI_PLATFORM_GRAPH_AGG_H__