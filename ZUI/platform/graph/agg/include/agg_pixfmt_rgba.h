//----------------------------------------------------------------------------
// Anti-Grain Geometry (AGG) - Version 2.5
// A high quality rendering engine for C++
// Copyright (C) 2002-2006 Maxim Shemanarev
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://antigrain.com
// 
// AGG is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// AGG is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with AGG; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA 02110-1301, USA.
//----------------------------------------------------------------------------
//
// Adaptation for high precision colors has been sponsored by 
// Liberty Technology Systems, Inc., visit http://lib-sys.com
//
// Liberty Technology Systems, Inc. is the provider of
// PostScript and PDF technology for software developers.
// 
//----------------------------------------------------------------------------

#ifndef AGG_PIXFMT_RGBA_INCLUDED
#define AGG_PIXFMT_RGBA_INCLUDED

#include <string.h>
#include <math.h>
#include "agg_basics.h"
#include "agg_color_rgba.h"
#include "agg_rendering_buffer.h"

namespace agg
{
    //=============================================================blender_rgba
    template<class ColorT, class Order> struct blender_rgba
    {
        typedef ColorT color_type;
        typedef Order order_type;
        typedef typename color_type::value_type value_type;
        typedef typename color_type::calc_type calc_type;
        enum base_scale_e 
        { 
            base_shift = color_type::base_shift,
            base_mask  = color_type::base_mask
        };

        //--------------------------------------------------------------------
        static AGG_INLINE void blend_pix(value_type* p, 
                                         unsigned cr, unsigned cg, unsigned cb,
                                         unsigned alpha, 
                                         unsigned cover=0)
        {
            calc_type r = p[Order::R];
            calc_type g = p[Order::G];
            calc_type b = p[Order::B];
            calc_type a = p[Order::A];
            p[Order::R] = (value_type)(((cr - r) * alpha + (r << base_shift)) >> base_shift);
            p[Order::G] = (value_type)(((cg - g) * alpha + (g << base_shift)) >> base_shift);
            p[Order::B] = (value_type)(((cb - b) * alpha + (b << base_shift)) >> base_shift);
            p[Order::A] = (value_type)((alpha + a) - ((alpha * a + base_mask) >> base_shift));
        }
    };


    //===============================================copy_or_blend_rgba_wrapper
    template<class Blender> struct copy_or_blend_rgba_wrapper
    {
        typedef typename Blender::color_type color_type;
        typedef typename Blender::order_type order_type;
        typedef typename color_type::value_type value_type;
        typedef typename color_type::calc_type calc_type;
        enum base_scale_e
        {
            base_shift = color_type::base_shift,
            base_scale = color_type::base_scale,
            base_mask  = color_type::base_mask
        };

        //--------------------------------------------------------------------
        static AGG_INLINE void copy_or_blend_pix(value_type* p, 
                                                 unsigned cr, unsigned cg, unsigned cb,
                                                 unsigned alpha)
        {
            if(alpha)
            {
                if(alpha == base_mask)
                {
                    p[order_type::R] = cr;
                    p[order_type::G] = cg;
                    p[order_type::B] = cb;
                    p[order_type::A] = base_mask;
                }
                else
                {
                    Blender::blend_pix(p, cr, cg, cb, alpha);
                }
            }
        }

        //--------------------------------------------------------------------
        static AGG_INLINE void copy_or_blend_pix(value_type* p, 
                                                 unsigned cr, unsigned cg, unsigned cb,
                                                 unsigned alpha,
                                                 unsigned cover)
        {
            if(cover == 255)
            {
                copy_or_blend_pix(p, cr, cg, cb, alpha);
            }
            else
            {
                if(alpha)
                {
                    alpha = (alpha * (cover + 1)) >> 8;
                    if(alpha == base_mask)
                    {
                        p[order_type::R] = cr;
                        p[order_type::G] = cg;
                        p[order_type::B] = cb;
                        p[order_type::A] = base_mask;
                    }
                    else
                    {
                        Blender::blend_pix(p, cr, cg, cb, alpha, cover);
                    }
                }
            }
        }
    };


    
    //=================================================pixfmt_alpha_blend_rgba
    template<class Blender, class RenBuf> 
    class pixfmt_alpha_blend_rgba
    {
    public:
        typedef RenBuf   rbuf_type;
        typedef typename rbuf_type::row_data row_data;
        typedef int32u   pixel_type;
        typedef Blender  blender_type;
        typedef typename blender_type::color_type color_type;
        typedef typename blender_type::order_type order_type;
        typedef typename color_type::value_type value_type;
        typedef typename color_type::calc_type calc_type;
        typedef copy_or_blend_rgba_wrapper<blender_type> cob_type;
        enum base_scale_e
        {
            base_shift = color_type::base_shift,
            base_scale = color_type::base_scale,
            base_mask  = color_type::base_mask,
            pix_width  = sizeof(pixel_type)
        };

