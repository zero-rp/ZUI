#include "skia.h"
#include "graph.h"
#if PLATFORM_GRAPH_SKIA
#pragma comment(lib, "Gdiplus.lib")
#if defined(__cplusplus)
//图像解码器

#undef min
#undef max
#include <stdint.h>
#include <core\SkCanvas.h>
#include <core\SkBitmap.h>
#include <core\SkRegion.h>

#ifdef __cplusplus
extern "C" {
#endif
    int __stdcall GdipCreateFromHDC(HDC hdc, void** graphics);
    int __stdcall GdipDeleteGraphics(void* graphics);
    int __stdcall GdipDrawImageI(void* graphics, void* image, int x, int y);
    int __stdcall GdipGraphicsClear(void* graphics, ZuiColor color);
    int __stdcall GdipLoadImageFromStream(void* stream, void** image);
    int __stdcall GdipGetImageWidth(void* image, int* width);
    int __stdcall GdipGetImageHeight(void* image, int* height);
    int __stdcall GdipImageGetFrameCount(void* image, char* guid, int* cunt);
    int __stdcall GdipImageSelectActiveFrame(void* image, char* guid, int* index);
    int __stdcall GdipGetPropertyItemSize(void*, int, int*);
    int __stdcall GdipGetPropertyItem(void*, int, int, char*);
    //int __stdcall CLSIDFromString(ZuiText *, char *);
    int __stdcall GdipDisposeImage(void* image);
    int __stdcall GdipDrawImageRectRectI(void* graphics, void* image, int dstx, int dsty, int dstwidth, int dstheight, int srcx, int srcy, int srcwidth, int srcheight, int srcUnit, void* imageAttributes, void* callback, void* callbackData);
    int __stdcall CreateStreamOnHGlobal(HGLOBAL hGlobal, BOOL fDeleteOnRelease, void** ppstm);
#ifdef __cplusplus
}
#endif


