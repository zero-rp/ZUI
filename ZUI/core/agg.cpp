#include <ZUI.h>
#if defined(__cplusplus)
#include "agg\include\agg_rendering_buffer.h"//内存管理器
#include "agg\include\agg_pixfmt_rgba.h"//颜色空间管理
#include "agg\include\agg_renderer_base.h"//基础渲染器
#include "agg\include\agg_renderer_scanline.h"//扫描线渲染器
#include "agg\include\agg_rasterizer_scanline_aa.h"//光栅化器
#include "agg\include\agg_scanline_u.h"//扫描线
#include "agg\include\agg_path_storage.h"//路径
#include "agg\include\agg_color_rgba.h"
#include "agg\include\agg_renderer_primitives.h"
#include "agg\include\agg_span_allocator.h"//线段分配器
#include "agg\include\agg_span_interpolator_linear.h"//插值器
#include "agg\include\agg_trans_affine.h"//变换矩阵
#include "agg\include\agg_span_image_filter_rgba.h"
#include "agg\include\agg_image_accessors.h"
//图像线段生成器
#include "agg\include\agg_math_stroke.h"
#include "agg\include\agg_conv_stroke.h"
#include "agg\include\agg_font_cache_manager.h"
#include "agg\agg_font_win32_tt.h"

#include <vector>
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
	int __stdcall GdipCreateFromHDC(HDC hdc, void **graphics);
	int __stdcall GdipDeleteGraphics(void *graphics);
	int __stdcall GdipDrawImageI(void *graphics, void *image, int x, int y);
	int __stdcall GdipGraphicsClear(void *graphics, ZuiColor color);
	int __stdcall GdipLoadImageFromStream(void* stream, void **image);
	int __stdcall GdipGetImageWidth(void *image, int *width);
	int __stdcall GdipGetImageHeight(void *image, int *height);
	int __stdcall GdipImageGetFrameCount(void *image, char *guid, int *cunt);
	int __stdcall GdipImageSelectActiveFrame(void *image, char *guid, int *index);
	int __stdcall GdipGetPropertyItemSize(void *, int, int*);
	int __stdcall GdipGetPropertyItem(void *, int, int, char*);
	//int __stdcall CLSIDFromString(ZuiText *, char *);
	int __stdcall GdipDisposeImage(void *image);
	int __stdcall GdipDrawImageRectRectI(void *graphics, void *image, int dstx, int dsty, int dstwidth, int dstheight, int srcx, int srcy, int srcwidth, int srcheight, int srcUnit, void* imageAttributes, void *callback, void * callbackData);
	//int __stdcall CreateStreamOnHGlobal(HGLOBAL hGlobal, BOOL fDeleteOnRelease, void **ppstm);
#ifdef __cplusplus
}
#endif


