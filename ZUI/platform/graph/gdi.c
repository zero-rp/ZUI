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
    int __stdcall GdipFillRectangle(void *graphics, void *brush, ZuiReal x, ZuiReal y, ZuiReal width, ZuiReal height);
    int __stdcall GdipDeleteBrush(void *brush);
    int __stdcall GdipDrawRectangle(void *graphics, void *pen, ZuiReal x, ZuiReal y, ZuiReal width, ZuiReal height);
    int __stdcall GdipDrawPolygon(void *graphics, void *pen, void* points, int count);
    int __stdcall GdipCreatePen1(ZuiColor color, ZuiReal width, int unit, void **pen);
    int __stdcall GdipDeletePen(void *pen);
    int __stdcall GdipDrawLineI(void *graphics, void *pen, ZuiInt x1, ZuiInt y1, ZuiInt x2, ZuiInt y2);
	int __stdcall GdipDrawLine(void *graphics, void *pen, ZuiReal x1, ZuiReal y1, ZuiReal x2, ZuiReal y2);
    int __stdcall GdipDrawDriverString(void *graphics, ZuiText string, int length, void *font, void *brush, ZuiRectR layoutRect, int flags, int matrix);
    int __stdcall GdipMeasureDriverString(
        void *graphics,
        ZuiText text,
        ZuiInt length,
        void *font,
        ZuiRectR positions,
        INT flags,
        int matrix,
        ZuiRectR boundingBox
    );
    int __stdcall GdipDrawImageI(void *graphics, void *image, int x, int y);
    int __stdcall GdipGraphicsClear(void *graphics, ZuiColor color);
    int __stdcall GdipCreateFontFamilyFromName(ZuiText name, void *fontCollection, void **fontFamily);
    int __stdcall GdipDeleteFontFamily(void *fontFamily);
    int __stdcall GdipSetStringFormatFlags(void *format, int flags);
    int __stdcall GdipCreateFont(void *fontFamily, ZuiReal emSize, int style, int unit, void **font);
    int __stdcall GdipStringFormatGetGenericTypographic(void **StringFormat);
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
ZuiGraphics dgp;//默认图形
/*初始化图形接口*/
ZuiBool ZuiGraphInitialize() {
    GdiplusStartupInput gdiplusStartupInput;
    memset(&gdiplusStartupInput, 0, sizeof(GdiplusStartupInput));
    gdiplusStartupInput.GdiplusVersion = 1;
    GdiplusStartup(&pGdiToken, &gdiplusStartupInput, NULL);//初始化GDI+
    MAKEZRECT(ZeroRectR, 0, 0, 0, 0);
    dgp = ZuiCreateGraphicsInMemory(10, 10);
    return TRUE;
}
/*反初始化图形接口*/
ZuiVoid ZuiGraphUnInitialize() {
    GdiplusShutdown(pGdiToken);
}
/*填充矩形*/
ZEXPORT ZuiVoid ZCALL ZuiDrawFillRect(ZuiGraphics Graphics, ZuiColor Color, ZuiReal Left, ZuiReal Top, ZuiReal Width, ZuiReal Height) {
    void *Brush;
    GdipCreateSolidFill(Color, &Brush);
    GdipFillRectangle(Graphics->graphics, Brush, Left, Top, Width, Height);
    GdipDeleteBrush(Brush);
}
/*画矩形*/
ZEXPORT ZuiVoid ZCALL ZuiDrawRect(ZuiGraphics Graphics, ZuiColor Color, ZuiReal Left, ZuiReal Top, ZuiReal Width, ZuiReal Height, ZuiReal LineWidth) {
    if (Graphics) {
        void *pen;
        GdipCreatePen1(Color, (ZuiReal)LineWidth, 2, &pen);
        GdipDrawRectangle(Graphics->graphics, pen, Left, Top, Width - 1, Height - 1);
        GdipDeletePen(pen);
    }
}
/*画多边形*/
ZEXPORT ZuiVoid ZCALL ZuiDrawPolygon(ZuiGraphics Graphics, ZuiColor Color, ZuiPointR *point, ZuiInt count, ZuiReal LineWidth) {
    void *pen;
    GdipCreatePen1(Color, LineWidth, 2, &pen);
    GdipDrawPolygon(Graphics->graphics, pen, point, count);
    GdipDeletePen(pen);
}
/*画直线*/
ZEXPORT ZuiVoid ZCALL ZuiDrawLine(ZuiGraphics Graphics, ZuiColor Color, ZuiReal x1, ZuiReal y1, ZuiReal x2, ZuiReal y2, ZuiReal LineWidth) {
	void *pen;
	GdipCreatePen1(Color, LineWidth, 2, &pen);
	GdipDrawLine(Graphics->graphics, pen, x1, y1, x2, y2);
	GdipDeletePen(pen);
}
/*测量文本矩形*/
ZEXPORT ZuiVoid ZCALL ZuiMeasureTextSize(ZuiStringFormat StringFormat, _ZuiText String, ZuiSizeR Size)
{
    if (String) {
        if (!StringFormat) { StringFormat = Global_StringFormat; }//使用默认字体
        ZRectR rc = { 0 };
        ZPointR pt = { 0 };
        GdipMeasureDriverString(dgp->graphics, &String, 1, StringFormat->font, &pt, 1, 0, &rc);
        Size->cx = rc.right;
        Size->cy = rc.bottom;
    }
}
/*画文本*/
ZEXPORT ZuiVoid ZCALL ZuiDrawString(ZuiGraphics Graphics, ZuiStringFormat StringFormat, ZuiText String, ZuiInt StrLen, ZPointR Pt[]) {
	if (String) {
		if (!StringFormat) { StringFormat = Global_StringFormat; }//使用默认字体
        GdipDrawDriverString(Graphics->graphics, String, StrLen, StringFormat->font, StringFormat->Brush, Pt, 1, 0);
        return;
	}
}

