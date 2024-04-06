/* Copyright (C) 2024 Free Software Foundation, Inc.
   Contributed by Mikael Pettersson <mikpelinux@gmail.com>.

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/*
 * Trampolines are 9 bytes long and have the following contents:
 *
 * 0000 ldi %hi(static_chain_value)
 * 0002 phi STATIC_CHAIN_REGNUM
 * 0003 ldi %lo(static_chain_value)
 * 0005 plo STATIC_CHAIN_REGNUM
 * 0006 lbr fnaddr
 */

#define OP_LBR 0xC0
#define OP_LDI 0xF8
#define OP_PHI 0xB0
#define OP_PLO 0xA0

#define STATIC_CHAIN_REGNUM 6	/* must match gcc/config/cdp1802/cdp1802.h */

extern void __trampoline_setup (unsigned char *addr, void *fnptr, void *scptr);

void
__trampoline_setup (unsigned char *addr, void *fnptr, void *scptr)
{
  unsigned short fnaddr = (unsigned short) fnptr;
  unsigned short scaddr = (unsigned short) scptr;

  addr[0] = OP_LDI;
  addr[1] = scaddr >> 8;
  addr[2] = OP_PHI + STATIC_CHAIN_REGNUM;
  addr[3] = OP_LDI;
  addr[4] = scaddr & 0xFF;
  addr[5] = OP_PLO + STATIC_CHAIN_REGNUM;
  addr[6] = OP_LBR;
  addr[7] = fnaddr >> 8;
  addr[8] = fnaddr & 0xFF;

  /* The RCA CDP1802 has no architectural cache to flush.  */
}
