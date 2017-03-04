#ifndef __ZUI_PLATFORM_GRAPH_GDI_H__
#define __ZUI_PLATFORM_GRAPH_GDI_H__

#include <ZUI.h>

#if (defined PLATFORM_GRAPH_GDI) && (PLATFORM_GRAPH_GDI == 1) && (defined PLATFORM_OS_WIN)

/**图像*/
typedef struct _ZImage {
    void *image;	///图形句柄
    ZuiInt Width;
    ZuiInt Height;
    ZRect src;	//源矩形
}*ZuiImage, ZImage;

/**图形*/
typedef struct _ZGraphics {
    ZuiInt Width;
    ZuiInt Height;
    void *graphics;	///图形句柄
    HDC hdc;		///内存场景
    HBITMAP HBitmap;///位图句柄
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
    void *font;///字体
    void *StringFormat;///文本格式
    void *FontFamily;///字体族
    void *BrushShadow;
    void *Brush;
}*ZuiStringFormat, ZStringFromat;

#endif //PLATFORM_GRAPH_GDI
#endif //__ZUI_PLATFORM_GRAPH_GDI_H__