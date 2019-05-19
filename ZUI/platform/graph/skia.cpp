#include "skia.h"
#include "graph.h"
#if PLATFORM_GRAPH_SKIA
#if defined(__cplusplus)
//图像解码器

#undef min
#undef max
#include <stdint.h>
#include <core\SkCanvas.h>
#include <core\SkBitmap.h>
#include <core\SkRegion.h>
#include <core\SkPath.h>
#include <core\SkPicture.h>
#include <core\SkImage.h>
#include <core\SkTypeface.h>
#include <core\SkFont.h>

extern "C"
{
#endif

#include <core/function.h>

    /**SKIA图形*/
    struct ZuiSkiaGraphics {
        SkCanvas* SkCanvas;
        SkBitmap* SkBitmap;
    };
    /**SKIA图像*/
    struct ZuiSkiaImage {
        void* buf;
        sk_sp<SkImage> image;
        sk_sp<SkData> data;
    };
    /**字体*/
    struct ZuiSkiaFont {
        sk_sp<SkTypeface> SkFontType;   //定义字体
        SkFont *font;
    };

    /*初始化图形接口*/
    ZuiBool ZuiGraphInitialize() {

        return TRUE;
    }
    /*反初始化图形接口*/
    ZuiVoid ZuiGraphUnInitialize() {

    }
    //---------------------------------------------------图形
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

        Graphics->graphics = new ZuiSkiaGraphics();

        Graphics->graphics->SkBitmap = new SkBitmap();
        Graphics->graphics->SkBitmap->setInfo(SkImageInfo::Make(Width, Height, kN32_SkColorType, kPremul_SkAlphaType));
        Graphics->graphics->SkBitmap->setPixels(Graphics->Bits);
        Graphics->graphics->SkCanvas = new SkCanvas(*Graphics->graphics->SkBitmap);

        Graphics->graphics->SkCanvas->clear(ARGB(0, 0, 0, 0));
        return Graphics;
    }
    /*销毁图形*/
    ZEXPORT ZuiVoid ZCALL ZuiDestroyGraphics(ZuiGraphics Graphics) {
        if (Graphics) {
            if (Graphics->graphics) {
                if (Graphics->graphics->SkCanvas)
                    delete Graphics->graphics->SkCanvas;
                if (Graphics->graphics->SkBitmap)
                    delete Graphics->graphics->SkBitmap;
                delete Graphics->graphics;
            }
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
    //-----------------------绘制
    /*填充矩形*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawFillRect(ZuiGraphics Graphics, ZuiColor Color, ZuiReal Left, ZuiReal Top, ZuiReal Right, ZuiReal Bottom) {
        if (!Graphics)
            return;
        SkRect rc = { Left, Top, Right, Bottom };

        SkPaint paint;

        paint.setColor(Color);
        paint.setStyle(SkPaint::kFill_Style);
        Graphics->graphics->SkCanvas->drawRect(rc, paint);
    }
    /*画矩形*/
    ZEXPORT ZuiVoid ZCALL ZuiDrawRect(ZuiGraphics Graphics, ZuiColor Color, ZuiReal Left, ZuiReal Top, ZuiReal Right, ZuiReal Bottom, ZuiReal LineWidth) {
        if (!Graphics)
            return;
        SkRect rc = { Left, Top, Right, Bottom };

        SkPaint paint;

        paint.setColor(Color);
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(LineWidth);
        Graphics->graphics->SkCanvas->drawRect(rc, paint);
    }
    //填充三角形
    ZEXPORT ZuiVoid ZCALL ZuiDrawFilltriangle(ZuiGraphics Graphics, ZuiColor Color, ZuiReal x1, ZuiReal y1, ZuiReal x2, ZuiReal y2, ZuiReal x3, ZuiReal y3)
    {
        if (!Graphics)
            return;
        SkPath     skPath;
        skPath.moveTo(SkPoint::Make(x1, y1));
        skPath.lineTo(SkPoint::Make(x2, y2));
        skPath.lineTo(SkPoint::Make(x3, y3));
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setColor(Color);
        paint.setStyle(SkPaint::kFill_Style);
        Graphics->graphics->SkCanvas->drawPath(skPath, paint);
    }
    //绘制三角形
    ZEXPORT ZuiVoid ZCALL ZuiDrawtriangle(ZuiGraphics Graphics, ZuiColor Color, ZuiReal x1, ZuiReal y1, ZuiReal x2, ZuiReal y2, ZuiReal x3, ZuiReal y3, ZuiReal LineWidth)
    {
        if (!Graphics)
            return;
        SkPath     skPath;
        skPath.moveTo(SkPoint::Make(x1, y1));
        skPath.lineTo(SkPoint::Make(x2, y2));
        skPath.lineTo(SkPoint::Make(x3, y3));
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setColor(Color);
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(LineWidth);
        Graphics->graphics->SkCanvas->drawPath(skPath, paint);
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
        SkPaint paint;

        paint.setColor(Color);
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::kStrokeAndFill_Style);

        Graphics->graphics->SkCanvas->drawSimpleText(String, StrLen * 2, SkTextEncoding::kUTF16, Rect->left,Rect->bottom, *Font->font->font, paint);
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

        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::kFill_Style);
        paint.setColor(SK_ColorBLACK);
        SkRect src = { xSrc, ySrc, xSrc + WidthSrc, ySrc + HeightSrc };
        SkRect dst = { x, y, x + Width, y + Height };
        Graphics->graphics->SkCanvas->drawImage(Image->image->image, x, y);

        return;
    }
    /*复制位图*/
    ZEXPORT ZuiVoid ZCALL ZuiAlphaBlendEx(ZuiGraphics Dest, ZuiInt srcX1, ZuiInt srcY1, ZuiInt srcX2, ZuiInt srcY2, ZuiInt dstX, ZuiInt dstY, ZuiGraphics Src, ZuiByte Alpha) {
        if (!Dest || !Src)
            return;
        SkPaint paint;
        if (Alpha != 0xFF) paint.setAlpha(Alpha);
        paint.setStyle(SkPaint::kFill_Style);
        SkIRect isrc = { srcX1,srcY1,srcX2,srcY2 };
        SkRect skrc = { dstX , dstY , dstX + (srcX2 - srcX1), dstY + (srcY2 - srcY1) };
        Dest->graphics->SkCanvas->drawBitmapRect(*Src->graphics->SkBitmap, isrc, skrc, &paint);
    }
    ZEXPORT ZuiVoid ZCALL ZuiBitBltEx(ZuiGraphics Dest, ZuiInt srcX1, ZuiInt srcY1, ZuiInt srcX2, ZuiInt srcY2, ZuiInt dstX, ZuiInt dstY, ZuiGraphics Src) {
        if (!Dest || !Src)
            return;
        SkPaint paint;
        paint.setStyle(SkPaint::kFill_Style);
        SkIRect isrc = { srcX1,srcY1,srcX2,srcY2 };
        SkRect skrc = { dstX , dstY , dstX + (srcX2 - srcX1), dstY + (srcY2 - srcY1) };
        Dest->graphics->SkCanvas->drawBitmapRect(*Src->graphics->SkBitmap, isrc, skrc, &paint);
    }
    /*清除图形*/
    ZEXPORT ZuiVoid ZCALL ZuiGraphicsClear(ZuiGraphics Graphics, ZuiColor Color) {
        if (!Graphics)
            return;
        Graphics->graphics->SkCanvas->clear(Color);
    }
    //---------------------------------------------------字体
    /*创建字体*/
    ZEXPORT ZuiFont ZCALL ZuiCreateFont(ZuiText FontName, ZuiReal FontSize, ZuiBool Bold, ZuiBool Italic) {
        int i = 0;
        ZuiFont Font = (ZuiFont)malloc(sizeof(ZFont));
        if (!Font) { return NULL; }
        memset(Font, 0, sizeof(ZFont));
        Font->font = new ZuiSkiaFont;
        int weight = 0;
        int width = 0;
        if (Bold) width |= SkFontStyle::kBold_Weight;
        SkFontStyle style(weight, width, Italic ? SkFontStyle::kItalic_Slant : SkFontStyle::kUpright_Slant);
        Font->font->SkFontType = SkTypeface::MakeFromName("微软雅黑", style);
        Font->font->font = new SkFont(Font->font->SkFontType);
        Font->font->font->setSize(FontSize);
        return Font;
    }
    /*销毁字体*/
    ZEXPORT ZuiVoid ZCALL ZuiDestroyFont(ZuiFont StringFormat) {
        if (!StringFormat)
            return;
        delete StringFormat->font->font;
        delete StringFormat->font;
        free(StringFormat);
    }
    //---------------------------------------------------路径




    //---------------------------------------------------剪裁区
    /*压入剪裁区*/
    ZEXPORT ZuiBool ZCALL ZuiGraphicsPushClipRect(ZuiGraphics Graphics, ZuiRectR box, ZuiInt mode) {
        if (!Graphics)
            return FALSE;
        SkRect rc = { box->left, box->top, box->right, box->bottom };
        Graphics->graphics->SkCanvas->save();
        Graphics->graphics->SkCanvas->clipRect(rc, SkRegion::kReplace_Op);
        return TRUE;
    }
    //ZEXPORT ZuiVoid ZCALL PushClipRegion(IRegion* pRegion, ZuiInt mode){}
    /*弹出剪裁区*/
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
    /*保存剪裁区*/
    ZEXPORT ZuiBool ZCALL ZuiGraphicsSaveClip(ZuiGraphics Graphics, ZuiInt * pnState) {
        if (!Graphics)
            return FALSE;
        int nState = Graphics->graphics->SkCanvas->save();
        if (pnState)* pnState = nState;
        return TRUE;
    }
    /*恢复剪裁区*/
    ZEXPORT ZuiBool ZCALL ZuiGraphicsRestoreClip(ZuiGraphics Graphics, ZuiInt nState) {
        if (!Graphics)
            return FALSE;
        Graphics->graphics->SkCanvas->restoreToCount(nState);
        return TRUE;
    }
    //获取剪裁区
    //ZEXPORT ZuiVoid ZCALL GetClipRegion(IRegion** ppRegion){
    //}
    /*获取当前剪裁区*/
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


    //---------------------------------------------------图像
    /*加载图像自内存*/
    ZEXPORT ZuiImage ZCALL ZuiLoadImageFromBinary(ZuiAny buf, ZuiInt len) {
        ZuiImage Image = (ZuiImage)malloc(sizeof(ZImage));
        if (!Image) { return NULL; }
        memset(Image, 0, sizeof(ZImage));
        Image->image = new ZuiSkiaImage();
        if (!Image->image) { free(Image); return NULL; }
        Image->image->buf = malloc(len);
        if (!Image->image->buf) { delete Image->image; free(Image); return NULL; }
        memcpy(Image->image->buf, buf, len);
        Image->image->data = SkData::MakeFromMalloc(Image->image->buf, len);
        Image->image->image = SkImage::MakeFromEncoded(Image->image->data);
        Image->Height = Image->image->image->height();
        Image->Width = Image->image->image->width();
        return Image;
    }
    /*销毁图像*/
    ZEXPORT ZuiVoid ZCALL ZuiDestroyImage(ZuiImage Image) {
        if (Image) {
            if (Image->image->buf)
                free(Image->image->buf);
            delete Image->image;
            free(Image);
        }
    }

#ifdef __cplusplus
}
#endif

#endif // PLATFORM_GRAPH_SKIA