        //--------------------------------------------------------------------
        pixfmt_alpha_blend_rgba() : m_rbuf(0) {}
        explicit pixfmt_alpha_blend_rgba(rbuf_type& rb) : m_rbuf(&rb) {}
        void attach(rbuf_type& rb) { m_rbuf = &rb; }

        //--------------------------------------------------------------------
        template<class PixFmt>
        bool attach(PixFmt& pixf, int x1, int y1, int x2, int y2)
        {
            rect_i r(x1, y1, x2, y2);
            if(r.clip(rect_i(0, 0, pixf.width()-1, pixf.height()-1)))
            {
                int stride = pixf.stride();
                m_rbuf->attach(pixf.pix_ptr(r.x1, stride < 0 ? r.y2 : r.y1), 
                               (r.x2 - r.x1) + 1,
                               (r.y2 - r.y1) + 1,
                               stride);
                return true;
            }
            return false;
        }

        //--------------------------------------------------------------------
        AGG_INLINE unsigned width()  const { return m_rbuf->width();  }
        AGG_INLINE unsigned height() const { return m_rbuf->height(); }
        AGG_INLINE int      stride() const { return m_rbuf->stride(); }

        //--------------------------------------------------------------------
        AGG_INLINE       int8u* row_ptr(int y)       { return m_rbuf->row_ptr(y); }
        AGG_INLINE const int8u* row_ptr(int y) const { return m_rbuf->row_ptr(y); }
        AGG_INLINE row_data     row(int y)     const { return m_rbuf->row(y); }

        //--------------------------------------------------------------------
        AGG_INLINE int8u* pix_ptr(int x, int y)
        {
            return m_rbuf->row_ptr(y) + x * pix_width;
        }

        AGG_INLINE const int8u* pix_ptr(int x, int y) const
        {
            return m_rbuf->row_ptr(y) + x * pix_width;
        }


        //--------------------------------------------------------------------
        AGG_INLINE static void make_pix(int8u* p, const color_type& c)
        {
            ((value_type*)p)[order_type::R] = c.r;
            ((value_type*)p)[order_type::G] = c.g;
            ((value_type*)p)[order_type::B] = c.b;
            ((value_type*)p)[order_type::A] = c.a;
        }

        //--------------------------------------------------------------------
        AGG_INLINE color_type pixel(int x, int y) const
        {
            const value_type* p = (const value_type*)m_rbuf->row_ptr(y);
            if(p)
            {
                p += x << 2;
                return color_type(p[order_type::R], 
                                  p[order_type::G], 
                                  p[order_type::B], 
                                  p[order_type::A]);
            }
            return color_type::no_color();
        }

        //--------------------------------------------------------------------
        AGG_INLINE void copy_pixel(int x, int y, const color_type& c)
        {
            value_type* p = (value_type*)m_rbuf->row_ptr(x, y, 1) + (x << 2);
            p[order_type::R] = c.r;
            p[order_type::G] = c.g;
            p[order_type::B] = c.b;
            p[order_type::A] = c.a;
        }

