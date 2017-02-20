/**
* @file		graphic.h
* @brief	图形接口.
* @author	[Zero](22249030)
* @version	1.0
* @date		$date
* @par History:
*
* [2016-10-25] <Zero> v1.0
*
* + v1.0版发布.
*
*/
#ifndef __ZUI_PLATFORM_GRAPH_H__
#define __ZUI_PLATFORM_GRAPH_H__

#if defined(__cplusplus)
extern "C"
{
#endif

/*FontStyleRegular    = 0,//常规  
  FontStyleBold       = 1,//加粗  
  FontStyleItalic     = 2,//斜体  
  FontStyleBoldItalic = 3,//粗斜  
  FontStyleUnderline  = 4,//下划线  
  FontStyleStrikeout  = 8//强调线，即在字体中部划线  
  */
#define ZTS_BOLD			1	//粗体
#define ZTS_ITALIC			2	//斜体
#define ZTS_ALIGN_LEFT		0	//水平居左
#define ZTS_ALIGN_CENTER	4	//水平居中
#define ZTS_ALIGN_RIGHT		8	//水平居右
#define ZTS_VALIGN_TOP		0	//垂直顶部对齐
#define ZTS_VALIGN_MIDDLE	16	//垂直居中对齐
#define ZTS_VALIGN_BOTTOM	32	//垂直底部对齐
#define ZTS_SHADOW			64	//阴影

#define AGG1

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
typedef struct _ZStringFormat{
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



/** 此函数用作初始化图形引擎.
* @return 此函数没有返回值.
*/
ZuiBool ZuiGraphInitialize();
ZuiVoid ZuiGraphUnInitialize();
/** 此函数用作填充一个矩形区域.
* @param Graphics ZuiGraphics对象
* @param Color 用作填充的颜色
* @param Left 左边
* @param Top 顶边
* @param Width 宽度
* @param Height 高度
* @return 此函数没有返回值.
*/
ZEXPORT ZuiVoid ZCALL ZuiDrawFillRect(ZuiGraphics Graphics, ZuiColor Color, ZuiInt Left, ZuiInt Top, ZuiInt Width, ZuiInt Height);
/** 此函数用作绘制一个矩形.
* @param Graphics ZuiGraphics对象
* @param Color 用作填充的颜色
* @param Left 左边
* @param Top 顶边
* @param Width 宽度
* @param Height 高度
* @param LineWidth 线宽
* @return 此函数没有返回值.
*/
ZEXPORT ZuiVoid ZCALL ZuiDrawRect(ZuiGraphics Graphics, ZuiColor Color, ZuiInt Left, ZuiInt Top, ZuiInt Width, ZuiInt Height, ZuiInt LineWidth);
/** 此函数用作绘制一个多边形.
* @param Graphics ZuiGraphics对象
* @param Color 用作填充的颜色
* @param point 顶点数组
* @param count 顶点数量
* @param LineWidth 线宽
* @return 此函数没有返回值.
*/
ZEXPORT ZuiVoid ZCALL ZuiDrawPolygon(ZuiGraphics Graphics, ZuiColor Color, ZuiPoint *point, ZuiInt count, ZuiInt LineWidth);
/** 此函数用作绘制一条直线.
* @param Graphics ZuiGraphics对象
* @param Color 直线的颜色
* @param x1 起始点
* @param y1
* @param x2 结束点
* @param y2
* @param LineWidth 线宽
* @return 此函数没有返回值.
*/
ZEXPORT ZuiVoid ZCALL ZuiDrawLine(ZuiGraphics Graphics, ZuiColor Color, ZuiInt x1, ZuiInt y1, ZuiInt x2, ZuiInt y2, ZuiInt LineWidth);
/** 此函数用作绘制文本.
* @param Graphics ZuiGraphics对象
* @param StringFormat 文本格式
* @param String 文本
* @param Rect 矩形区域
* @return 此函数没有返回值.
*/
ZEXPORT ZuiVoid ZCALL ZuiDrawString(ZuiGraphics Graphics, ZuiStringFormat StringFormat, ZuiText String, ZuiRect Rect);
/** 此函数用作测量文本矩形.
* @param Graphics ZuiGraphics对象
* @param StringFormat 文本格式
* @param String 文本
* @param Rect 矩形区域 指针
* @param LRect 参考矩形区域 指针
* @return 此函数没有返回值.
*/
ZEXPORT ZuiVoid ZCALL ZuiMeasureStringRect(ZuiGraphics Graphics, ZuiStringFormat StringFormat, ZuiText String, ZuiRectR Rect, ZuiRectR LRect);
/** 此函数用作绘制并填充圆角矩形.
* @param Graphics ZuiGraphics对象
* @param Color 用作填充的颜色
* @param BorderColor 用作描边的颜色
* @param x
* @param y
* @param Width
* @param Height
* @param LineWidth
* @param Round
* @return 此函数没有返回值.
*/
ZEXPORT ZuiVoid ZCALL ZuiDrawFilledRoundRect(ZuiGraphics Graphics, ZuiColor Color, ZuiColor BorderColor, ZuiInt x, ZuiInt y, ZuiInt Width, ZuiInt Height, ZuiInt LineWidth, ZuiReal Round);
/** 此函数用作绘制圆角矩形.
* @param Graphics ZuiGraphics对象
* @param Color 颜色
* @param x
* @param y
* @param Width
* @param Height
* @param LineWidth
* @param Round
* @return 此函数没有返回值.
*/
ZEXPORT ZuiVoid ZCALL ZuiDrawRoundRect(ZuiGraphics Graphics, ZuiColor Color, ZuiInt x, ZuiInt y, ZuiInt Width, ZuiInt Height, ZuiInt LineWidth, ZuiReal Round);
/** 此函数用作绘制图像.
* @param Graphics ZuiGraphics对象
* @param Image ZuiImage对象
* @param x
* @param y
* @return 此函数没有返回值.
*/
ZEXPORT ZuiVoid ZCALL ZuiDrawImage(ZuiGraphics Graphics, ZuiImage Image, ZuiInt x, ZuiInt y);
/** 此函数用作绘制图像(扩展).
* @param Graphics ZuiGraphics对象
* @param Image ZuiImage对象
* @param x 左边
* @param y 顶边
* @param Width 宽度
* @param Height 高度
* @param xSrc 源坐标
* @param ySrc 源顶边
* @param WidthSrc 源宽度
* @param HeightSrc 源高度
* @param Alpha 透明度
* @return 此函数没有返回值.
*/
ZEXPORT ZuiVoid ZCALL ZuiDrawImageEx(ZuiGraphics Graphics, ZuiImage Image, ZuiInt x, ZuiInt y, ZuiInt Width, ZuiInt Height, ZuiInt xSrc, ZuiInt ySrc, ZuiInt WidthSrc, ZuiInt HeightSrc, ZuiByte Alpha);
ZEXPORT ZuiVoid ZCALL ZuiAlphaBlendEx(ZuiGraphics Dest, ZuiInt x, ZuiInt y, ZuiInt Width, ZuiInt Height, ZuiGraphics Src, ZuiInt xSrc, ZuiInt ySrc, ZuiInt WidthSrc, ZuiInt HeightSrc, ZuiByte Alpha);
/** 此函数用作以透明通道复制图形.
* @param Dest 目标图形
* @param x 左边
* @param y 顶边
* @param Width 宽度
* @param Height 高度
* @param Src 源图形
* @param xSrc 源坐标
* @param ySrc 源顶边
* @param Alpha 透明度
* @return 此函数没有返回值.
*/
ZEXPORT ZuiVoid ZCALL ZuiAlphaBlend(ZuiGraphics Dest, ZuiInt x, ZuiInt y, ZuiInt Width, ZuiInt Height, ZuiGraphics Src, ZuiInt xSrc, ZuiInt ySrc, ZuiByte Alpha);
/** 此函数用作使用指定颜色清空ZuiGraphics对象.
* @param Graphics ZuiGraphics对象
* @param Color 颜色
* @return 此函数没有返回值.
*/
ZEXPORT ZuiVoid ZCALL ZuiGraphicsClear(ZuiGraphics Graphics, ZuiColor Color);
/** 此函数用作创建ZuiStringFormat(文本格式)对象.
* @param FontName 字体名称
* @param FontSize 字体大小
* @param TextColor 字体颜色
* @param ShadowColor 阴影颜色
* @param StringStyle 字体风格 VTS_开头的常量
* @return 成功返回ZuiStringFormat对象.
*/
ZEXPORT ZuiStringFormat ZCALL ZuiCreateStringFormat(ZuiText FontName, ZuiReal FontSize, ZuiColor TextColor, ZuiColor ShadowColor, ZuiInt StringStyle);
/** 此函数用作销毁ZuiStringFormat对象.
* @param StringFormat ZuiStringFormat对象
* @return 此函数没有返回值.
*/
ZEXPORT ZuiVoid ZCALL ZuiDestroyStringFormat(ZuiStringFormat StringFormat);
/** 此函数用作在内存中创建ZuiGraphics对象.
* @param Width 宽度
* @param Height 高度
* @return 成功返回创建的ZuiGraphics对象.
*/
ZEXPORT ZuiGraphics ZCALL ZuiCreateGraphicsInMemory(ZuiInt Width, ZuiInt Height);
/** 此函数用作销毁ZuiGraphics对象.
* @param Graphics ZuiGraphics对象
* @return 此函数没有返回值.
*/
ZEXPORT ZuiVoid ZCALL ZuiDestroyGraphics(ZuiGraphics Graphics);
ZEXPORT ZuiRegion ZCALL ZuiCreateRegion();
ZEXPORT ZuiRegion ZCALL ZuiCreateRegionRect(ZuiRect layoutRect);
ZEXPORT ZuiVoid ZCALL ZuiDestroyRegion(ZuiRegion region);
ZEXPORT ZuiBool ZCALL ZuiRegionIsVisiblePoint(ZuiRegion region, ZuiInt x, ZuiInt y);
ZEXPORT ZuiVoid ZCALL ZuiRegionCombineExclude(ZuiRegion region, ZuiRegion region1);
ZEXPORT ZuiVoid ZCALL ZuiRegionTranslate(ZuiRegion region, ZuiInt x, ZuiInt y);
ZEXPORT ZuiBool ZCALL ZuiGraphicsSetClipRegion(ZuiGraphics Graphics, ZuiRegion region, ZuiInt mod);
ZEXPORT ZuiBool ZCALL ZuiGraphicsGetClipRegion(ZuiGraphics Graphics, ZuiRegion region);
ZEXPORT ZuiBool ZCALL ZuiGraphicsResetClip(ZuiGraphics Graphics);
/** 此函数用作从文件载入图像.
* @param FileName 路径
* @return 成功返回ZuiImage对象.
*/
ZEXPORT ZuiImage ZCALL ZuiLoadImageFromFile(ZuiText FileName);
/** 此函数用作从字节集载入图像.
* @param buf 地址
* @param len 长度
* @return 成功返回ZuiImage对象.
*/
ZEXPORT ZuiImage ZCALL ZuiLoadImageFromBinary(ZuiAny buf, ZuiInt len);
/*取图像帧数*/
ZEXPORT ZuiInt ZCALL ZuiImageGetFrameCount(ZuiImage Image);
/*取图像各帧延时*/
ZEXPORT ZuiInt ZCALL ZuiImageGetFrameIniDly(ZuiImage Image, ZuiInt *arry);
/*设置图像当前帧*/
ZEXPORT ZuiInt ZCALL ZuiImageSetFrame(ZuiImage Image, ZuiInt index);
/** 此函数用作销毁ZuiImage对象.
* @param Image ZuiImage对象
* @return 此函数没有返回值.
*/
ZEXPORT ZuiVoid ZCALL ZuiDestroyImage(ZuiImage Image);


#ifdef __cplusplus
}
#endif
#endif //__ZUI_PLATFORM_GRAPH_GDI_H__