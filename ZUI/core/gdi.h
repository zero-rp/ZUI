#ifndef __ZUI_PLATFORM_GRAPH_GDI_H__
#define __ZUI_PLATFORM_GRAPH_GDI_H__


#define ZTS_BOLD			1	//粗体
#define ZTS_ITALIC			2	//斜体
#define ZTS_ALIGN_LEFT		0	//水平居左
#define ZTS_ALIGN_CENTER	4	//水平居中
#define ZTS_ALIGN_RIGHT		8	//水平居右
#define ZTS_VALIGN_TOP		0	//垂直顶部对齐
#define ZTS_VALIGN_MIDDLE	16	//垂直居中对齐
#define ZTS_VALIGN_BOTTOM	32	//垂直底部对齐
#define ZTS_SHADOW			64	//阴影



/*图像*/
typedef struct _ZImage {
	void *image;
	ZuiInt Width;
	ZuiInt Height;
}*ZuiImage, ZImage;

/*图形*/
typedef struct _ZGraphics {
	ZuiInt Width;
	ZuiInt Height;	
	void *graphics;	//GDI+图形句柄
	HDC hdc;		//内存场景
	HBITMAP HBitmap;//位图句柄
	void* Bits;		//内存指针
}*ZuiGraphics, ZGraphics;

/*区域*/
typedef struct _ZRegion {

	void *region;	//区域句柄
}*ZuiRegion, ZRegion;

/*文本格式*/
typedef struct _ZStringFormat{
	ZuiColor TextColor;
	ZuiColor ShadowColor;
	void *font;//字体
	void *StringFormat;//文本格式
	void *FontFamily;//字体族
	void *BrushShadow;
	void *Brush;
}*ZuiStringFormat, ZStringFromat;