        //--------------------------------------------------------------------
        AGG_INLINE void blend_pixel(int x, int y, const color_type& c, int8u cover)
        {
            cob_type::copy_or_blend_pix(
                (value_type*)m_rbuf->row_ptr(x, y, 1) + (x << 2), 
                c.r, c.g, c.b, c.a, 
                cover);
        }


        //--------------------------------------------------------------------
        AGG_INLINE void copy_hline(int x, int y, 
                                   unsigned len, 
                                   const color_type& c)
        {
            value_type* p = (value_type*)m_rbuf->row_ptr(x, y, len) + (x << 2);
            pixel_type v;
            ((value_type*)&v)[order_type::R] = c.r;
            ((value_type*)&v)[order_type::G] = c.g;
            ((value_type*)&v)[order_type::B] = c.b;
            ((value_type*)&v)[order_type::A] = c.a;
            do
            {
                *(pixel_type*)p = v;
                p += 4;
            }
            while(--len);
        }


        //--------------------------------------------------------------------
        AGG_INLINE void copy_vline(int x, int y,
                                   unsigned len, 
                                   const color_type& c)
        {
            pixel_type v;
            ((value_type*)&v)[order_type::R] = c.r;
            ((value_type*)&v)[order_type::G] = c.g;
            ((value_type*)&v)[order_type::B] = c.b;
            ((value_type*)&v)[order_type::A] = c.a;
            do
            {
                value_type* p = (value_type*)m_rbuf->row_ptr(x, y++, 1) + (x << 2);
                *(pixel_type*)p = v;
            }
            while(--len);
        }


        //--------------------------------------------------------------------
        void blend_hline(int x, int y,
                         unsigned len, 
                         const color_type& c,
                         int8u cover)
        {
            if (c.a)
            {
                value_type* p = (value_type*)m_rbuf->row_ptr(x, y, len) + (x << 2);
                calc_type alpha = (calc_type(c.a) * (cover + 1)) >> 8;
                if(alpha == base_mask)
                {
                    pixel_type v;
                    ((value_type*)&v)[order_type::R] = c.r;
                    ((value_type*)&v)[order_type::G] = c.g;
                    ((value_type*)&v)[order_type::B] = c.b;
                    ((value_type*)&v)[order_type::A] = c.a;
                    do
                    {
                        *(pixel_type*)p = v;
                        p += 4;
                    }
                    while(--len);
                }
                else
                {
                    if(cover == 255)
                    {
                        do
                        {
                            blender_type::blend_pix(p, c.r, c.g, c.b, alpha);
                            p += 4;
                        }
                        while(--len);
                    }
                    else
                    {
                        do
                        {
                            blender_type::blend_pix(p, c.r, c.g, c.b, alpha, cover);
                            p += 4;
                        }
                        while(--len);
                    }
                }
            }
        }


        //--------------------------------------------------------------------
        void blend_vline(int x, int y,
                         unsigned len, 
                         const color_type& c,
                         int8u cover)
        {
            if (c.a)
            {
                value_type* p;
                calc_type alpha = (calc_type(c.a) * (cover + 1)) >> 8;
                if(alpha == base_mask)
                {
                    pixel_type v;
                    ((value_type*)&v)[order_type::R] = c.r;
                    ((value_type*)&v)[order_type::G] = c.g;
                    ((value_type*)&v)[order_type::B] = c.b;
                    ((value_type*)&v)[order_type::A] = c.a;
                    do
                    {
                        p = (value_type*)m_rbuf->row_ptr(x, y++, 1) + (x << 2);
                        *(pixel_type*)p = v;
                    }
                    while(--len);
                }
                else
                {
                    if(cover == 255)
                    {
                        do
                        {
                            p = (value_type*)m_rbuf->row_ptr(x, y++, 1) + (x << 2);
                            blender_type::blend_pix(p, c.r, c.g, c.b, alpha);
                        }
                        while(--len);
                    }
                    else
                    {
                        do
                        {
                            p = (value_type*)m_rbuf->row_ptr(x, y++, 1) + (x << 2);
                            blender_type::blend_pix(p, c.r, c.g, c.b, alpha, cover);
                        }
                        while(--len);
                    }
                }
            }
        }