extern "C"
{
#endif
#define ARGBTORGBA8(A) (agg::rgba8(((A) >> 16) & 0xFF, ((A) >> 8) & 0xFF, (A) & 0xff, ((A) >> 24) & 0xFF))
#define ARGBTORGBA8P(A) (new agg::rgba8(((A) >> 16) & 0xFF, ((A) >> 8) & 0xFF, (A) & 0xff, ((A) >> 24) & 0xFF))
    /**SKIA图形*/
    struct ZuiSkiaGraphics {
        SkCanvas* SkCanvas;
        SkBitmap* SkBitmap;
    };
    /**SKIA图像*/
    struct ZuiSkiaImage {
        HDC hdc;
        HBITMAP HBitmap;///位图句柄
        void* graphics;
        void* buf;
        void* image;

    };

    /*初始化图形接口*/
    ZuiBool ZuiGraphInitialize() {

        return TRUE;
    }
    /*反初始化图形接口*/
    ZuiVoid ZuiGraphUnInitialize() {

    }

    /*填充矩形*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawFillRect(ZuiGraphics Graphics, ZuiColor Color, ZuiReal Left, ZuiReal Top, ZuiReal Width, ZuiReal Height) {
        if (!Graphics)
            return;
        SkRect rc = { Left, Top, Left + Width, Top + Height };

        SkPaint paint;

        paint.setColor(Color);
        paint.setStyle(SkPaint::kFill_Style);
        Graphics->graphics->SkCanvas->drawRect(rc, paint);
    }
    /*画矩形*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawRect(ZuiGraphics Graphics, ZuiColor Color, ZuiReal Left, ZuiReal Top, ZuiReal Width, ZuiReal Height, ZuiReal LineWidth) {
        if (!Graphics)
            return;
        SkRect rc = { Left, Top, Left + Width, Top + Height };

        SkPaint paint;

        paint.setColor(Color);
        paint.setStyle(SkPaint::kStroke_Style);
        Graphics->graphics->SkCanvas->drawRect(rc, paint);
    }
    //填充三角形
    ZEXPORT ZuiVoid ZCALL ZuiDrawFilltriangle(ZuiGraphics Graphics, ZuiColor Color, ZuiReal x1, ZuiReal y1, ZuiReal x2, ZuiReal y2, ZuiReal x3, ZuiReal y3)
    {
        if (!Graphics)
            return;
    }
    //绘制三角形
    ZEXPORT ZuiVoid ZCALL ZuiDrawtriangle(ZuiGraphics Graphics, ZuiColor Color, ZuiReal x1, ZuiReal y1, ZuiReal x2, ZuiReal y2, ZuiReal x3, ZuiReal y3, ZuiReal LineWidth)
    {
        if (!Graphics)
            return;
    }
    /*画直线*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawLine(ZuiGraphics Graphics, ZuiColor Color, ZuiReal x1, ZuiReal y1, ZuiReal x2, ZuiReal y2, ZuiReal LineWidth)
    {
        if (!Graphics)
            return;
        SkPoint p1 = { x1,y1 };
        SkPoint p2 = { x2,y2 };
        SkPaint paint;

        paint.setColor(Color);
        paint.setStyle(SkPaint::kStroke_Style);
        Graphics->graphics->SkCanvas->drawLine(p1, p2, paint);
    }
    /*画文本(按照计算好的坐标)*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawStringPt(ZuiGraphics Graphics, ZuiFont Font, ZuiColor Color, ZuiText String, ZuiInt StrLens, ZPointR Pt[]) {
        if (!String || !Font || !Graphics)
            return;

    }
    ZEXPORT ZuiVoid ZCALL ZuiDrawString(ZuiGraphics Graphics, ZuiFont Font, ZuiText String, ZuiInt StrLen, ZRectR * Rect, ZuiColor Color, ZuiUInt TextStyle) {
        if (!String || !Font || !Graphics)
            return;

    }
    /*测量文本大小*/
    ZEXPORT ZuiVoid ZCALL ZuiMeasureTextSize(ZuiFont Font, _ZuiText String, ZuiSizeR Size)
    {
        if (!String || !Font)
            return;
    }
    /*画图像缩放*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawImageEx(ZuiGraphics Graphics, ZuiImage Image, ZuiReal x, ZuiReal y, ZuiReal Width, ZuiReal Height, ZuiReal xSrc, ZuiReal ySrc, ZuiReal WidthSrc, ZuiReal HeightSrc, ZuiByte Alpha) {
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


        return;
    }
    /*复制位图*/
    ZEXPORT ZuiVoid ZCALL ZuiAlphaBlendEx(ZuiGraphics Dest, ZuiInt srcX1, ZuiInt srcY1, ZuiInt srcX2, ZuiInt srcY2, ZuiInt dstX, ZuiInt dstY, ZuiGraphics Src, ZuiByte Alpha) {
        if (!Dest || !Src)
            return;


    }
    ZEXPORT ZuiVoid ZCALL ZuiBitBltEx(ZuiGraphics Dest, ZuiInt srcX1, ZuiInt srcY1, ZuiInt srcX2, ZuiInt srcY2, ZuiInt dstX, ZuiInt dstY, ZuiGraphics Src) {
        if (!Dest || !Src)
            return;
    }
    /*清除图形*/
    ZEXPORT ZuiVoid ZCALL ZuiGraphicsClear(ZuiGraphics Graphics, ZuiColor Color) {
        if (!Graphics)
            return;

    }
    /*创建字体*/
    ZEXPORT ZuiFont ZCALL ZuiCreateFont(ZuiText FontName, ZuiReal FontSize, ZuiBool Bold, ZuiBool Italic) {
        int i = 0;
        ZuiFont Font = (ZuiFont)malloc(sizeof(ZFont));
        if (!Font) { return NULL; }
        memset(Font, 0, sizeof(ZFont));

        return Font;
    }
    /*销毁字体*/
    ZEXPORT ZuiVoid ZCALL ZuiDestroyFont(ZuiFont StringFormat) {
        if (!StringFormat)
            return;
    }
    /*创建图形*/
    ZEXPORT ZuiGraphics ZCALL ZuiCreateGraphicsInMemory(ZuiInt Width, ZuiInt Height) {
        ZuiGraphics Graphics = (ZuiGraphics)malloc(sizeof(ZGraphics));
        if (!Graphics) { return NULL; }
        memset(Graphics, 0, sizeof(ZGraphics));
        Graphics->Width = Width;
        Graphics->Height = Height;
        HDC tempdc = GetDC(0);
        Graphics->hdc = CreateCompatibleDC(tempdc);
        ReleaseDC(0, tempdc);
        BITMAPINFO BitmapInfo;
        memset(&BitmapInfo, 0, sizeof(BitmapInfo));
        BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
        BitmapInfo.bmiHeader.biBitCount = 32;
        BitmapInfo.bmiHeader.biWidth = Width;
        BitmapInfo.bmiHeader.biHeight = -Height;
        BitmapInfo.bmiHeader.biPlanes = 1;
        BitmapInfo.bmiHeader.biCompression = BI_RGB;
        Graphics->HBitmap = CreateDIBSection(Graphics->hdc, &BitmapInfo, DIB_RGB_COLORS, &Graphics->Bits, 0, 0);
        if (!Graphics->HBitmap) {
            DeleteDC(Graphics->hdc);
            return NULL;
        }
        HBITMAP OldBitmap = (HBITMAP)SelectObject(Graphics->hdc, (HGDIOBJ)Graphics->HBitmap);

        Graphics->graphics = new ZuiSkiaGraphics();

        Graphics->graphics->SkBitmap = new SkBitmap();
        Graphics->graphics->SkBitmap->setInfo(SkImageInfo::Make(Width, Height, kN32_SkColorType, kPremul_SkAlphaType));
        Graphics->graphics->SkBitmap->setPixels(Graphics->Bits);
        Graphics->graphics->SkCanvas = new SkCanvas(*Graphics->graphics->SkBitmap);

        Graphics->graphics->SkCanvas->clear(ARGB(0, 0, 0, 0));
        return Graphics;
    }
    /*创建一个可复用的图形*/
    ZEXPORT ZuiGraphics ZCALL ZuiCreateGraphics() {
        ZuiGraphics Graphics = (ZuiGraphics)malloc(sizeof(ZGraphics));
        if (!Graphics) { return NULL; }
        memset(Graphics, 0, sizeof(ZGraphics));
        Graphics->graphics = new ZuiSkiaGraphics();



        return Graphics;
    }
    /*附加到一块内存上*/
    ZEXPORT ZuiGraphics ZCALL ZuiCreateGraphicsAttach(ZuiGraphics Graphics, ZuiAny bits, ZuiInt Width, ZuiInt Height, ZuiInt stride) {
        if (!Graphics) { return NULL; }
        if (Graphics->hdc) {
            DeleteDC(Graphics->hdc);
            Graphics->hdc = NULL;
        }
        if (Graphics->HBitmap) {
            DeleteObject(Graphics->HBitmap);
            Graphics->HBitmap = NULL;
        }
        Graphics->Bits = bits;
        Graphics->Width = Width;
        Graphics->Height = Height;

        return Graphics;
    }

    /*销毁图形*/
    ZEXPORT ZuiVoid ZCALL ZuiDestroyGraphics(ZuiGraphics Graphics) {
        if (Graphics) {



            if (Graphics->hdc) {
                DeleteDC(Graphics->hdc);
                Graphics->hdc = NULL;
            }
            if (Graphics->HBitmap) {
                DeleteObject(Graphics->HBitmap);
                Graphics->HBitmap = NULL;
            }
            free(Graphics);
        }
    }

    //设置剪裁区
    ZEXPORT ZuiBool ZCALL ZuiGraphicsPushClipRect(ZuiGraphics Graphics, ZuiRectR box, ZuiInt mode) {
        if (!Graphics)
            return FALSE;
        SkRect rc = { box->left, box->top, box->right, box->bottom };
        Graphics->graphics->SkCanvas->save();
        Graphics->graphics->SkCanvas->clipRect(rc, SkRegion::kReplace_Op);
        return TRUE;
    }
    //ZEXPORT ZuiVoid ZCALL PushClipRegion(IRegion* pRegion, ZuiInt mode){}
    //
    ZEXPORT ZuiBool ZCALL ZuiGraphicsPopClip(ZuiGraphics Graphics) {
        if (!Graphics)
            return FALSE;
        Graphics->graphics->SkCanvas->restore();
        return TRUE;
    }
    //
    ZEXPORT ZuiBool ZCALL ZuiGraphicsExcludeClipRect(ZuiGraphics Graphics, ZuiRectR box) {
        if (!Graphics)
            return FALSE;
        SkRect rc = { box->left, box->top, box->right, box->bottom };
        Graphics->graphics->SkCanvas->clipRect(rc, SkRegion::kDifference_Op);
        return TRUE;
    }
    //
    ZEXPORT ZuiBool ZCALL ZuiGraphicsIntersectClipRect(ZuiGraphics Graphics, ZuiRectR box) {
        if (!Graphics)
            return FALSE;
        SkRect rc = { box->left, box->top, box->right, box->bottom };
        Graphics->graphics->SkCanvas->clipRect(rc, SkRegion::kIntersect_Op);
        return TRUE;
    }
    //
    ZEXPORT ZuiBool ZCALL ZuiGraphicsSaveClip(ZuiGraphics Graphics, ZuiInt * pnState) {
        if (!Graphics)
            return FALSE;
        int nState = Graphics->graphics->SkCanvas->save();
        if (pnState)* pnState = nState;
        return TRUE;
    }
    //
    ZEXPORT ZuiBool ZCALL ZuiGraphicsRestoreClip(ZuiGraphics Graphics, ZuiInt nState) {
        if (!Graphics)
            return FALSE;
        Graphics->graphics->SkCanvas->restoreToCount(nState);
        return TRUE;
    }
    //获取剪裁区
    //ZEXPORT ZuiVoid ZCALL GetClipRegion(IRegion** ppRegion){
    //}
    //
    ZEXPORT ZuiBool ZCALL ZuiGraphicsGetClipBox(ZuiGraphics Graphics, ZuiRectR box) {
        if (!Graphics)
            return FALSE;
        SkIRect skrc;
        Graphics->graphics->SkCanvas->getDeviceClipBounds(&skrc);
        box->left = skrc.fLeft;
        box->top = skrc.fTop;
        box->right = skrc.fRight;
        box->bottom = skrc.fBottom;
        return TRUE;
    }




    /*加载图像自内存*/
    ZEXPORT ZuiImage ZCALL ZuiLoadImageFromBinary(ZuiAny buf, ZuiInt len) {
        ZuiImage Image = (ZuiImage)malloc(sizeof(ZImage));
        if (!Image) { return NULL; }
        memset(Image, 0, sizeof(ZImage));
        Image->image = new ZuiSkiaImage();
        if (!Image->image) { free(Image); return NULL; }
        HGLOBAL hMem = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, len);
        void* str;
        CreateStreamOnHGlobal(hMem, 0, &str);
        void* mem = GlobalLock(hMem);
        memcpy(mem, buf, len);
        GlobalUnlock(hMem);
        GdipLoadImageFromStream(str, &Image->image->image);
        GdipGetImageHeight(Image->image->image, (int*)& Image->Height);
        GdipGetImageWidth(Image->image->image, (int*)& Image->Width);

        //得到数据
        HDC tempdc = GetDC(0);
        Image->image->hdc = CreateCompatibleDC(tempdc);
        ReleaseDC(0, tempdc);
        BITMAPINFO BitmapInfo;
        memset(&BitmapInfo, 0, sizeof(BitmapInfo));
        BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
        BitmapInfo.bmiHeader.biBitCount = 32;
        BitmapInfo.bmiHeader.biWidth = Image->Width;
        BitmapInfo.bmiHeader.biHeight = Image->Height;
        BitmapInfo.bmiHeader.biPlanes = 1;
        BitmapInfo.bmiHeader.biCompression = BI_RGB;
        Image->image->HBitmap = CreateDIBSection(Image->image->hdc, &BitmapInfo, DIB_RGB_COLORS, &Image->image->buf, 0, 0);
        if (!Image->image->HBitmap) {
            DeleteDC(Image->image->hdc);
            GdipDisposeImage(Image->image->image);
            delete Image->image;
            free(Image);
            return NULL;
        }
        SelectObject(Image->image->hdc, (HGDIOBJ)Image->image->HBitmap);


        GdipCreateFromHDC(Image->image->hdc, &Image->image->graphics);
        //把图片画到DC上
        GdipDrawImageI(Image->image->graphics, Image->image->image, 0, 0);


        return Image;
    }
    /*销毁图像*/
    ZEXPORT ZuiVoid ZCALL ZuiDestroyImage(ZuiImage Image) {
        if (Image) {
            GdipDeleteGraphics(Image->image->graphics);
            DeleteDC(Image->image->hdc);
            DeleteObject(Image->image->HBitmap);
            GdipDisposeImage(Image->image->image);



            delete Image->image;
            free(Image);
        }
    }

#ifdef __cplusplus
}
#endif

#endif // PLATFORM_GRAPH_SKIA
