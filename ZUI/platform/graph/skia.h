#ifndef __ZUI_PLATFORM_GRAPH_SKIA_H__
#define __ZUI_PLATFORM_GRAPH_SKIA_H__

#include <ZUI.h>
#ifdef PLATFORM_OS_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
/**图像*/
typedef struct _ZImage {
    struct ZuiSkiaImage *image;	///图形句柄
    ZuiInt Width;
    ZuiInt Height;
    ZRect src;      //源矩形
}*ZuiImage, ZImage;

/**图形*/
typedef struct _ZGraphics {
    ZuiInt Width;
    ZuiInt Height;
    struct ZuiSkiaGraphics *graphics;	///图形句柄
#ifdef PLATFORM_OS_WIN
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
    struct ZuiSkiaFont *font;
}*ZuiFont, ZFont;

/**路径*/
typedef struct _ZPath {
    struct ZuiSkiaPath* path;
}*ZuiPath, ZPath;
#endif //__ZUI_PLATFORM_GRAPH_SKIA_H__