        //--------------------------------------------------------------------
        void blend_solid_hspan(int x, int y,
                               unsigned len, 
                               const color_type& c,
                               const int8u* covers)
        {
            if (c.a)
            {
                value_type* p = (value_type*)m_rbuf->row_ptr(x, y, len) + (x << 2);
                do 
                {
                    calc_type alpha = (calc_type(c.a) * (calc_type(*covers) + 1)) >> 8;
                    if(alpha == base_mask)
                    {
                        p[order_type::R] = c.r;
                        p[order_type::G] = c.g;
                        p[order_type::B] = c.b;
                        p[order_type::A] = base_mask;
                    }
                    else
                    {
                        blender_type::blend_pix(p, c.r, c.g, c.b, alpha, *covers);
                    }
                    p += 4;
                    ++covers;
                }
                while(--len);
            }
        }


        //--------------------------------------------------------------------
        void blend_solid_vspan(int x, int y,
                               unsigned len, 
                               const color_type& c,
                               const int8u* covers)
        {
            if (c.a)
            {
                do 
                {
                    value_type* p = (value_type*)m_rbuf->row_ptr(x, y++, 1) + (x << 2);
                    calc_type alpha = (calc_type(c.a) * (calc_type(*covers) + 1)) >> 8;
                    if(alpha == base_mask)
                    {
                        p[order_type::R] = c.r;
                        p[order_type::G] = c.g;
                        p[order_type::B] = c.b;
                        p[order_type::A] = base_mask;
                    }
                    else
                    {
                        blender_type::blend_pix(p, c.r, c.g, c.b, alpha, *covers);
                    }
                    ++covers;
                }
                while(--len);
            }
        }


        //--------------------------------------------------------------------
        void copy_color_hspan(int x, int y,
                              unsigned len, 
                              const color_type* colors)
        {
            value_type* p = (value_type*)m_rbuf->row_ptr(x, y, len) + (x << 2);
            do 
            {
                p[order_type::R] = colors->r;
                p[order_type::G] = colors->g;
                p[order_type::B] = colors->b;
                p[order_type::A] = colors->a;
                ++colors;
                p += 4;
            }
            while(--len);
        }


        //--------------------------------------------------------------------
        void copy_color_vspan(int x, int y,
                              unsigned len, 
                              const color_type* colors)
        {
            do 
            {
                value_type* p = (value_type*)m_rbuf->row_ptr(x, y++, 1) + (x << 2);
                p[order_type::R] = colors->r;
                p[order_type::G] = colors->g;
                p[order_type::B] = colors->b;
                p[order_type::A] = colors->a;
                ++colors;
            }
            while(--len);
        }


        //--------------------------------------------------------------------
        void blend_color_hspan(int x, int y,
                               unsigned len, 
                               const color_type* colors,
                               const int8u* covers,
                               int8u cover)
        {
            value_type* p = (value_type*)m_rbuf->row_ptr(x, y, len) + (x << 2);
            if(covers)
            {
                do 
                {
                    cob_type::copy_or_blend_pix(p, 
                                                colors->r, 
                                                colors->g, 
                                                colors->b, 
                                                colors->a, 
                                                *covers++);
                    p += 4;
                    ++colors;
                }
                while(--len);
            }
            else
            {
                if(cover == 255)
                {
                    do 
                    {
                        cob_type::copy_or_blend_pix(p, 
                                                    colors->r, 
                                                    colors->g, 
                                                    colors->b, 
                                                    colors->a);
                        p += 4;
                        ++colors;
                    }
                    while(--len);
                }
                else
                {
                    do 
                    {
                        cob_type::copy_or_blend_pix(p, 
                                                    colors->r, 
                                                    colors->g, 
                                                    colors->b, 
                                                    colors->a, 
                                                    cover);
                        p += 4;
                        ++colors;
                    }
                    while(--len);
                }
            }
        }