extern "C"
{
#endif
#define ARGBTORGBA8(A) (agg::rgba8(((A) >> 16) & 0xFF, ((A) >> 8) & 0xFF, (A) & 0xff, ((A) >> 24) & 0xFF))
#define ARGBTORGBA8P(A) (new agg::rgba8(((A) >> 16) & 0xFF, ((A) >> 8) & 0xFF, (A) & 0xff, ((A) >> 24) & 0xFF))
/**AGG图形*/
struct ZuiAggGraphics {
	agg::rendering_buffer *rbuf;		//内存管理器
	agg::pixfmt_bgra32 *pixf;			//颜色管理器
	agg::renderer_base<agg::pixfmt_bgra32> *renb;//基础渲染器
	agg::renderer_scanline_aa_solid<agg::renderer_base<agg::pixfmt_bgra32>> *rensl;//高级渲染器
	agg::renderer_primitives<agg::renderer_base<agg::pixfmt_bgra32>> *rendp;//基础操作,矩形,线段什么的
	agg::path_storage ps;//路径存储
	agg::rasterizer_scanline_aa<> ras;//光栅化
};
struct ZuiAggImage {
	HDC hdc;
	HBITMAP HBitmap;///位图句柄
	void *graphics;
	void *buf;
	void *image;
	agg::rendering_buffer *rbuf;		//内存管理器
	agg::pixfmt_bgra32 *pixf;			//颜色管理器
};
struct ZuiAggFont{
	HDC dc;
	agg::font_engine_win32_tt_int16 *font;//字体
	agg::font_cache_manager<agg::font_engine_win32_tt_int16> *font_manager;//字体管理器  
	agg::font_engine_win32_tt_int16::path_adaptor_type *vs;// 顶点源
	agg::conv_curve<agg::font_engine_win32_tt_int16::path_adaptor_type> *ccvs;//转换后的顶点
	agg::rgba8 *color;
};

ZRectR ZeroRectR;//零坐标矩形
agg::scanline_u8 sl;//扫描线
				 
/*初始化图形接口*/
ZuiBool ZuiGraphInitialize(){
	GdiplusStartupInput gdiplusStartupInput;
	ZuiInt pGdiToken;
	memset(&gdiplusStartupInput, 0, sizeof(GdiplusStartupInput));
	gdiplusStartupInput.GdiplusVersion = 1;
	GdiplusStartup(&pGdiToken, &gdiplusStartupInput, NULL);//初始化GDI+
	MAKEZRECT(ZeroRectR, 0, 0, 0, 0);
	return TRUE;
}
/*填充矩形*/
ZEXPORT ZuiVoid ZCALL ZuiDrawFillRect(ZuiGraphics Graphics, ZuiColor Color, ZuiInt Left, ZuiInt Top, ZuiInt Width, ZuiInt Height) {
	if (Graphics)
	{
		Graphics->graphics->rendp->fill_color(ARGBTORGBA8(Color));
		Graphics->graphics->rendp->solid_rectangle(Left, Top, Left + Width, Top + Height);
	}
}
/*画矩形*/
ZEXPORT ZuiVoid ZCALL ZuiDrawRect(ZuiGraphics Graphics, ZuiColor Color, ZuiInt Left, ZuiInt Top, ZuiInt Width, ZuiInt Height, ZuiInt LineWidth) {
	if (Graphics){
		Graphics->graphics->rendp->line_color(ARGBTORGBA8(Color));
		Graphics->graphics->rendp->rectangle(Left, Top, Width + Left, Top + Height);
	}
}
/*画直线*/
ZEXPORT ZuiVoid ZCALL ZuiDrawLine(ZuiGraphics Graphics, ZuiColor Color, ZuiInt x1, ZuiInt y1, ZuiInt x2, ZuiInt y2, ZuiInt LineWidth)
{
	if (Graphics) {
		Graphics->graphics->ps.move_to(x1, y1);
		Graphics->graphics->ps.line_to(x2, y2);
				
		agg::conv_stroke<agg::path_storage> stroke(Graphics->graphics->ps);
		//stroke.width(1);
		Graphics->graphics->ras.add_path(stroke);

		Graphics->graphics->rensl->color(ARGBTORGBA8(Color));
		agg::render_scanlines(Graphics->graphics->ras, sl, *Graphics->graphics->rensl);
		//重置
		Graphics->graphics->ps.remove_all();
		Graphics->graphics->ras.reset();
	}
}


/*画文本*/
ZEXPORT ZuiVoid ZCALL ZuiDrawString(ZuiGraphics Graphics, ZuiStringFormat StringFormat, ZuiText String, ZuiRect Rect) {
	if (String) {
		// 字体串  
		wchar_t *s = String;

		// 字符输出的位置  
		double x = Rect->Left, y = Rect->Top + Rect->Height / 2;
		for (; *s; s++) {      // 让字体引擎准备好字体数据 
			const agg::glyph_cache* glyph = StringFormat->font->font_manager->glyph(*s);
			if (glyph) {
				StringFormat->font->vs->init(glyph->data, glyph->data_size, x, y);
				Graphics->graphics->ras.add_path(*StringFormat->font->ccvs);
				agg::render_scanlines_aa_solid(Graphics->graphics->ras, sl, *Graphics->graphics->renb, *StringFormat->font->color);
				// 前进  
				x += glyph->advance_x;
				y += glyph->advance_y;
				Graphics->graphics->ras.reset();
			}
		}
	}
}
/*测量文本矩形*/
ZEXPORT ZuiVoid ZCALL ZuiMeasureStringRect(ZuiGraphics Graphics, ZuiStringFormat StringFormat, ZuiText String, ZuiRectR Rect,ZuiRectR LRect)
{
	if (String && Graphics){
		// 字体串  
		wchar_t *s = String;
		double x = 0, y = 0;
		for (; *s; s++) {      // 让字体引擎准备好字体数据 
			const agg::glyph_cache* glyph = StringFormat->font->font_manager->glyph(*s);
			if (glyph) {
				// 前进  
				x += glyph->advance_x;
				y += glyph->advance_y;
			}
		}
		Rect->Height = y;
		Rect->Width = x;
		Rect->Left = 0;
		Rect->Top = 0;
	}
}
/*画圆角矩形*/
ZEXPORT ZuiVoid ZCALL ZuiDrawRoundRect(ZuiGraphics Graphics, ZuiColor Color, ZuiInt x, ZuiInt y, ZuiInt Width, ZuiInt Height, ZuiInt LineWidth, ZuiReal Round) {

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

	//目标路径
	agg::path_storage p2;
	p2.move_to(x, y);
	p2.line_to(x + Width, y);
	p2.line_to(x + Width, y + Height);
	p2.line_to(x, y + Height);
	p2.close_polygon();
	Graphics->graphics->ras.add_path(p2);

	typedef agg::image_accessor_clip<agg::pixfmt_bgra32> img_source_type;//图像访问器 图像以外的地方用指定颜色填充
	typedef agg::span_image_filter_rgba_bilinear<img_source_type, agg::span_interpolator_linear<>> span_gen_type;//线段生成器
	typedef agg::renderer_scanline_aa<agg::renderer_base<agg::pixfmt_bgra32>, agg::span_allocator<agg::rgba8>, span_gen_type> renderer_type;//渲染器
	agg::trans_affine mtx_Work;
	mtx_Work.translate(x, y);
	mtx_Work.premultiply(agg::trans_affine_scaling(Width / float(WidthSrc), Height / float(HeightSrc)));//缩放
	mtx_Work.invert();
	agg::span_interpolator_linear<> interpolator(mtx_Work); //插值器
	{
		img_source_type img_src(*Image->image->pixf, agg::rgba(0, 0.4, 0, 0.5), xSrc, ySrc, HeightSrc, WidthSrc);
		span_gen_type sg(img_src, interpolator);
		agg::span_allocator<agg::rgba8> span_allocator;
		renderer_type ri(*Graphics->graphics->renb, span_allocator, sg);
		agg::render_scanlines(Graphics->graphics->ras, sl, ri);
	}

	//重置
	Graphics->graphics->ras.reset();

	return;
}
/*复制位图*/
ZEXPORT ZuiVoid ZCALL ZuiAlphaBlend(ZuiGraphics Dest, ZuiInt x, ZuiInt y, ZuiInt Width, ZuiInt Height, ZuiGraphics Src, ZuiInt xSrc, ZuiInt ySrc, ZuiByte Alpha) {
	if (Dest && Src){

	}
}
/*清除图形*/
ZEXPORT ZuiVoid ZCALL ZuiGraphicsClear(ZuiGraphics Graphics, ZuiColor Color) {
	if (Graphics)
	{

	}
}
/*创建字体格式*/
ZEXPORT ZuiStringFormat ZCALL ZuiCreateStringFormat(ZuiText FontName, ZuiReal FontSize, ZuiColor TextColor, ZuiColor ShadowColor, ZuiInt StringStyle) {
	int i = 0;
	ZuiStringFormat StringFormat = (ZuiStringFormat)malloc(sizeof(ZStringFromat));
	if (!StringFormat){ return NULL; }
	memset(StringFormat, 0, sizeof(ZStringFromat));
	StringFormat->font = new ZuiAggFont;
	StringFormat->font->dc = ::GetDC(::GetActiveWindow());
	//创建字体引擎
	StringFormat->font->font = new agg::font_engine_win32_tt_int16(StringFormat->font->dc);
	
	StringFormat->font->font->height(FontSize);
	StringFormat->font->font->flip_y(true);
	StringFormat->font->font->hinting(true);

	// 注意后面的 glyph_rendering ren_type 参数  
	StringFormat->font->font->create_font(FontName, agg::glyph_ren_outline);

	StringFormat->font->font_manager = new agg::font_cache_manager<agg::font_engine_win32_tt_int16>(*StringFormat->font->font);
	// 顶点源
	StringFormat->font->vs = new agg::font_engine_win32_tt_int16::path_adaptor_type();
	// 注意这里，使用 conv_curve 转换  
	StringFormat->font->ccvs = new agg::conv_curve<agg::font_engine_win32_tt_int16::path_adaptor_type>(*StringFormat->font->vs);

	StringFormat->font->color = ARGBTORGBA8P(TextColor);
	return StringFormat;
}
/*销毁字体格式*/
ZEXPORT ZuiVoid ZCALL ZuiDestroyStringFormat(ZuiStringFormat StringFormat) {
	if (StringFormat){
		delete StringFormat->font->ccvs;
		delete StringFormat->font->vs;
		delete StringFormat->font->font;
		delete StringFormat->font->font_manager;
		delete StringFormat->font;
		free(StringFormat);
	}
}
/*创建图形*/
ZEXPORT ZuiGraphics ZCALL ZuiCreateGraphicsInMemory(ZuiInt Width, ZuiInt Height) {
	ZuiGraphics Graphics = (ZuiGraphics)malloc(sizeof(ZGraphics));
	if (!Graphics){ return NULL; }
	memset(Graphics, 0, sizeof(ZGraphics));
	Graphics->Width = Width;
	Graphics->Height = Height;
	HDC tempdc = GetDC(0);
	Graphics->hdc = CreateCompatibleDC(tempdc);
	ReleaseDC(0,tempdc);
	BITMAPINFO BitmapInfo;
	memset(&BitmapInfo, 0, sizeof(BitmapInfo));
	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biWidth = Width;
	BitmapInfo.bmiHeader.biHeight = Height;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;
	Graphics->HBitmap = CreateDIBSection(Graphics->hdc, &BitmapInfo, DIB_RGB_COLORS, &Graphics->Bits, 0, 0);
	if (!Graphics->HBitmap) {
		DeleteDC(Graphics->hdc);
		return NULL;
	}
	HBITMAP OldBitmap = (HBITMAP)SelectObject(Graphics->hdc, (HGDIOBJ)Graphics->HBitmap);

	Graphics->graphics = new ZuiAggGraphics();

	Graphics->graphics->rbuf = new agg::rendering_buffer((agg::int8u *)Graphics->Bits, Width, Height, -Width * 4);
	Graphics->graphics->pixf = new agg::pixfmt_bgra32(*Graphics->graphics->rbuf);
	Graphics->graphics->renb = new agg::renderer_base<agg::pixfmt_bgra32>(*Graphics->graphics->pixf);


	Graphics->graphics->rensl = new agg::renderer_scanline_aa_solid<agg::renderer_base<agg::pixfmt_bgra32>>(*Graphics->graphics->renb);
	Graphics->graphics->rendp = new agg::renderer_primitives<agg::renderer_base<agg::pixfmt_bgra32>>(*Graphics->graphics->renb);

	return Graphics;
}
/*销毁图形*/
ZEXPORT ZuiVoid ZCALL ZuiDestroyGraphics(ZuiGraphics Graphics) {
	if (Graphics){
		
		delete Graphics->graphics->rbuf;
		delete Graphics->graphics->pixf;
		delete Graphics->graphics->renb;
		delete Graphics->graphics->rensl;
		delete Graphics->graphics->rendp;
		delete (ZuiAggGraphics*)Graphics->graphics;
		DeleteDC(Graphics->hdc);
		DeleteObject(Graphics->HBitmap);
		free(Graphics);
	}
}
/*创建区域*/
ZEXPORT ZuiRegion ZCALL ZuiCreateRegion() {
	ZuiRegion region = (ZuiRegion)malloc(sizeof(ZRegion));
	if (!region){ return NULL; }
	memset(region, 0, sizeof(ZRegion));
	


	return region;
}
ZEXPORT ZuiRegion ZCALL ZuiCreateRegionRect(ZuiRect layoutRect) {
	ZuiRegion region = (ZuiRegion)malloc(sizeof(ZRegion));
	if (!region){ return NULL; }
	


	return region;
}
ZEXPORT ZuiVoid ZCALL ZuiDestroyRegion(ZuiRegion region) {
	if (region){
		


		free(region);
	}
}

ZEXPORT ZuiBool ZCALL ZuiGraphicsSetClipRegion(ZuiGraphics Graphics, ZuiRegion region, ZuiInt mod){
	if (Graphics && region){
		
	}
	return FALSE;
}
ZEXPORT ZuiBool ZCALL ZuiGraphicsGetClipRegion(ZuiGraphics Graphics, ZuiRegion region) {
	if (Graphics && region) {
		
	}
	return FALSE;
}
ZEXPORT ZuiBool ZCALL ZuiGraphicsResetClip(ZuiGraphics Graphics){
	if (Graphics){
		
	}
	return FALSE;
}


/*加载图像自内存*/
ZEXPORT ZuiImage ZCALL ZuiLoadImageFromBinary(ZuiAny buf, ZuiInt len) {
		ZuiImage Image = (ZuiImage)malloc(sizeof(ZImage));
		if (!Image){ return NULL; }
		memset(Image, 0, sizeof(ZImage));
		Image->image = new ZuiAggImage();
		if (!Image->image) { return NULL; }
		HGLOBAL hMem = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, len);
		void *str;
		CreateStreamOnHGlobal(hMem, 0, (LPSTREAM *)&str);
		void * mem = GlobalLock(hMem);
		memcpy(mem, buf, len);
		GlobalUnlock(hMem);
		GdipLoadImageFromStream(str, &Image->image->image);
		GdipGetImageHeight(Image->image->image, &Image->Height);
		GdipGetImageWidth(Image->image->image, &Image->Width);

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

		
		//创建AGG对象
		Image->image->rbuf = new agg::rendering_buffer((agg::int8u *)Image->image->buf, Image->Width, Image->Height, -Image->Width * 4);
		Image->image->pixf = new agg::pixfmt_bgra32(*Image->image->rbuf);

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