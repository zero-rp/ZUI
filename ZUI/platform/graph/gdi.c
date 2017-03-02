#include <ZUI.h>
#if (defined PLATFORM_GRAPH_GDI) && (PLATFORM_GRAPH_GDI == 1) && (defined PLATFORM_OS_WIN)

#pragma comment(lib, "Msimg32.lib")  
#pragma comment(lib, "Gdiplus.lib")

typedef struct
{
    ZuiInt GdiplusVersion;
    ZuiInt DebugEventCallback;
    ZuiInt SuppressBackgroundThreadc;
    ZuiInt SuppressExternalCodecs;
}GdiplusStartupInput;
#ifdef __cplusplus
extern "C" {
#endif
    int __stdcall GdiplusStartup(int *token, GdiplusStartupInput *input, void *output);
    int __stdcall GdiplusShutdown(int token);
    int __stdcall GdipCreateFromHDC(HDC hdc, void **graphics);
    int __stdcall GdipSetTextRenderingHint(void *graphics, int a);
    int __stdcall GdipDeleteGraphics(void *graphics);
    int __stdcall GdipCreateSolidFill(ZuiColor color, void **brush);
    int __stdcall GdipFillRectangleI(void *graphics, void *brush, int x, int y, int width, int height);
    int __stdcall GdipDeleteBrush(void *brush);
    int __stdcall GdipDrawRectangleI(void *graphics, void *pen, int x, int y, int width, int height);
    int __stdcall GdipDrawPolygonI(void *graphics, void *pen, void* points, int count);
    int __stdcall GdipCreatePen1(ZuiColor color, ZuiReal width, int unit, void **pen);
    int __stdcall GdipDeletePen(void *pen);
    int __stdcall GdipDrawLineI(void *graphics, void *pen, ZuiInt x1, ZuiInt y1, ZuiInt x2, ZuiInt y2);
    int __stdcall GdipDrawString(void *graphics, ZuiText string, int length, void *font, ZuiRectR layoutRect, void *stringFormat, void *brush);
    int __stdcall GdipMeasureString(void *graphics, ZuiText string, ZuiInt length, void *font, ZuiRectR layoutRect, void *stringFormat, ZuiRectR boundingBox, ZuiInt codepointsFitted, ZuiInt linesFilled);
    int __stdcall GdipDrawImageI(void *graphics, void *image, int x, int y);
    int __stdcall GdipGraphicsClear(void *graphics, ZuiColor color);
    int __stdcall GdipCreateFontFamilyFromName(ZuiText name, void *fontCollection, void **fontFamily);
    int __stdcall GdipDeleteFontFamily(void *fontFamily);
    int __stdcall GdipSetStringFormatFlags(void *format, int flags);
    int __stdcall GdipCreateFont(void *fontFamily, ZuiReal emSize, int style, int unit, void **font);
    int __stdcall GdipCreateStringFormat(int formatAttributes, int language, void **StringFormat);
    int __stdcall GdipSetStringFormatAlign(void *format, int align);
    int __stdcall GdipSetStringFormatLineAlign(void *format, int align);
    int __stdcall GdipDeleteFont(void* font);
    int __stdcall GdipLoadImageFromStream(void* stream, void **image);
    int __stdcall GdipLoadImageFromFile(ZuiText filename, void **image);
    int __stdcall GdipGetImageWidth(void *image, int *width);
    int __stdcall GdipGetImageHeight(void *image, int *height);
    int __stdcall GdipImageGetFrameCount(void *image, char *guid, int *cunt);
    int __stdcall GdipImageSelectActiveFrame(void *image, char *guid, int *index);
    int __stdcall GdipGetPropertyItemSize(void *, int, int*);
    int __stdcall GdipGetPropertyItem(void *, int, int, char*);
    int __stdcall CLSIDFromString(ZuiText *, char *);
    int __stdcall GdipDisposeImage(void *image);
    int __stdcall GdipDrawImageRectRectI(void *graphics, void *image, int dstx, int dsty, int dstwidth, int dstheight, int srcx, int srcy, int srcwidth, int srcheight, int srcUnit, void* imageAttributes, void *callback, void * callbackData);
    int __stdcall CreateStreamOnHGlobal(HGLOBAL hGlobal, BOOL fDeleteOnRelease, void **ppstm);
    int __stdcall GdipCreateRegion(void **region);
    int __stdcall GdipCreateRegionRectI(ZuiRect layoutRect, void **region);
    int __stdcall GdipDeleteRegion(void *region);
    int __stdcall GdipIsVisibleRegionPointI(void *region, int x, int y, void *graphics, int * booln);
    int __stdcall GdipCombineRegionRegion(void *region, void *region1, int mode);
    int __stdcall GdipSetClipRegion(void *graphics, void *region, int mode);
    int __stdcall GdipGetClip(void *graphics, void *region);
    int __stdcall GdipTranslateRegionI(void *region, int x, int y);
    int __stdcall GdipResetClip(void *graphics);
#ifdef __cplusplus
}
#endif