        //--------------------------------------------------------------------
        void blend_color_vspan(int x, int y,
                               unsigned len, 
                               const color_type* colors,
                               const int8u* covers,
                               int8u cover)
        {
            value_type* p;
            if(covers)
            {
                do 
                {
                    p = (value_type*)m_rbuf->row_ptr(x, y++, 1) + (x << 2);
                    cob_type::copy_or_blend_pix(p, 
                                                colors->r, 
                                                colors->g, 
                                                colors->b, 
                                                colors->a,
                                                *covers++);
                    ++colors;
                }
                while(--len);
            }
            else
            {
                if(cover == 255)
                {
                    do 
                    {
                        p = (value_type*)m_rbuf->row_ptr(x, y++, 1) + (x << 2);
                        cob_type::copy_or_blend_pix(p, 
                                                    colors->r, 
                                                    colors->g, 
                                                    colors->b, 
                                                    colors->a);
                        ++colors;
                    }
                    while(--len);
                }
                else
                {
                    do 
                    {
                        p = (value_type*)m_rbuf->row_ptr(x, y++, 1) + (x << 2);
                        cob_type::copy_or_blend_pix(p, 
                                                    colors->r, 
                                                    colors->g, 
                                                    colors->b, 
                                                    colors->a, 
                                                    cover);
                        ++colors;
                    }
                    while(--len);
                }
            }
        }

        //--------------------------------------------------------------------
        template<class Function> void for_each_pixel(Function f)
        {
            unsigned y;
            for(y = 0; y < height(); ++y)
            {
                row_data r = m_rbuf->row(y);
                if(r.ptr)
                {
                    unsigned len = r.x2 - r.x1 + 1;
                    value_type* p = 
                        (value_type*)m_rbuf->row_ptr(r.x1, y, len) + (r.x1 << 2);
                    do
                    {
                        f(p);
                        p += 4;
                    }
                    while(--len);
                }
            }
        }

        //--------------------------------------------------------------------
        void premultiply()
        {
            for_each_pixel(multiplier_rgba<color_type, order_type>::premultiply);
        }

        //--------------------------------------------------------------------
        void demultiply()
        {
            for_each_pixel(multiplier_rgba<color_type, order_type>::demultiply);
        }

        //--------------------------------------------------------------------
        template<class GammaLut> void apply_gamma_dir(const GammaLut& g)
        {
            for_each_pixel(apply_gamma_dir_rgba<color_type, order_type, GammaLut>(g));
        }

        //--------------------------------------------------------------------
        template<class GammaLut> void apply_gamma_inv(const GammaLut& g)
        {
            for_each_pixel(apply_gamma_inv_rgba<color_type, order_type, GammaLut>(g));
        }

        //--------------------------------------------------------------------
        template<class RenBuf2> void copy_from(const RenBuf2& from, 
                                               int xdst, int ydst,
                                               int xsrc, int ysrc,
                                               unsigned len)
        {
            const int8u* p = from.row_ptr(ysrc);
            if(p)
            {
                memmove(m_rbuf->row_ptr(xdst, ydst, len) + xdst * pix_width, 
                        p + xsrc * pix_width, 
                        len * pix_width);
            }
        }

