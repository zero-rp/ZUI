#ifndef __ZUI_PLATFORM_GRAPH_H__
#define __ZUI_PLATFORM_GRAPH_H__
#include <ZUI.h>
#if defined(__cplusplus)
extern "C"
{
#endif

    #define ARGB(A,R,G,B) ((int)((((A)&0xff)<<24)|(((R)&0xff)<<16)|(((G)&0xff)<<8)|((B)&0xff)))
    #define RGBA(R,G,B,A) ARGB(A,R,G,B)
    #define RGB2ARGB(COLOR,A) RGBA(((COLOR) >> 16 & 0xFF), ((COLOR) >> 8 & 0xFF), ((COLOR) & 0xFF), (A))
    #ifdef LINUX
    #define RGB(r,g,b) ((ZuiInt)(((ZuiByte)(r)|((short)((ZuiByte)(g))<<8))|(((ZuiInt)(ZuiByte)(b))<<16)))
    #endif

    /*FontStyleRegular    = 0,//常规
    FontStyleBold       = 1,//加粗
    FontStyleItalic     = 2,//斜体
    FontStyleBoldItalic = 3,//粗斜
    FontStyleUnderline  = 4,//下划线
    FontStyleStrikeout  = 8//强调线，即在字体中部划线
    */
#define ZTS_BOLD            1	//粗体
#define ZTS_ITALIC          2	//斜体
#define ZTS_ALIGN_LEFT      0	//水平居左
#define ZTS_ALIGN_CENTER    4	//水平居中
#define ZTS_ALIGN_RIGHT		8	//水平居右
#define ZTS_VALIGN_TOP		0	//垂直顶部对齐
#define ZTS_VALIGN_MIDDLE	16	//垂直居中对齐
#define ZTS_VALIGN_BOTTOM	32	//垂直底部对齐
#define ZTS_SHADOW			64	//阴影


#include "format.h"

    /** 此函数用作初始化图形引擎.
    * @return 此函数没有返回值.
    */
    ZuiBool ZuiGraphInitialize();
    ZuiVoid ZuiGraphUnInitialize();
    //填充三角形
    ZEXPORT ZuiVoid ZCALL ZuiDrawFilltriangle(ZuiGraphics Graphics, ZuiColor Color, ZuiReal x1, ZuiReal y1, ZuiReal x2, ZuiReal y2, ZuiReal x3, ZuiReal y3);
    //绘制三角形
    ZEXPORT ZuiVoid ZCALL ZuiDrawtriangle(ZuiGraphics Graphics, ZuiColor Color, ZuiReal x1, ZuiReal y1, ZuiReal x2, ZuiReal y2, ZuiReal x3, ZuiReal y3, ZuiReal LineWidth);
    //填充矩形
    ZEXPORT ZuiVoid ZCALL ZuiDrawFillRect(ZuiGraphics Graphics, ZuiColor Color, ZuiReal Left, ZuiReal Top, ZuiReal Width, ZuiReal Height);
    //绘制矩形
    ZEXPORT ZuiVoid ZCALL ZuiDrawRect(ZuiGraphics Graphics, ZuiColor Color, ZuiReal Left, ZuiReal Top, ZuiReal Width, ZuiReal Height, ZuiReal LineWidth);
    //绘制多边形
    ZEXPORT ZuiVoid ZCALL ZuiDrawPolygon(ZuiGraphics Graphics, ZuiColor Color, ZuiPointR *point, ZuiInt count, ZuiReal LineWidth);
    //绘制直线
    ZEXPORT ZuiVoid ZCALL ZuiDrawLine(ZuiGraphics Graphics, ZuiColor Color, ZuiReal x1, ZuiReal y1, ZuiReal x2, ZuiReal y2, ZuiReal LineWidth);
    //绘制文本
	ZEXPORT ZuiVoid ZCALL ZuiDrawStringPt(ZuiGraphics Graphics, ZuiFont Font, ZuiColor Color, ZuiText String, ZuiInt StrLens, ZPointR Pt[]);
    ZEXPORT ZuiVoid ZCALL ZuiDrawString(ZuiGraphics Graphics, ZuiFont Font, ZuiText String, ZuiInt StrLens, ZRectR *Rect, ZuiColor Color, ZuiUInt TextStyle);
    //测量字符大小
    ZEXPORT ZuiVoid ZCALL ZuiMeasureTextSize(ZuiFont Font, _ZuiText String, ZuiSizeR Size);
    //绘制图像
    ZEXPORT ZuiVoid ZCALL ZuiDrawImageEx(ZuiGraphics Graphics, ZuiImage Image, ZuiReal x, ZuiReal y, ZuiReal Width, ZuiReal Height, ZuiReal xSrc, ZuiReal ySrc, ZuiReal WidthSrc, ZuiReal HeightSrc, ZuiByte Alpha);
    //复制图形
    ZEXPORT ZuiVoid ZCALL ZuiAlphaBlendEx(ZuiGraphics Dest, ZuiInt srcX1, ZuiInt srcY1, ZuiInt srcX2, ZuiInt srcY2, ZuiInt dstX, ZuiInt dstY, ZuiGraphics Src, ZuiByte Alpha);
    ZEXPORT ZuiVoid ZCALL ZuiAlphaBlend(ZuiGraphics Dest, ZuiInt x, ZuiInt y, ZuiInt Width, ZuiInt Height, ZuiGraphics Src, ZuiInt xSrc, ZuiInt ySrc, ZuiByte Alpha);
    ZEXPORT ZuiVoid ZCALL ZuiBitBltEx(ZuiGraphics Dest, ZuiInt srcX1, ZuiInt srcY1, ZuiInt srcX2, ZuiInt srcY2, ZuiInt dstX, ZuiInt dstY, ZuiGraphics Src);
    //清空图形
    ZEXPORT ZuiVoid ZCALL ZuiGraphicsClear(ZuiGraphics Graphics, ZuiColor Color);
    //创建字体
    ZEXPORT ZuiFont ZCALL ZuiCreateFont(ZuiText FontName, ZuiReal FontSize, ZuiBool Bold, ZuiBool Italic);
    //销毁字体
    ZEXPORT ZuiVoid ZCALL ZuiDestroyFont(ZuiFont StringFormat);
    //创建图形
    ZEXPORT ZuiGraphics ZCALL ZuiCreateGraphicsInMemory(ZuiInt Width, ZuiInt Height);
    ZEXPORT ZuiGraphics ZCALL ZuiCreateGraphics();
    /*附加到一块内存上*/
    ZEXPORT ZuiGraphics ZCALL ZuiCreateGraphicsAttach(ZuiGraphics Graphics, ZuiAny bits, ZuiInt Width, ZuiInt Height, ZuiInt stride);
    //销毁图形
    ZEXPORT ZuiVoid ZCALL ZuiDestroyGraphics(ZuiGraphics Graphics);
    //设置剪辑区
    ZEXPORT ZuiBool ZCALL ZuiGraphicsSetClipBox(ZuiGraphics Graphics, ZuiRectR box);
    //获取剪辑区
    ZEXPORT ZuiBool ZCALL ZuiGraphicsGetClipBox(ZuiGraphics Graphics, ZuiRectR box);
    //重置剪辑区
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
#endif