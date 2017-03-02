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

/**区域*/
typedef struct _ZRegion {

    void *region;	///区域句柄
}*ZuiRegion, ZRegion;

/**文本格式*/
typedef struct _ZStringFormat {
    ZuiColor TextColor;
    ZuiColor ShadowColor;
    struct ZuiAggFont *font;
}*ZuiStringFormat, ZStringFromat;

#endif //PLATFORM_GRAPH_GDI
#endif //__ZUI_PLATFORM_GRAPH_AGG_H__