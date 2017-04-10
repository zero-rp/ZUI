#ifndef __ZUI_PLATFORM_GRAPH_SKIA_H__
#define __ZUI_PLATFORM_GRAPH_SKIA_H__

#include <ZUI.h>

#if (defined PLATFORM_GRAPH_SKIA) && (PLATFORM_GRAPH_SKIA == 1) || (defined PLATFORM_OS_ANDROID)


/**图像*/
typedef struct _ZImage {
	void *image;	///图形句柄
	ZuiInt Width;
	ZuiInt Height;
	ZRect src;		//源矩形
}*ZuiImage, ZImage;

/**图形*/
typedef struct _ZGraphics {
	ZuiInt Width;
	ZuiInt Height;



}*ZuiGraphics, ZGraphics;

/**区域*/
typedef struct _ZRegion {

	void *region;	///区域句柄
}*ZuiRegion, ZRegion;

/**文本格式*/
typedef struct _ZStringFormat {
	ZuiColor TextColor;     ///字体颜色
	ZuiColor ShadowColor;   ///阴影颜色
	void *font;             ///字体
	void *StringFormat;     ///文本格式
	void *FontFamily;       ///字体族
	void *BrushShadow;
	void *Brush;
}*ZuiStringFormat, ZStringFromat;




#endif //PLATFORM_GRAPH_SKIA
#endif //__ZUI_PLATFORM_GRAPH_SKIA_H__