/*画图像*/
ZEXPORT ZuiVoid ZCALL ZuiDrawImage(ZuiGraphics Graphics, ZuiImage Image, ZuiReal x, ZuiReal y) {
    if (!(Graphics && Image)) {
        return;
    }
    GdipDrawImageI(Graphics->graphics, Image->image, x, y);
}
/*画图像缩放*/
ZEXPORT ZuiVoid ZCALL ZuiDrawImageEx(ZuiGraphics Graphics, ZuiImage Image, ZuiReal x, ZuiReal y, ZuiReal Width, ZuiReal Height, ZuiReal xSrc, ZuiInt ySrc, ZuiInt WidthSrc, ZuiInt HeightSrc, ZuiByte Alpha) {
    if (!(Graphics && Image)) {
        return;
    }
    if (Image->src.left)
        xSrc = Image->src.left;
    if (Image->src.top)
        ySrc = Image->src.top;

    if (Width <= 0) {
        if (Image->src.right)
            Width = Image->src.right - xSrc;
        else
            Width = (ZuiInt)(Image->Width - xSrc);
    }
    if (Height <= 0) {
        if (Image->src.bottom)
            Height = Image->src.bottom - ySrc;
        else
            Height = (ZuiInt)(Image->Height - ySrc);
    }


    if (WidthSrc <= 0 || WidthSrc + xSrc > Image->Width) {
        if (Image->src.right)
            WidthSrc = Image->src.right;
        else
            WidthSrc = (ZuiInt)(Image->Width - xSrc);
    }
    if (HeightSrc <= 0 || HeightSrc + ySrc > Image->Height) {
        if (Image->src.bottom)
            HeightSrc = Image->src.bottom;
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

    
    if (StringStyle & ZTS_BOLD) {
        i = 1;
    }
    else if (StringStyle & ZTS_ITALIC) {
        i += 2;
    }

    GdipCreateFontFamilyFromName(FontName, NULL, &StringFormat->FontFamily);
    GdipCreateFont(StringFormat->FontFamily, FontSize, i, 0, &StringFormat->font);
    StringFormat->FontSize = FontSize;
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