ZRectR ZeroRectR;//零坐标矩形
ZuiInt pGdiToken;
/*初始化图形接口*/
ZuiBool ZuiGraphInitialize() {
    GdiplusStartupInput gdiplusStartupInput;
    memset(&gdiplusStartupInput, 0, sizeof(GdiplusStartupInput));
    gdiplusStartupInput.GdiplusVersion = 1;
    GdiplusStartup(&pGdiToken, &gdiplusStartupInput, NULL);//初始化GDI+
    MAKEZRECT(ZeroRectR, 0, 0, 0, 0);
    return TRUE;
}
/*反初始化图形接口*/
ZuiVoid ZuiGraphUnInitialize() {
    GdiplusShutdown(pGdiToken);
}
/*填充矩形*/
ZEXPORT ZuiVoid ZCALL ZuiDrawFillRect(ZuiGraphics Graphics, ZuiColor Color, ZuiInt Left, ZuiInt Top, ZuiInt Width, ZuiInt Height) {
    void *Brush;
    GdipCreateSolidFill(Color, &Brush);
    GdipFillRectangleI(Graphics->graphics, Brush, Left, Top, Width, Height);
    GdipDeleteBrush(Brush);
}
/*画矩形*/
ZEXPORT ZuiVoid ZCALL ZuiDrawRect(ZuiGraphics Graphics, ZuiColor Color, ZuiInt Left, ZuiInt Top, ZuiInt Width, ZuiInt Height, ZuiInt LineWidth) {
    if (Graphics) {
        void *pen;
        GdipCreatePen1(Color, (ZuiReal)LineWidth, 2, &pen);
        GdipDrawRectangleI(Graphics->graphics, pen, Left, Top, Width - 1, Height - 1);
        GdipDeletePen(pen);
    }
}
/*画多边形*/
ZEXPORT ZuiVoid ZCALL ZuiDrawPolygon(ZuiGraphics Graphics, ZuiColor Color, ZuiPoint *point, ZuiInt count, ZuiInt LineWidth) {
    void *pen;
    GdipCreatePen1(Color, (ZuiReal)LineWidth, 2, &pen);
    GdipDrawPolygonI(Graphics->graphics, pen, point, count);
    GdipDeletePen(pen);
}
/*画直线*/
ZEXPORT ZuiVoid ZCALL ZuiDrawLine(ZuiGraphics Graphics, ZuiColor Color, ZuiInt x1, ZuiInt y1, ZuiInt x2, ZuiInt y2, ZuiInt LineWidth)
{
    void *pen;
    GdipCreatePen1(Color, (ZuiReal)LineWidth, 2, &pen);
    GdipDrawLineI(Graphics->graphics, pen, x1, y1, x2, y2);
    GdipDeletePen(pen);
}
/*画文本*/
ZEXPORT ZuiVoid ZCALL ZuiDrawString(ZuiGraphics Graphics, ZuiStringFormat StringFormat, ZuiText String, ZuiRect Rect) {
    if (String) {
        if (!StringFormat) { StringFormat = Global_StringFormat; }//使用默认字体
        ZRectR r;
        MAKEZRECT(r, (ZuiReal)Rect->Left, (ZuiReal)Rect->Top, (ZuiReal)Rect->Width, (ZuiReal)Rect->Height);
        //int len = MultiByteToWideChar(0, 0, String, -1, 0, 0) * 2;
        //void *buf = malloc(len);
        //memset(buf, 0, len);
        //MultiByteToWideChar(0, 0, String, strlen(String), buf, len);
        GdipDrawString(Graphics->graphics, String, wcslen(String), StringFormat->font, &r, StringFormat->StringFormat, StringFormat->Brush);
        //free(buf);
    }
}
/*测量文本矩形*/
ZEXPORT ZuiVoid ZCALL ZuiMeasureStringRect(ZuiGraphics Graphics, ZuiStringFormat StringFormat, ZuiText String, ZuiRectR Rect, ZuiRectR LRect)
{
    if (String && Graphics) {
        if (!StringFormat) { StringFormat = Global_StringFormat; }//使用默认字体
        if (!LRect) { LRect = &ZeroRectR; }
        GdipMeasureString(Graphics->graphics, String, wcslen(String), StringFormat->font, LRect, StringFormat->StringFormat, Rect, 0, 0);
    }
}
/*填充圆角矩形*/
ZEXPORT ZuiVoid ZCALL ZuiDrawFilledRoundRect(ZuiGraphics Graphics, ZuiColor Color, ZuiColor BorderColor, ZuiInt x, ZuiInt y, ZuiInt Width, ZuiInt Height, ZuiInt LineWidth, ZuiReal Round) {

}
/*画圆角矩形*/
ZEXPORT ZuiVoid ZCALL ZuiDrawRoundRect(ZuiGraphics Graphics, ZuiColor Color, ZuiInt x, ZuiInt y, ZuiInt Width, ZuiInt Height, ZuiInt LineWidth, ZuiReal Round) {
    if (Round <= 0) {
        ZuiDrawRect(Graphics, Color, x, y, Width, Height, LineWidth);
        return;
    }

}
/*画图像*/
ZEXPORT ZuiVoid ZCALL ZuiDrawImage(ZuiGraphics Graphics, ZuiImage Image, ZuiInt x, ZuiInt y) {
    if (!(Graphics && Image)) {
        return;
    }
    GdipDrawImageI(Graphics->graphics, Image->image, x, y);
}
/*画图像缩放*/
ZEXPORT ZuiVoid ZCALL ZuiDrawImageEx(ZuiGraphics Graphics, ZuiImage Image, ZuiInt x, ZuiInt y, ZuiInt Width, ZuiInt Height, ZuiInt xSrc, ZuiInt ySrc, ZuiInt WidthSrc, ZuiInt HeightSrc, ZuiByte Alpha) {
    if (!(Graphics && Image)) {
        return;
    }
    if (Image->src.Left)
        xSrc = Image->src.Left;
    if (Image->src.Top)
        ySrc = Image->src.Top;

    if (Width <= 0) {
        if (Image->src.Width)
            Width = Image->src.Width - xSrc;
        else
            Width = (ZuiInt)(Image->Width - xSrc);
    }
    if (Height <= 0) {
        if (Image->src.Height)
            Height = Image->src.Height - ySrc;
        else
            Height = (ZuiInt)(Image->Height - ySrc);
    }


    if (WidthSrc <= 0 || WidthSrc + xSrc > Image->Width) {
        if (Image->src.Width)
            WidthSrc = Image->src.Width;
        else
            WidthSrc = (ZuiInt)(Image->Width - xSrc);
    }
    if (HeightSrc <= 0 || HeightSrc + ySrc > Image->Height) {
        if (Image->src.Height)
            HeightSrc = Image->src.Height;
        else
            HeightSrc = (ZuiInt)(Image->Height - ySrc);
    }
    GdipDrawImageRectRectI(Graphics->graphics, Image->image, x, y, Width, Height, xSrc, ySrc, WidthSrc, HeightSrc, 2, NULL, NULL, NULL);
}
/*复制位图扩展*/
ZEXPORT ZuiVoid ZCALL ZuiAlphaBlendEx(ZuiGraphics Dest, ZuiInt x, ZuiInt y, ZuiInt Width, ZuiInt Height, ZuiGraphics Src, ZuiInt xSrc, ZuiInt ySrc, ZuiInt WidthSrc, ZuiInt HeightSrc, ZuiByte Alpha) {
    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.AlphaFormat = 1;
    bf.SourceConstantAlpha = Alpha;
    int a = AlphaBlend(Dest->hdc, x, y, Width, Height, Src->hdc, xSrc, ySrc, WidthSrc, HeightSrc, bf);
}
/*复制位图*/
ZEXPORT ZuiVoid ZCALL ZuiAlphaBlend(ZuiGraphics Dest, ZuiInt x, ZuiInt y, ZuiInt Width, ZuiInt Height, ZuiGraphics Src, ZuiInt xSrc, ZuiInt ySrc, ZuiByte Alpha) {
    if (Dest && Src) {
        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.AlphaFormat = 1;
        bf.SourceConstantAlpha = Alpha;
        AlphaBlend(Dest->hdc, x, y, Width, Height, Src->hdc, xSrc, ySrc, (Src->Width > Width) ? Width : Src->Width, (Src->Height > Height) ? Height : Src->Height, bf);
    }
}
/*清除图形*/
ZEXPORT ZuiVoid ZCALL ZuiGraphicsClear(ZuiGraphics Graphics, ZuiColor Color) {
    if (Graphics)
    {
        GdipGraphicsClear(Graphics->graphics, Color);
    }
}
/*创建字体格式*/
ZEXPORT ZuiStringFormat ZCALL ZuiCreateStringFormat(ZuiText FontName, ZuiReal FontSize, ZuiColor TextColor, ZuiColor ShadowColor, ZuiInt StringStyle) {
    int i = 0;
    ZuiStringFormat StringFormat = (ZuiStringFormat)ZuiMalloc(sizeof(ZStringFromat));
    if (!StringFormat) { return NULL; }
    memset(StringFormat, 0, sizeof(ZStringFromat));

    GdipCreateStringFormat(0, 0, &StringFormat->StringFormat);//创建字体格式
    int a = GetLastError();
    if (StringStyle & ZTS_ALIGN_LEFT) {
        i = 0;
    }
    else if (StringStyle & ZTS_ALIGN_CENTER) {
        i = 1;
    }
    else if (StringStyle & ZTS_ALIGN_RIGHT)
    {
        i = 2;
    }
    GdipSetStringFormatAlign(StringFormat->StringFormat, i);//设置水平对齐方式
    if (StringStyle & ZTS_VALIGN_TOP) {
        i = 0;
    }
    else if (StringStyle & ZTS_VALIGN_MIDDLE) {
        i = 1;
    }
    else if (StringStyle & ZTS_VALIGN_BOTTOM)
    {
        i = 2;
    }
    GdipSetStringFormatLineAlign(StringFormat->StringFormat, i);//设置纵向对齐方式
    i = 0;
    if (StringStyle & ZTS_BOLD) {
        i = 1;
    }
    else if (StringStyle & ZTS_ITALIC) {
        i += 2;
    }

    GdipCreateFontFamilyFromName(FontName, NULL, &StringFormat->FontFamily);
    GdipCreateFont(StringFormat->FontFamily, FontSize, i, 0, &StringFormat->font);
    GdipDeleteFontFamily(StringFormat->FontFamily);
    if (StringStyle & ZTS_SHADOW)
    {
        GdipCreateSolidFill(ShadowColor, &StringFormat->BrushShadow);
    }
    GdipCreateSolidFill(TextColor, &StringFormat->Brush);
    StringFormat->TextColor = TextColor;
    return StringFormat;
}
/*销毁字体格式*/
ZEXPORT ZuiVoid ZCALL ZuiDestroyStringFormat(ZuiStringFormat StringFormat) {
    if (StringFormat) {
        GdipDeleteFontFamily(StringFormat->FontFamily);
        GdipDeleteFont(StringFormat->font);
        GdipDeleteBrush(StringFormat->Brush);
        ZuiFree(StringFormat);
    }
}
/*创建图形*/
ZEXPORT ZuiGraphics ZCALL ZuiCreateGraphicsInMemory(ZuiInt Width, ZuiInt Height) {
    ZuiGraphics Graphics = (ZuiGraphics)ZuiMalloc(sizeof(ZGraphics));
    if (!Graphics) { return NULL; }
    memset(Graphics, 0, sizeof(ZGraphics));
    Graphics->Width = Width;
    Graphics->Height = Height;

    Graphics->hdc = CreateCompatibleDC(NULL);
    BITMAPINFO BitmapInfo;
    memset(&BitmapInfo, 0, sizeof(BitmapInfo));
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biWidth = Width;
    BitmapInfo.bmiHeader.biHeight = Height;
    BitmapInfo.bmiHeader.biPlanes = 1;
    Graphics->HBitmap = CreateDIBSection(0, &BitmapInfo, 0, &Graphics->Bits, 0, 0);
    if (!Graphics->HBitmap) {
        DeleteDC(Graphics->hdc);
        return NULL;
    }
    HBITMAP OldBitmap = (HBITMAP)SelectObject(Graphics->hdc, (HGDIOBJ)Graphics->HBitmap);

    GdipCreateFromHDC(Graphics->hdc, &Graphics->graphics);
    GdipSetTextRenderingHint(Graphics->graphics, 5);//设置抗锯齿模式
    return Graphics;
}
/*销毁图形*/
ZEXPORT ZuiVoid ZCALL ZuiDestroyGraphics(ZuiGraphics Graphics) {
    if (Graphics) {
        GdipDeleteGraphics(Graphics->graphics);
        DeleteDC(Graphics->hdc);
        DeleteObject(Graphics->HBitmap);
        ZuiFree(Graphics);
    }
}
/*创建区域*/
ZEXPORT ZuiRegion ZCALL ZuiCreateRegion() {
    ZuiRegion region = (ZuiRegion)ZuiMalloc(sizeof(ZRegion));
    if (!region) { return NULL; }
    memset(region, 0, sizeof(ZRegion));
    GdipCreateRegion(&region->region);
    return region;
}
ZEXPORT ZuiRegion ZCALL ZuiCreateRegionRect(ZuiRect layoutRect) {
    ZuiRegion region = (ZuiRegion)ZuiMalloc(sizeof(ZRegion));
    if (!region) { return NULL; }
    GdipCreateRegionRectI(layoutRect, &region->region);
    return region;
}
ZEXPORT ZuiVoid ZCALL ZuiDestroyRegion(ZuiRegion region) {
    if (region) {
        GdipDeleteRegion(region->region);
        ZuiFree(region);
    }
}
/*区域求余*/
ZEXPORT ZuiVoid ZCALL ZuiRegionCombineExclude(ZuiRegion region, ZuiRegion region1) {
    if (region && region1) {
        GdipCombineRegionRegion(region->region, region1->region, 4);
    }
}
ZEXPORT ZuiVoid ZCALL ZuiRegionCombineAnd(ZuiRegion region, ZuiRegion region1) {
    if (region && region1) {
        GdipCombineRegionRegion(region->region, region1->region, 4);
    }
}
/*区域平移*/
ZEXPORT ZuiVoid ZCALL ZuiRegionTranslate(ZuiRegion region, ZuiInt x, ZuiInt y) {
    if (region) {
        GdipTranslateRegionI(region->region, x, y);
    }
}
ZEXPORT ZuiBool ZCALL ZuiGraphicsSetClipRegion(ZuiGraphics Graphics, ZuiRegion region, ZuiInt mod) {
    if (Graphics && region) {
        GdipSetClipRegion(Graphics->graphics, region->region, mod);
    }
    return FALSE;
}
ZEXPORT ZuiBool ZCALL ZuiGraphicsGetClipRegion(ZuiGraphics Graphics, ZuiRegion region) {
    if (Graphics && region) {
        GdipGetClip(Graphics->graphics, region->region);
    }
    return FALSE;
}
ZEXPORT ZuiBool ZCALL ZuiGraphicsResetClip(ZuiGraphics Graphics) {
    if (Graphics) {
        GdipResetClip(Graphics->graphics);
    }
    return FALSE;
}

