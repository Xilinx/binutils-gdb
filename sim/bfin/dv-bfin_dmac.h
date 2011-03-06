/* Blackfin Direct Memory Access (DMA) Controller model.

   Copyright (C) 2010-2011 Free Software Foundation, Inc.
   Contributed by Analog Devices, Inc.

   This file is part of simulators.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef DV_BFIN_DMAC_H
#define DV_BFIN_DMAC_H

#define BFIN_MMR_DMAC0_BASE	0xFFC00C00
#define BFIN_MMR_DMAC1_BASE	0xFFC01C00

#define BFIN_DMAC_MDMA_BASE	0x100

struct hw *bfin_dmac_get_peer (struct hw *dma, bu16 pmap);
bu16 bfin_dmac_default_pmap (struct hw *dma);

#endif
