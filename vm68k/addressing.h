/* -*- C++ -*- */
/* Libvm68k - M68000 virtual machine library
   Copyright (C) 1998-2001 Hypercore Software Design, Ltd.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef _VM68K_ADDRESSING_H
#define _VM68K_ADDRESSING_H 1

#include <vm68k/processor.h>

#include <string>
#include <cstdio>

namespace vm68k
{
  namespace addressing
  {
    using namespace std;

    template <class Size> class basic_d_register
    {
    public:
      typedef Size size_type;
      typedef typename Size::uvalue_type uvalue_type;
      typedef typename Size::svalue_type svalue_type;

    public:
      static size_t extension_size() {return 0;}

    private:
      int r;

    public:
      basic_d_register(int reg, uint32_type address)
	: r(reg) {}

    public:
      // XXX address is left unimplemented.

      typename Size::svalue_type get(const context &c) const;

      void put(context &c, svalue_type value) const {Size::put(c.regs.d[r], value);}
      void finish(context &c) const {}

    public:
      string text(const context &c) const;
    };

    template <class Size> inline typename Size::svalue_type
    basic_d_register<Size>::get(const context &c) const
    {
      return Size::get(c.regs.d[r]);
    }

    template <class Size> string
    basic_d_register<Size>::text(const context &c) const
    {
      char buf[8];
      sprintf(buf, "%%d%u", r);
      return buf;
    }

    typedef basic_d_register<byte_size> byte_d_register;
    typedef basic_d_register<word_size> word_d_register;
    typedef basic_d_register<long_word_size> long_word_d_register;

    template <class Size> class basic_a_register
    {
    public:
      typedef Size size_type;
      typedef typename Size::uvalue_type uvalue_type;
      typedef typename Size::svalue_type svalue_type;

    public:
      static size_t extension_size() {return 0;}

    private:
      int r;

    public:
      basic_a_register(int reg, uint32_type address)
	: r(reg) {}

    public:
      // XXX address is left unimplemented.

      typename Size::svalue_type get(const context &c) const;

      void put(context &c, svalue_type value) const {long_word_size::put(c.regs.a[r], value);}
      void finish(context &c) const {}

    public:
      string text(const context &c) const;
    };

    template <class Size> inline typename Size::svalue_type
    basic_a_register<Size>::get(const context &c) const
    {
      return Size::get(c.regs.a[r]);
    }

    template <class Size> string
    basic_a_register<Size>::text(const context &c) const
    {
      char buf[8];
      sprintf(buf, "%%a%u", r);
      return buf;
    }

    // XXX Address register direct is not allowed for byte size.
    typedef basic_a_register<word_size> word_a_register;
    typedef basic_a_register<long_word_size> long_word_a_register;

    template <class Size> class basic_indirect
    {
    public:
      typedef Size size_type;
      typedef typename Size::uvalue_type uvalue_type;
      typedef typename Size::svalue_type svalue_type;

    public:
      static size_t extension_size() {return 0;}

    private:
      int r;

    public:
      basic_indirect(int reg, uint32_type address)
	: r(reg) {}

    public:
      uint32_type address(const context &c) const {return c.regs.a[r];}

      typename Size::svalue_type get(const context &c) const;

      void put(context &c, svalue_type value) const {Size::put(*c.mem, c.data_fc(), address(c), value);}
      void finish(context &c) const {}

    public:
      string text(const context &c) const;
    };

    template <class Size> inline typename Size::svalue_type
    basic_indirect<Size>::get(const context &c) const
    {
      return Size::get(*c.mem, c.data_fc(), address(c));
    }

    template <class Size> string
    basic_indirect<Size>::text(const context &c) const
    {
      char buf[64];
      sprintf(buf, "%%a%u@/* %#lx */", r, address(c) + 0UL);
      return buf;
    }

    typedef basic_indirect<byte_size> byte_indirect;
    typedef basic_indirect<word_size> word_indirect;
    typedef basic_indirect<long_word_size> long_word_indirect;

    template <class Size> class basic_postinc_indirect
    {
    public:
      typedef Size size_type;
      typedef typename Size::uvalue_type uvalue_type;
      typedef typename Size::svalue_type svalue_type;

    public:
      static size_t extension_size() {return 0;}

    private:
      int r;

    public:
      basic_postinc_indirect(int reg, uint32_type address)
	: r(reg) {}

    protected:
      size_t advance_size() const
      {
	if (r == 7)		// XXX: %a7 is special.
	  return Size::aligned_value_size();
	else
	  return Size::value_size();
      }

    public:
      // XXX: address is left unimplemented.

      typename Size::svalue_type get(const context &c) const;

      void put(context &c, svalue_type value) const {Size::put(*c.mem, c.data_fc(), c.regs.a[r], value);}

      void finish(context &c) const {c.regs.a[r] += advance_size();}

    public:
      string text(const context &c) const;
    };

    template <class Size> inline typename Size::svalue_type
    basic_postinc_indirect<Size>::get(const context &c) const
    {
      return Size::get(*c.mem, c.data_fc(), c.regs.a[r]);
    }

    template <class Size> string
    basic_postinc_indirect<Size>::text(const context &c) const
    {
      char buf[64];
      sprintf(buf, "%%a%u@+/* %#lx */", r, c.regs.a[r] + 0UL);
      return buf;
    }

    typedef basic_postinc_indirect<byte_size> byte_postinc_indirect;
    typedef basic_postinc_indirect<word_size> word_postinc_indirect;
    typedef basic_postinc_indirect<long_word_size> long_word_postinc_indirect;

    template <class Size> class basic_predec_indirect
    {
    public:
      typedef Size size_type;
      typedef typename Size::uvalue_type uvalue_type;
      typedef typename Size::svalue_type svalue_type;

    public:
      static size_t extension_size() {return 0;}

    private:
      int r;

    public:
      basic_predec_indirect(int reg, uint32_type address)
	: r(reg) {}

    protected:
      size_t advance_size() const
      {
	if (r == 7)		// XXX: %a7 is special.
	  return Size::aligned_value_size();
	else
	  return Size::value_size();
      }

    public:
      // XXX: address is left unimplemented.

      typename Size::svalue_type get(const context &c) const;

      void put(context &c, svalue_type value) const {Size::put(*c.mem, c.data_fc(),
							       c.regs.a[r] - advance_size(), value);}
      void finish(context &c) const {c.regs.a[r] -= advance_size();}

    public:
      string text(const context &c) const;
    };

    template <class Size> inline typename Size::svalue_type
    basic_predec_indirect<Size>::get(const context &c) const
    {
      return Size::get(*c.mem, c.data_fc(), c.regs.a[r] - advance_size());
    }

    template <class Size> string
    basic_predec_indirect<Size>::text(const context &c) const
    {
      char buf[64];
      sprintf(buf, "%%a%u@-/* %#lx */", r,
	      c.regs.a[r] - advance_size() + 0UL);
      return buf;
    }

    typedef basic_predec_indirect<byte_size> byte_predec_indirect;
    typedef basic_predec_indirect<word_size> word_predec_indirect;
    typedef basic_predec_indirect<long_word_size> long_word_predec_indirect;

    template <class Size> class basic_disp_indirect
    {
    public:
      typedef Size size_type;
      typedef typename Size::uvalue_type uvalue_type;
      typedef typename Size::svalue_type svalue_type;

    public:
      static size_t extension_size() {return 2;}

    private:
      int r;

    private:
      uint32_type addr;

    public:
      basic_disp_indirect(int reg, uint32_type address)
	: r(reg), addr(address) {}

    public:
      uint32_type address(const context &c) const;

      typename Size::svalue_type get(const context &c) const;

      void put(context &c, svalue_type value) const {Size::put(*c.mem, c.data_fc(), address(c), value);}
      void finish(context &c) const {}

    public:
      string text(const context &c) const;
    };

    template <class Size> inline uint32_type
    basic_disp_indirect<Size>::address(const context &c) const
    {
      return c.regs.a[r] + c.fetch_s(word_size(), addr);
    }

    template <class Size> inline typename Size::svalue_type
    basic_disp_indirect<Size>::get(const context &c) const
    {
      return Size::get(*c.mem, c.data_fc(), address(c));
    }

    template <class Size> string
    basic_disp_indirect<Size>::text(const context &c) const
    {
      char buf[64];
      sprintf(buf, "%%a%u@(%d)/* %#lx */", r, c.fetch_s(word_size(), addr),
	      address(c) + 0UL);
      return buf;
    }

    typedef basic_disp_indirect<byte_size> byte_disp_indirect;
    typedef basic_disp_indirect<word_size> word_disp_indirect;
    typedef basic_disp_indirect<long_word_size> long_word_disp_indirect;

    template <class Size> class basic_index_indirect
    {
    public:
      typedef Size size_type;
      typedef typename Size::uvalue_type uvalue_type;
      typedef typename Size::svalue_type svalue_type;

    public:
      static size_t extension_size() {return 2;}

    private:
      int r;

    private:
      uint32_type addr;

    public:
      basic_index_indirect(int reg, uint32_type address)
	: r(reg), addr(address) {}

    public:
      uint32_type address(const context &c) const;

      typename Size::svalue_type get(const context &c) const;

      void put(context &c, svalue_type value) const {Size::put(*c.mem, c.data_fc(), address(c), value);}

      void finish(context &c) const {}

    public:
      string text(const context &c) const;
    };

    template <class Size> inline uint32_type
    basic_index_indirect<Size>::address(const context &c) const
    {
      uint16_type w = c.fetch_u(word_size(), addr);
      int r = w >> 12 & 0xf;
      uint32_type x = r >= 8 ? c.regs.a[r - 8] : c.regs.d[r];
      if (w & 0x800)
	return c.regs.a[r] + byte_size::svalue(w) + long_word_size::get(x);
      else
	return c.regs.a[r] + byte_size::svalue(w) + word_size::get(x);
    }

    template <class Size> inline typename Size::svalue_type
    basic_index_indirect<Size>::get(const context &c) const
    {
      return Size::get(*c.mem, c.data_fc(), address(c));
    }

    template <class Size> string
    basic_index_indirect<Size>::text(const context &c) const
    {
      uint16_type w = c.fetch_u(word_size(), addr);
      int r = w >> 12 & 0xf;
      char buf[64];
      if (r >= 8)
	sprintf(buf, "%%a%u@(%d,%%a%u%s)/* %#lx */", r, byte_size::svalue(w),
		r - 8, w & 0x800 ? ":l" : ":w", address(c) + 0UL);
      else
	sprintf(buf, "%%a%u@(%d,%%d%u%s)/* %#lx */", r, byte_size::svalue(w),
		r, w & 0x800 ? ":l" : ":w", address(c) + 0UL);
      return buf;
    }

    typedef basic_index_indirect<byte_size> byte_index_indirect;
    typedef basic_index_indirect<word_size> word_index_indirect;
    typedef basic_index_indirect<long_word_size> long_word_index_indirect;

    template <class Size> class basic_abs_short
    {
    public:
      typedef Size size_type;
      typedef typename Size::uvalue_type uvalue_type;
      typedef typename Size::svalue_type svalue_type;

    public:
      static size_t extension_size() {return 2;}

    private:
      uint32_type addr;

    public:
      basic_abs_short(int reg, uint32_type address)
	: addr(address) {}

    public:
      uint32_type address(const context &c) const;

      typename Size::svalue_type get(const context &c) const;

      void put(context &c, svalue_type value) const {Size::put(*c.mem, c.data_fc(), address(c), value);}
      void finish(context &c) const {}

    public:
      string text(const context &c) const
      {
	char buf[32];
	sprintf(buf, "%#lx:s", address(c) + 0UL);
	return buf;
      }
    };

    template <class Size> inline uint32_type
    basic_abs_short<Size>::address(const context &c) const
    {
      return c.fetch_s(word_size(), addr);
    }

    template <class Size> inline typename Size::svalue_type
    basic_abs_short<Size>::get(const context &c) const
    {
      return Size::get(*c.mem, c.data_fc(), address(c));
    }

    typedef basic_abs_short<byte_size> byte_abs_short;
    typedef basic_abs_short<word_size> word_abs_short;
    typedef basic_abs_short<long_word_size> long_word_abs_short;

    template <class Size> class basic_abs_long
    {
    public:
      typedef Size size_type;
      typedef typename Size::uvalue_type uvalue_type;
      typedef typename Size::svalue_type svalue_type;

    public:
      static size_t extension_size() {return 4;}

    private:
      uint32_type addr;

    public:
      basic_abs_long(int reg, uint32_type address)
	: addr(address) {}

    public:
      uint32_type address(const context &c) const;

      typename Size::svalue_type get(const context &c) const;

      void put(context &c, svalue_type value) const {Size::put(*c.mem, c.data_fc(), address(c), value);}
      void finish(context &c) const {}

    public:
      string text(const context &c) const;
    };

    template <class Size> inline uint32_type
    basic_abs_long<Size>::address(const context &c) const
    {
      return c.fetch_s(long_word_size(), addr);
    }

    template <class Size> inline typename Size::svalue_type
    basic_abs_long<Size>::get(const context &c) const
    {
      return Size::get(*c.mem, c.data_fc(), address(c));
    }

    template <class Size> string
    basic_abs_long<Size>::text(const context &c) const
    {
      char buf[32];
      sprintf(buf, "%#lx", address(c) + 0UL);
      return buf;
    }

    typedef basic_abs_long<byte_size> byte_abs_long;
    typedef basic_abs_long<word_size> word_abs_long;
    typedef basic_abs_long<long_word_size> long_word_abs_long;

    template <class Size> class basic_disp_pc_indirect
    {
    public:
      typedef Size size_type;
      typedef typename Size::uvalue_type uvalue_type;
      typedef typename Size::svalue_type svalue_type;

    public:
      static size_t extension_size() {return 2;}

    private:
      uint32_type addr;

    public:
      basic_disp_pc_indirect(int reg, uint32_type address)
	: addr(address) {}

    public:
      uint32_type address(const context &c) const;

      typename Size::svalue_type get(const context &c) const;

      // XXX put is left unimplemented.
      void finish(context &c) const {}

    public:
      string text(const context &c) const;
    };

    template <class Size> inline uint32_type
    basic_disp_pc_indirect<Size>::address(const context &c) const
    {
      return addr + c.fetch_s(word_size(), addr);
    }

    template <class Size> inline typename Size::svalue_type
    basic_disp_pc_indirect<Size>::get(const context &c) const
    {
      return Size::get(*c.mem, c.data_fc(), address(c));
    }

    template <class Size> string
    basic_disp_pc_indirect<Size>::text(const context &c) const
    {
      char buf[64];
      sprintf(buf, "%%pc@(%d)/* %#lx */", c.fetch_s(word_size(), addr),
	      address(c) + 0UL);
      return buf;
    }

    typedef basic_disp_pc_indirect<byte_size> byte_disp_pc_indirect;
    typedef basic_disp_pc_indirect<word_size> word_disp_pc_indirect;
    typedef basic_disp_pc_indirect<long_word_size> long_word_disp_pc_indirect;

    template <class Size> class basic_index_pc_indirect
    {
    public:
      typedef Size size_type;
      typedef typename Size::uvalue_type uvalue_type;
      typedef typename Size::svalue_type svalue_type;

    public:
      static size_t extension_size() {return 2;}

    private:
      uint32_type addr;

    public:
      basic_index_pc_indirect(int reg, uint32_type address)
	: addr(address) {}

    public:
      uint32_type address(const context &c) const;

      typename Size::svalue_type get(const context &c) const;

      void put(context &c, svalue_type value) const {Size::put(*c.mem, c.data_fc(), address(c), value);}
      void finish(context &c) const {}

    public:
      string text(const context &c) const;
    };

    template <class Size> inline uint32_type
    basic_index_pc_indirect<Size>::address(const context &c) const
    {
      uint16_type w = c.fetch_u(word_size(), addr);
      int r = w >> 12 & 0xf;
      uint32_type x = r >= 8 ? c.regs.a[r - 8] : c.regs.d[r];
      if (w & 0x800)
	return addr + byte_size::svalue(w) + long_word_size::get(x);
      else
	return addr + byte_size::svalue(w) + word_size::get(x);
    }

    template <class Size> inline typename Size::svalue_type
    basic_index_pc_indirect<Size>::get(const context &c) const
    {
      return Size::get(*c.mem, c.data_fc(), address(c));
    }

    template <class Size> string
    basic_index_pc_indirect<Size>::text(const context &c) const
    {
      uint16_type w = c.fetch_u(word_size(), addr);
      int r = w >> 12 & 0xf;
      char buf[64];
      if (r >= 8)
	sprintf(buf, "%%pc@(%d,%%a%u%s)/* %#lx */", byte_size::svalue(w), r - 8,
		w & 0x800 ? ":l" : ":w", address(c) + 0UL);
      else
	sprintf(buf, "%%pc@(%d,%%d%u%s)/* %#lx */", byte_size::svalue(w), r,
		w & 0x800 ? ":l" : ":w", address(c) + 0UL);
      return buf;
    }

    typedef basic_index_pc_indirect<byte_size> byte_index_pc_indirect;
    typedef basic_index_pc_indirect<word_size> word_index_pc_indirect;
    typedef basic_index_pc_indirect<long_word_size>
      long_word_index_pc_indirect;

    template <class Size> class basic_immediate
    {
    public:
      typedef Size size_type;
      typedef typename Size::uvalue_type uvalue_type;
      typedef typename Size::svalue_type svalue_type;

    public:
      static size_t extension_size() {return Size::aligned_value_size();}

    private:
      uint32_type addr;

    public:
      basic_immediate(int reg, uint32_type address)
	: addr(address) {}

    public:
      // XXX address is left unimplemented.

      typename Size::svalue_type get(const context &) const;

      // XXX put is left unimplemented.
      void finish(context &c) const {}

    public:
      string text(const context &c) const;
    };

    template <class Size> inline typename Size::svalue_type
    basic_immediate<Size>::get(const context &c) const
    {
      return c.fetch_s(Size(), addr);
    }

    template <class Size> string
    basic_immediate<Size>::text(const context &c) const
    {
      char buf[32];
      sprintf(buf, "#%#lx", (get(c) & Size::value_mask()) + 0UL);
      return buf;
    }

    typedef basic_immediate<byte_size> byte_immediate;
    typedef basic_immediate<word_size> word_immediate;
    typedef basic_immediate<long_word_size> long_word_immediate;
  }
}

#endif /* not _VM68K_ADDRESSING_H */