ZEXPORT ZuiBool ZCALL ZuiRegionIsVisiblePoint(ZuiRegion region, ZuiInt x, ZuiInt y) {
    int b = 0;
    if (region) {
        GdipIsVisibleRegionPointI(region->region, x, y, NULL, &b);
    }
    return b;
}

/*加载图像自文件*/
ZEXPORT ZuiImage ZCALL ZuiLoadImageFromFile(ZuiText FileName) {
    ZuiImage Image = (ZuiImage)ZuiMalloc(sizeof(ZImage));
    if (!Image) { return NULL; }
    memset(Image, 0, sizeof(ZImage));
    GdipLoadImageFromFile(FileName, &Image->image);
    GdipGetImageHeight(Image->image, &Image->Height);
    GdipGetImageWidth(Image->image, &Image->Width);
    return Image;
}
/*加载图像自内存*/
ZEXPORT ZuiImage ZCALL ZuiLoadImageFromBinary(ZuiAny buf, ZuiInt len) {
    ZuiImage Image = (ZuiImage)ZuiMalloc(sizeof(ZImage));
    if (!Image) { return NULL; }
    memset(Image, 0, sizeof(ZImage));
    HGLOBAL hMem = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, len);
    void *str;
    CreateStreamOnHGlobal(hMem, 0, &str);
    void * mem = GlobalLock(hMem);
    memcpy(mem, buf, len);
    GlobalUnlock(hMem);
    GdipLoadImageFromStream(str, &Image->image);
    GdipGetImageHeight(Image->image, &Image->Height);
    GdipGetImageWidth(Image->image, &Image->Width);
    return Image;
}
/*取图像帧数*/
ZEXPORT ZuiInt ZCALL ZuiImageGetFrameCount(ZuiImage Image) {
    char guid[16];
    int cunt = 0;
    CLSIDFromString((ZuiText *)L"{6AEDBD6D-3FB5-418A-83A6-7F45229DC872}", guid);
    GdipImageGetFrameCount(Image->image, guid, &cunt);
    return cunt;
}
/*设置图像当前帧*/
ZEXPORT ZuiInt ZCALL ZuiImageSetFrame(ZuiImage Image, ZuiInt index) {
    char guid[16];
    int cunt = 0;
    CLSIDFromString((ZuiText *)L"{6AEDBD6D-3FB5-418A-83A6-7F45229DC872}", guid);
    return GdipImageSelectActiveFrame(Image->image, guid, (int *)index);
}
/*取图像各帧延时*/
typedef struct
{
    int id;
    ULONG length;
    WORD type;
    VOID *	value;
}PropertyItem;
ZEXPORT ZuiInt ZCALL ZuiImageGetFrameIniDly(ZuiImage Image, ZuiInt *arry) {
    int size;
    GdipGetPropertyItemSize(Image->image, 20736, &size);
    char *buf = ZuiMalloc(size);
    GdipGetPropertyItem(Image->image, 20736, size, buf);
    PropertyItem *a = (PropertyItem *)buf;
    memcpy(arry, a->value, a->length);
    ZuiFree(buf);
}
/*销毁图像*/
ZEXPORT ZuiVoid ZCALL ZuiDestroyImage(ZuiImage Image) {
    GdipDisposeImage(Image->image);
    ZuiFree(Image);
}

#endif