/**
* 此函数用作初始化图形引擎.
* @param Graphics ZuiGraphics对象
* @param Color 用作填充的颜色
* @param Left 左边
* @param Top 顶边
* @param Width 宽度
* @param Height 高度
* @此函数没有返回值.
*/
ZuiBool ZuiGraphInitialize();
/**
* 此函数用作填充一个矩形区域.
* @param Graphics ZuiGraphics对象
* @param Color 用作填充的颜色
* @param Left 左边
* @param Top 顶边
* @param Width 宽度
* @param Height 高度
* @此函数没有返回值.
*/
ZAPI(ZuiVoid) ZuiDrawFillRect(ZuiGraphics Graphics, ZuiColor Color, ZuiInt Left, ZuiInt Top, ZuiInt Width, ZuiInt Height);
/**
* 此函数用作绘制一个矩形.
* @param Graphics ZuiGraphics对象
* @param Color 用作填充的颜色
* @param Left 左边
* @param Top 顶边
* @param Width 宽度
* @param Height 高度
* @param LineWidth 线宽
* @此函数没有返回值.
*/
ZAPI(ZuiVoid) ZuiDrawRect(ZuiGraphics Graphics, ZuiColor Color, ZuiInt Left, ZuiInt Top, ZuiInt Width, ZuiInt Height, ZuiInt LineWidth);
/**
* 此函数用作绘制一个多边形.
* @param Graphics ZuiGraphics对象
* @param Color 用作填充的颜色
* @param point 顶点数组
* @param count 顶点数量
* @param LineWidth 线宽
* @此函数没有返回值.
*/
ZAPI(ZuiVoid) ZuiDrawPolygon(ZuiGraphics Graphics, ZuiColor Color, ZuiPoint *point, ZuiInt count, ZuiInt LineWidth);
/**
* 此函数用作绘制一条直线.
* @param Graphics ZuiGraphics对象
* @param Color 直线的颜色
* @param x1 起始点
* @param y1
* @param x2 结束点
* @param y2
* @param LineWidth 线宽
* @此函数没有返回值.
*/
ZAPI(ZuiVoid) ZuiDrawLine(ZuiGraphics Graphics, ZuiColor Color, ZuiInt x1, ZuiInt y1, ZuiInt x2, ZuiInt y2, ZuiInt LineWidth);
/**
* 此函数用作绘制文本.
* @param Graphics ZuiGraphics对象
* @param StringFormat 文本格式
* @param String 文本
* @param Rect 矩形区域
* @此函数没有返回值.
*/
ZAPI(ZuiVoid) ZuiDrawString(ZuiGraphics Graphics, ZuiStringFormat StringFormat, ZuiText String, ZuiRect Rect);
/**
* 此函数用作测量文本矩形.
* @param Graphics ZuiGraphics对象
* @param StringFormat 文本格式
* @param String 文本
* @param Rect 矩形区域 指针
* @此函数没有返回值.
*/
ZAPI(ZuiVoid) ZuiMeasureStringRect(ZuiGraphics Graphics, ZuiStringFormat StringFormat, ZuiText String, ZuiRectR Rect, ZuiRectR LRect);
/**
* 此函数用作绘制并填充圆角矩形.
* @param Graphics ZuiGraphics对象
* @param Color 用作填充的颜色
* @param BorderColor 用作描边的颜色
* @param x
* @param y
* @param Width
* @param Height
* @param LineWidth
* @param Round
* @此函数没有返回值.
*/
ZAPI(ZuiVoid) ZuiDrawFilledRoundRect(ZuiGraphics Graphics, ZuiColor Color, ZuiColor BorderColor, ZuiInt x, ZuiInt y, ZuiInt Width, ZuiInt Height, ZuiInt LineWidth, ZuiReal Round);
/**
* 此函数用作绘制圆角矩形.
* @param Graphics ZuiGraphics对象
* @param Color 颜色
* @param x
* @param y
* @param Width
* @param Height
* @param LineWidth
* @param Round
* @此函数没有返回值.
*/
ZAPI(ZuiVoid) ZuiDrawRoundRect(ZuiGraphics Graphics, ZuiColor Color, ZuiInt x, ZuiInt y, ZuiInt Width, ZuiInt Height, ZuiInt LineWidth, ZuiReal Round);
/**
* 此函数用作绘制图像.
* @param Graphics ZuiGraphics对象
* @param Image ZuiImage对象
* @param x
* @param y
* @此函数没有返回值.
*/
ZAPI(ZuiVoid) ZuiDrawImage(ZuiGraphics Graphics, ZuiImage Image, ZuiInt x, ZuiInt y);
/**
* 此函数用作绘制图像(扩展).
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
* @此函数没有返回值.
*/
ZAPI(ZuiVoid) ZuiDrawImageEx(ZuiGraphics Graphics, ZuiImage Image, ZuiInt x, ZuiInt y, ZuiInt Width, ZuiInt Height, ZuiInt xSrc, ZuiInt ySrc, ZuiInt WidthSrc, ZuiInt HeightSrc, ZuiByte Alpha);
ZAPI(ZuiVoid) ZuiAlphaBlendEx(ZuiGraphics Dest, ZuiInt x, ZuiInt y, ZuiInt Width, ZuiInt Height, ZuiGraphics Src, ZuiInt xSrc, ZuiInt ySrc, ZuiInt WidthSrc, ZuiInt HeightSrc, ZuiByte Alpha);
/**
* 此函数用作以透明通道复制图形.
* @param Dest 目标图形
* @param x 左边
* @param y 顶边
* @param Width 宽度
* @param Height 高度
* @param Src 源图形
* @param xSrc 源坐标
* @param ySrc 源顶边
* @param Alpha 透明度
* @此函数没有返回值.
*/
ZAPI(ZuiVoid) ZuiAlphaBlend(ZuiGraphics Dest, ZuiInt x, ZuiInt y, ZuiInt Width, ZuiInt Height, ZuiGraphics Src, ZuiInt xSrc, ZuiInt ySrc, ZuiByte Alpha);
/**
* 此函数用作使用指定颜色清空ZuiGraphics对象.
* @param Graphics ZuiGraphics对象
* @param Color 颜色
* @此函数没有返回值.
*/
ZAPI(ZuiVoid) ZuiGraphicsClear(ZuiGraphics Graphics, ZuiColor Color);
/**
* 此函数用作创建ZuiStringFormat(文本格式)对象.
* @param FontName 字体名称
* @param FontSize 字体大小
* @param TextColor 字体颜色
* @param ShadowColor 阴影颜色
* @param StringStyle 字体风格 VTS_开头的常量
* @param Returns 成功返回ZuiStringFormat对象.
*/
ZAPI(ZuiStringFormat) ZuiCreateStringFormat(ZuiText FontName, ZuiReal FontSize, ZuiColor TextColor, ZuiColor ShadowColor, ZuiInt StringStyle);
/**
* 此函数用作销毁ZuiStringFormat对象.
* @param StringFormat ZuiStringFormat对象
* @此函数没有返回值.
*/
ZAPI(ZuiVoid) ZuiDestroyStringFormat(ZuiStringFormat StringFormat);
/**
* 此函数用作在内存中创建ZuiGraphics对象.
* @param Width 宽度
* @param Height 高度
* @param Returns 成功返回创建的ZuiGraphics对象.
*/
ZAPI(ZuiGraphics) ZuiCreateGraphicsInMemory(ZuiInt Width, ZuiInt Height);
/**
* 此函数用作销毁ZuiGraphics对象.
* @param Graphics ZuiGraphics对象
* @此函数没有返回值.
*/
ZAPI(ZuiVoid) ZuiDestroyGraphics(ZuiGraphics Graphics);
ZAPI(ZuiRegion) ZuiCreateRegion();
ZAPI(ZuiRegion) ZuiCreateRegionRect(ZuiRect layoutRect);
ZAPI(ZuiVoid) ZuiDestroyRegion(ZuiRegion region);
ZAPI(ZuiBool) ZuiRegionIsVisiblePoint(ZuiRegion region, ZuiInt x, ZuiInt y);
ZAPI(ZuiVoid) ZuiRegionCombineExclude(ZuiRegion region, ZuiRegion region1);
ZAPI(ZuiVoid) ZuiRegionTranslate(ZuiRegion region, ZuiInt x, ZuiInt y);
ZAPI(ZuiBool) ZuiGraphicsSetClipRegion(ZuiGraphics Graphics, ZuiRegion region, ZuiInt mod);
ZAPI(ZuiBool) ZuiGraphicsGetClipRegion(ZuiGraphics Graphics, ZuiRegion region);
ZAPI(ZuiBool) ZuiGraphicsResetClip(ZuiGraphics Graphics);
/**
* 此函数用作从文件载入图像.
* @param Graphics ZuiGraphics对象
* @param FileName 路径
* @param Returns 成功返回ZuiImage对象.
*/
ZAPI(ZuiImage) ZuiLoadImageFromFile(ZuiText FileName);
/**
* 此函数用作从字节集载入图像.
* @param Graphics ZuiGraphics对象
* @param Binary 字节集
* @param Returns 成功返回ZuiImage对象.
*/
ZAPI(ZuiImage) ZuiLoadImageFromBinary(ZuiAny buf, ZuiInt len);
/*取图像帧数*/
ZAPI(ZuiInt) ZuiImageGetFrameCount(ZuiImage Image);
/*取图像各帧延时*/
ZAPI(ZuiInt) ZuiImageGetFrameIniDly(ZuiImage Image, ZuiInt *arry);
/*设置图像当前帧*/
ZAPI(ZuiInt) ZuiImageSetFrame(ZuiImage Image, ZuiInt index);
/**
* 此函数用作销毁ZuiImage对象.
* @param Image ZuiImage对象
* @此函数没有返回值.
*/
ZAPI(ZuiVoid) ZuiDestroyImage(ZuiImage Image);



#endif //__ZUI_PLATFORM_GRAPH_GDI_H__