        //--------------------------------------------------------------------
        template<class SrcPixelFormatRenderer>
        void blend_from(const SrcPixelFormatRenderer& from, 
                        int xdst, int ydst,
                        int xsrc, int ysrc,
                        unsigned len,
                        int8u cover)
        {
            typedef typename SrcPixelFormatRenderer::order_type src_order;
            const value_type* psrc = (value_type*)from.row_ptr(ysrc);
            if(psrc)
            {
                psrc += xsrc << 2;
                value_type* pdst = 
                    (value_type*)m_rbuf->row_ptr(xdst, ydst, len) + (xdst << 2);
                int incp = 4;
                if(xdst > xsrc)
                {
                    psrc += (len-1) << 2;
                    pdst += (len-1) << 2;
                    incp = -4;
                }

                if(cover == 255)
                {
                    do 
                    {
                        cob_type::copy_or_blend_pix(pdst, 
                                                    psrc[src_order::R],
                                                    psrc[src_order::G],
                                                    psrc[src_order::B],
                                                    psrc[src_order::A]);
                        psrc += incp;
                        pdst += incp;
                    }
                    while(--len);
                }
                else
                {
                    do 
                    {
                        cob_type::copy_or_blend_pix(pdst, 
                                                    psrc[src_order::R],
                                                    psrc[src_order::G],
                                                    psrc[src_order::B],
                                                    psrc[src_order::A],
                                                    cover);
                        psrc += incp;
                        pdst += incp;
                    }
                    while(--len);
                }
            }
        }

        //--------------------------------------------------------------------
        template<class SrcPixelFormatRenderer>
        void blend_from_color(const SrcPixelFormatRenderer& from, 
                              const color_type& color,
                              int xdst, int ydst,
                              int xsrc, int ysrc,
                              unsigned len,
                              int8u cover)
        {
            typedef typename SrcPixelFormatRenderer::value_type src_value_type;
            const src_value_type* psrc = (src_value_type*)from.row_ptr(ysrc);
            if(psrc)
            {
                value_type* pdst = 
                    (value_type*)m_rbuf->row_ptr(xdst, ydst, len) + (xdst << 2);
                do 
                {
                    cob_type::copy_or_blend_pix(pdst, 
                                                color.r, color.g, color.b, color.a,
                                                (*psrc * cover + base_mask) >> base_shift);
                    ++psrc;
                    pdst += 4;
                }
                while(--len);
            }
        }

        //--------------------------------------------------------------------
        template<class SrcPixelFormatRenderer>
        void blend_from_lut(const SrcPixelFormatRenderer& from, 
                            const color_type* color_lut,
                            int xdst, int ydst,
                            int xsrc, int ysrc,
                            unsigned len,
                            int8u cover)
        {
            typedef typename SrcPixelFormatRenderer::value_type src_value_type;
            const src_value_type* psrc = (src_value_type*)from.row_ptr(ysrc);
            if(psrc)
            {
                value_type* pdst = 
                    (value_type*)m_rbuf->row_ptr(xdst, ydst, len) + (xdst << 2);

                if(cover == 255)
                {
                    do 
                    {
                        const color_type& color = color_lut[*psrc];
                        cob_type::copy_or_blend_pix(pdst, 
                                                    color.r, color.g, color.b, color.a);
                        ++psrc;
                        pdst += 4;
                    }
                    while(--len);
                }
                else
                {
                    do 
                    {
                        const color_type& color = color_lut[*psrc];
                        cob_type::copy_or_blend_pix(pdst, 
                                                    color.r, color.g, color.b, color.a,
                                                    cover);
                        ++psrc;
                        pdst += 4;
                    }
                    while(--len);
                }
            }
        }

    private:
        rbuf_type* m_rbuf;
    };


    //-----------------------------------------------------------------------
    typedef blender_rgba<rgba8, order_bgra> blender_bgra32; //----blender_bgra32


    typedef pixfmt_alpha_blend_rgba<blender_bgra32, rendering_buffer> pixfmt_bgra32; //----pixfmt_bgra32
}

#endif

