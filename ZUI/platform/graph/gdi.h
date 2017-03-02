#ifndef __ZUI_PLATFORM_GRAPH_GDI_H__
#define __ZUI_PLATFORM_GRAPH_GDI_H__

#include <ZUI.h>

#if (defined PLATFORM_GRAPH_GDI) && (PLATFORM_GRAPH_GDI == 1) && (defined PLATFORM_OS_WIN)

/**图像*/
typedef struct _ZImage {
#ifdef AGG
    struct ZuiAggImage *image;	///图形句柄
#else
    void *image;	///图形句柄
#endif
    ZuiInt Width;
    ZuiInt Height;
    ZRect src;	//源矩形
}*ZuiImage, ZImage;

/**图形*/
typedef struct _ZGraphics {
    ZuiInt Width;
    ZuiInt Height;
#ifdef AGG
    struct ZuiAggGraphics *graphics;	///图形句柄
#else
    void *graphics;	///图形句柄
#endif
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
#ifdef AGG
    struct ZuiAggFont *font;	///图形句柄
#else
    void *font;///字体
    void *StringFormat;///文本格式
    void *FontFamily;///字体族
    void *BrushShadow;
    void *Brush;
#endif
}*ZuiStringFormat, ZStringFromat;

#endif //PLATFORM_GRAPH_GDI
#endif //__ZUI_PLATFORM_GRAPH_GDI_H__