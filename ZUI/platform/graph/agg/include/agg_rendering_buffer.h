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

#ifndef AGG_RENDERING_BUFFER_INCLUDED
#define AGG_RENDERING_BUFFER_INCLUDED

#include "agg_array.h"

namespace agg
{

    //===========================================================row_accessor
    template<class T> class row_accessor
    {
    public:
        typedef const_row_info<T> row_data;

        //-------------------------------------------------------------------
        row_accessor() :
            m_buf(0),
            m_start(0),
            m_width(0),
            m_height(0),
            m_stride(0)
        {
        }

        //--------------------------------------------------------------------
        row_accessor(T* buf, unsigned width, unsigned height, int stride) :
            m_buf(0),
            m_start(0),
            m_width(0),
            m_height(0),
            m_stride(0)
        {
            attach(buf, width, height, stride);
        }


        //--------------------------------------------------------------------
        void attach(T* buf, unsigned width, unsigned height, int stride)
        {
			m_buf = m_start = buf;
			m_width = width;
			m_height = height;
			m_stride = stride;
			if(stride < 0) 
            { 
				m_start = m_buf - int(height - 1) * stride;
			}
        }

        //--------------------------------------------------------------------
        AGG_INLINE       T* buf()          { return m_buf;    }
        AGG_INLINE const T* buf()    const { return m_buf;    }
        AGG_INLINE unsigned width()  const { return m_width;  }
        AGG_INLINE unsigned height() const { return m_height; }
        AGG_INLINE int      stride() const { return m_stride; }
        AGG_INLINE unsigned stride_abs() const 
        {
            return (m_stride < 0) ? unsigned(-m_stride) : unsigned(m_stride); 
        }

        //--------------------------------------------------------------------
		AGG_INLINE       T* row_ptr(int, int y, unsigned) 
        { 
            return m_start + y * m_stride; 
        }
		AGG_INLINE       T* row_ptr(int y)       { return m_start + y * m_stride; }
		AGG_INLINE const T* row_ptr(int y) const { return m_start + y * m_stride; }
		AGG_INLINE row_data row    (int y) const 
        { 
            return row_data(0, m_width-1, row_ptr(y)); 
        }

        //--------------------------------------------------------------------
        template<class RenBuf>
        void copy_from(const RenBuf& src)
        {
            unsigned h = height();
            if(src.height() < h) h = src.height();
        
            unsigned l = stride_abs();
            if(src.stride_abs() < l) l = src.stride_abs();
        
            l *= sizeof(T);

            unsigned y;
            unsigned w = width();
            for (y = 0; y < h; y++)
            {
                memcpy(row_ptr(0, y, w), src.row_ptr(y), l);
            }
        }

        //--------------------------------------------------------------------
        void clear(T value)
        {
            unsigned y;
            unsigned w = width();
            unsigned stride = stride_abs();
            for(y = 0; y < height(); y++)
            {
                T* p = row_ptr(0, y, w);
                unsigned x;
                for(x = 0; x < stride; x++)
                {
                    *p++ = value;
                }
            }
        }

    private:
        //--------------------------------------------------------------------
        T*            m_buf;    // Pointer to renrdering buffer
        T*            m_start;  // Pointer to first pixel depending on stride 
        unsigned      m_width;  // Width in pixels
        unsigned      m_height; // Height in pixels
        int           m_stride; // Number of bytes per row. Can be < 0
    };


#ifdef AGG_RENDERING_BUFFER
    typedef AGG_RENDERING_BUFFER rendering_buffer;
#else
    typedef row_accessor<int8u> rendering_buffer;
#endif

}


#endif
