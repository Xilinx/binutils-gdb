/* Table of opcodes for the Texas Instruments TMS320C54X
   Copyright 1999, 2000, 2001 Free Software Foundation, Inc.
   Contributed by Timothy Wall (twall@cygnus.com)

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#include "sysdep.h"
#include "opcode/tic54x.h"

/* these are the only register names not found in mmregs */
const symbol regs[] = {
  { "AR0", 16 },                  { "ar0", 16 },
  { "AR1", 17 },                  { "ar1", 17 },
  { "AR2", 18 },                  { "ar2", 18 },
  { "AR3", 19 },                  { "ar3", 19 },
  { "AR4", 20 },                  { "ar4", 20 },
  { "AR5", 21 },                  { "ar5", 21 },
  { "AR6", 22 },                  { "ar6", 22 },
  { "AR7", 23 },                  { "ar7", 23 },
  { NULL, 0}
};

/* status bits, MM registers, condition codes, etc */
/* some symbols are only valid for certain chips... */
const symbol mmregs[] = {
  { "IMR", 0 },                   { "imr", 0 },
  { "IFR", 1 },                   { "ifr", 1 },
  { "ST0", 6 },                   { "st0", 6 },
  { "ST1", 7 },                   { "st1", 7 },
  { "AL",  8 },                   { "al",  8 },
  { "AH",  9 },                   { "ah",  9 },
  { "AG",  10 },                  { "ag",  10 },
  { "BL",  11 },                  { "bl",  11 },
  { "BH",  12 },                  { "bh",  12 },
  { "BG",  13 },                  { "bg",  13 },
  { "T",   14 },                  { "t",   14 },
  { "TRN", 15 },                  { "trn", 15 },
  { "AR0", 16 },                  { "ar0", 16 },
  { "AR1", 17 },                  { "ar1", 17 },
  { "AR2", 18 },                  { "ar2", 18 },
  { "AR3", 19 },                  { "ar3", 19 },
  { "AR4", 20 },                  { "ar4", 20 },
  { "AR5", 21 },                  { "ar5", 21 },
  { "AR6", 22 },                  { "ar6", 22 },
  { "AR7", 23 },                  { "ar7", 23 },
  { "SP",  24 },                  { "sp",  24 },
  { "BK",  25 },                  { "bk",  25 },
  { "BRC", 26 },                  { "brc", 26 },
  { "RSA", 27 },                  { "rsa", 27 },
  { "REA", 28 },                  { "rea", 28 },
  { "PMST",29 },                  { "pmst",29 },
  { "XPC", 30 },                  { "xpc", 30 }, /* 'c548 only */
  /* optional peripherals */      /* optional peripherals */
  { "M1F", 31 },                  { "m1f", 31 },
  { "DRR0",0x20 },                { "drr0",0x20 },
  { "BDRR0",0x20 },               { "bdrr0",0x20 }, /* 'c543, 545 */
  { "DXR0",0x21 },                { "dxr0",0x21 },
  { "BDXR0",0x21 },               { "bdxr0",0x21 }, /* 'c543, 545 */
  { "SPC0",0x22 },                { "spc0",0x22 },
  { "BSPC0",0x22 },               { "bspc0",0x22 }, /* 'c543, 545 */
  { "SPCE0",0x23 },               { "spce0",0x23 },
  { "BSPCE0",0x23 },              { "bspce0",0x23 }, /* 'c543, 545 */
  { "TIM", 0x24 },                { "tim", 0x24 },
  { "PRD", 0x25 },                { "prd", 0x25 },
  { "TCR", 0x26 },                { "tcr", 0x26 },
  { "SWWSR",0x28 },               { "swwsr",0x28 },
  { "BSCR",0x29 },                { "bscr",0x29 },
  { "HPIC",0x2C },                { "hpic",0x2c },
  /* 'c541, 'c545 */              /* 'c541, 'c545 */
  { "DRR1",0x30 },                { "drr1",0x30 },
  { "DXR1",0x31 },                { "dxr1",0x31 },
  { "SPC1",0x32 },                { "spc1",0x32 },
  /* 'c542, 'c543 */              /* 'c542, 'c543 */
  { "TRCV",0x30 },                { "trcv",0x30 },
  { "TDXR",0x31 },                { "tdxr",0x31 },
  { "TSPC",0x32 },                { "tspc",0x32 },
  { "TCSR",0x33 },                { "tcsr",0x33 },
  { "TRTA",0x34 },                { "trta",0x34 },
  { "TRAD",0x35 },                { "trad",0x35 },
  { "AXR0",0x38 },                { "axr0",0x38 },
  { "BKX0",0x39 },                { "bkx0",0x39 },
  { "ARR0",0x3A },                { "arr0",0x3a },
  { "BKR0",0x3B },                { "bkr0",0x3b },
  /* 'c545, 'c546, 'c548 */       /* 'c545, 'c546, 'c548 */
  { "CLKMD",0x58 },               { "clkmd",0x58 },
  /* 'c548 */                     /* 'c548 */
  { "AXR1",0x3C },                { "axr1",0x3c },
  { "BKX1",0x3D },                { "bkx1",0x3d },
  { "ARR1",0x3E },                { "arr1",0x3e },
  { "BKR1",0x3F },                { "bkr1",0x3f },
  { "BDRR1",0x40 },               { "bdrr1",0x40 },
  { "BDXR1",0x41 },               { "bdxr1",0x41 },
  { "BSPC1",0x42 },               { "bspc1",0x42 },
  { "BSPCE1",0x43 },              { "bspce1",0x43 },
  { NULL, 0},
};

const symbol condition_codes[] = {
  /* condition codes */
  { "UNC",  0 },                { "unc",  0 },
#define CC1   0x40
#define CCB   0x08
#define CCEQ  0x05
#define CCNEQ 0x04
#define CCLT  0x03
#define CCLEQ 0x07
#define CCGT  0x06
#define CCGEQ 0x02
#define CCOV  0x70
#define CCNOV 0x60
#define CCBIO 0x03
#define CCNBIO 0x02
#define CCTC  0x30
#define CCNTC 0x20
#define CCC   0x0C
#define CCNC  0x08
  { "aeq",  CC1|CCEQ },         { "AEQ",  CC1|CCEQ },
  { "aneq", CC1|CCNEQ },        { "ANEQ", CC1|CCNEQ },
  { "alt",  CC1|CCLT },         { "ALT",  CC1|CCLT },
  { "aleq", CC1|CCLEQ },        { "ALEQ", CC1|CCLEQ },
  { "agt",  CC1|CCGT },         { "AGT",  CC1|CCGT },
  { "ageq", CC1|CCGEQ },        { "AGEQ", CC1|CCGEQ },
  { "aov",  CC1|CCOV },         { "AOV",  CC1|CCOV },
  { "anov", CC1|CCNOV },        { "ANOV", CC1|CCNOV },
  { "beq",  CC1|CCB|CCEQ },     { "BEQ",  CC1|CCB|CCEQ },
  { "bneq", CC1|CCB|CCNEQ },    { "BNEQ", CC1|CCB|CCNEQ },
  { "blt",  CC1|CCB|CCLT },     { "BLT",  CC1|CCB|CCLT },
  { "bleq", CC1|CCB|CCLEQ },    { "BLEQ", CC1|CCB|CCLEQ },
  { "bgt",  CC1|CCB|CCGT },     { "BGT",  CC1|CCB|CCGT },
  { "bgeq", CC1|CCB|CCGEQ },    { "BGEQ", CC1|CCB|CCGEQ },
  { "bov",  CC1|CCB|CCOV },     { "BOV",  CC1|CCB|CCOV },
  { "bnov", CC1|CCB|CCNOV },    { "BNOV", CC1|CCB|CCNOV },
  { "tc",   CCTC },             { "TC",   CCTC },
  { "ntc",  CCNTC },            { "NTC",  CCNTC },
  { "c",    CCC },              { "C",    CCC },
  { "nc",   CCNC },             { "NC",   CCNC },
  { "bio",  CCBIO },            { "BIO",  CCBIO },
  { "nbio", CCNBIO },           { "NBIO", CCNBIO },
  { NULL, 0 }
};

const symbol cc2_codes[] = {
  { "UNC", 0 },  { "unc", 0 },
  { "AEQ", 5 },  { "aeq", 5 },
  { "ANEQ", 4 }, { "aneq", 4 },
  { "AGT", 6 },  { "agt", 6 },
  { "ALT", 3 },  { "alt", 3 },
  { "ALEQ", 7 }, { "aleq", 7 },
  { "AGEQ", 2 }, { "ageq", 2 },
  { "BEQ", 13 }, { "beq", 13 },
  { "BNEQ", 12 },{ "bneq", 12 },
  { "BGT", 14 }, { "bgt", 14 },
  { "BLT", 11 }, { "blt", 11 },
  { "BLEQ", 15 },{ "bleq", 15 },
  { "BGEQ", 10 },{ "bgeq", 10 },
  { NULL, 0 },
};

const symbol cc3_codes[] = {
  { "EQ", 0x0000 },  { "eq", 0x0000 },
  { "LT", 0x0100 },  { "lt", 0x0100 },
  { "GT", 0x0200 },  { "gt", 0x0200 },
  { "NEQ", 0x0300 }, { "neq", 0x0300 },
  { "0", 0x0000 },
  { "1", 0x0100 },
  { "2", 0x0200 },
  { "3", 0x0300 },
  { "00", 0x0000 },
  { "01", 0x0100 },
  { "10", 0x0200 },
  { "11", 0x0300 },
  { NULL, 0 },
};

/* FIXME -- also allow decimal digits */
const symbol status_bits[] = {
  /* status register 0 */
  { "TC",  12 },                { "tc",  12 },
  { "C",   11 },                { "c",   11 },
  { "OVA", 10 },                { "ova", 10 },
  { "OVB",  9 },                { "ovb",  9 },
  /* status register 1 */
  { "BRAF",15 },                { "braf",15 },
  { "CPL", 14 },                { "cpl", 14 },
  { "XF",  13 },                { "xf",  13 },
  { "HM",  12 },                { "hm",  12 },
  { "INTM",11 },                { "intm",11 },
  { "OVM",  9 },                { "ovm",  9 },
  { "SXM",  8 },                { "sxm",  8 },
  { "C16",  7 },                { "c16",  7 },
  { "FRCT", 6 },                { "frct", 6 },
  { "CMPT", 5 },                { "cmpt", 5 },
  { NULL, 0 },
};

const char *misc_symbols[] = {
  "ARP", "arp",
  "DP",  "dp",
  "ASM", "asm",
  "TS",  "ts",
  NULL
};

/* Due to the way instructions are hashed and scanned in
   gas/config/tc-tic54x.c, all identically-named opcodes must be consecutively
   placed

   Items marked with "PREFER" have been moved prior to a more costly
   instruction with a similar operand format.

   Mnemonics which can take either a predefined symbol or a memory reference
   as an argument are arranged so that the more restrictive (predefined
   symbol) version is checked first (marked "SRC").
*/
const template tic54x_unknown_opcode =
  { "???",   1,0,0,0x0000, 0x0000, {0}, 0, 0, 0};
const template tic54x_optab[] = {
  /* these must precede bc/bcd, cc/ccd to avoid misinterpretation */
  { "fb",    2,1,1,0xF880, 0xFF80, {OP_xpmad}, B_BRANCH|FL_FAR|FL_NR, 0, 0 },
  { "fbd",   2,1,1,0xFA80, 0xFF80, {OP_xpmad}, B_BRANCH|FL_FAR|FL_DELAY|FL_NR, 0, 0 },
  { "fcall", 2,1,1,0xF980, 0xFF80, {OP_xpmad}, B_BRANCH|FL_FAR|FL_NR, 0, 0 },
  { "fcalld",2,1,1,0xFB80, 0xFF80, {OP_xpmad}, B_BRANCH|FL_FAR|FL_DELAY|FL_NR, 0, 0 },

  { "abdst", 1,2,2,0xE300, 0xFF00, {OP_Xmem,OP_Ymem}, 0, 0, 0 },
  { "abs",   1,1,2,0xF485, 0xFCFF, {OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "add",   1,1,3,0xF400, 0xFCE0, {OP_SRC,OPT|OP_SHIFT,OPT|OP_DST}, 0, 0, 0 },/*SRC*/
  { "add",   1,2,3,0xF480, 0xFCFF, {OP_SRC,OP_ASM,OPT|OP_DST}, 0, 0, 0},/*SRC*/
  { "add",   1,2,2,0x0000, 0xFE00, {OP_Smem,OP_SRC1}, FL_SMR, 0, 0 },
  { "add",   1,3,3,0x0400, 0xFE00, {OP_Smem,OP_TS,OP_SRC1}, FL_SMR, 0, 0 },
  { "add",   1,3,4,0x3C00, 0xFC00, {OP_Smem,OP_16,OP_SRC,OPT|OP_DST}, FL_SMR, 0, 0 },
  { "add",   1,3,3,0x9000, 0xFE00, {OP_Xmem,OP_SHFT,OP_SRC1}, 0, 0, 0 },/*PREFER*/
  { "add",   2,2,4,0x6F00, 0xFF00, {OP_Smem,OPT|OP_SHIFT,OP_SRC,OPT|OP_DST},
    FL_EXT|FL_SMR, 0x0C00, 0xFCE0},
  { "add",   1,3,3,0xA000, 0xFE00, {OP_Xmem,OP_Ymem,OP_DST}, 0, 0, 0},
  { "add",   2,2,4,0xF000, 0xFCF0, {OP_lk,OPT|OP_SHIFT,OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "add",   2,3,4,0xF060, 0xFCFF, {OP_lk,OP_16,OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "addc",  1,2,2,0x0600, 0xFE00, {OP_Smem,OP_SRC1}, FL_SMR, 0, 0 },
  { "addm",  2,2,2,0x6B00, 0xFF00, {OP_lk,OP_Smem}, FL_NR|FL_SMR, 0, 0 },
  { "adds",  1,2,2,0x0200, 0xFE00, {OP_Smem,OP_SRC1}, FL_SMR, 0, 0 },
  { "and",   1,1,3,0xF080, 0xFCE0, {OP_SRC,OPT|OP_SHIFT,OPT|OP_DST}, 0, 0, 0 },
  { "and",   1,2,2,0x1800, 0xFE00, {OP_Smem,OP_SRC1}, FL_SMR, 0, 0  },
  { "and",   2,2,4,0xF030, 0xFCF0, {OP_lk,OPT|OP_SHFT,OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "and",   2,3,4,0xF063, 0xFCFF, {OP_lk,OP_16,OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "andm",  2,2,2,0x6800, 0xFF00, {OP_lk,OP_Smem}, FL_NR, 0, 0 },
  { "b",     2,1,1,0xF073, 0xFFFF, {OP_pmad}, B_BRANCH|FL_NR, 0, 0 },
  { "bd",    2,1,1,0xF273, 0xFFFF, {OP_pmad}, B_BRANCH|FL_DELAY|FL_NR, 0, 0 },
  { "bacc",  1,1,1,0xF4E2, 0xFEFF, {OP_SRC1}, B_BACC|FL_NR, 0, 0 },
  { "baccd", 1,1,1,0xF6E2, 0xFEFF, {OP_SRC1}, B_BACC|FL_DELAY|FL_NR, 0, 0 },
  { "banz",  2,2,2,0x6C00, 0xFF00, {OP_pmad,OP_Sind}, B_BRANCH|FL_NR, 0, 0 },
  { "banzd", 2,2,2,0x6E00, 0xFF00, {OP_pmad,OP_Sind}, B_BRANCH|FL_DELAY|FL_NR, 0, 0 },
  { "bc",    2,2,4,0xF800, 0xFF00, {OP_pmad,OP_CC,OPT|OP_CC,OPT|OP_CC},
    B_BRANCH|FL_NR, 0, 0 },
  { "bcd",   2,2,4,0xFA00, 0xFF00, {OP_pmad,OP_CC,OPT|OP_CC,OPT|OP_CC},
    B_BRANCH|FL_DELAY|FL_NR, 0, 0 },
  { "bit",   1,2,2,0x9600, 0xFF00, {OP_Xmem,OP_BITC}, 0, 0, 0 },
  { "bitf",  2,2,2,0x6100, 0xFF00, {OP_Smem,OP_lk}, FL_SMR, 0, 0 },
  { "bitt",  1,1,1,0x3400, 0xFF00, {OP_Smem}, FL_SMR, 0, 0 },
  { "cala",  1,1,1,0xF4E3, 0xFEFF, {OP_SRC1}, B_BACC|FL_NR, 0, 0 },
  { "calad", 1,1,1,0xF6E3, 0xFEFF, {OP_SRC1}, B_BACC|FL_DELAY|FL_NR, 0, 0 },
  { "call",  2,1,1,0xF074, 0xFFFF, {OP_pmad}, B_BRANCH|FL_NR, 0, 0 },
  { "calld", 2,1,1,0xF274, 0xFFFF, {OP_pmad}, B_BRANCH|FL_DELAY|FL_NR, 0, 0 },
  { "cc",    2,2,4,0xF900, 0xFF00, {OP_pmad,OP_CC,OPT|OP_CC,OPT|OP_CC},
    B_BRANCH|FL_NR, 0, 0 },
  { "ccd",   2,2,4,0xFB00, 0xFF00, {OP_pmad,OP_CC,OPT|OP_CC,OPT|OP_CC},
    B_BRANCH|FL_DELAY|FL_NR, 0, 0 },
  { "cmpl",  1,1,2,0xF493, 0xFCFF, {OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "cmpm",  2,2,2,0x6000, 0xFF00, {OP_Smem,OP_lk}, FL_SMR, 0, 0 },
  { "cmpr",  1,2,2,0xF4A8, 0xFCF8, {OP_CC3,OP_ARX}, FL_NR, 0, 0 },
  { "cmps",  1,2,2,0x8E00, 0xFE00, {OP_SRC1,OP_Smem}, 0, 0, 0 },
  { "dadd",  1,2,3,0x5000, 0xFC00, {OP_Lmem,OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "dadst", 1,2,2,0x5A00, 0xFE00, {OP_Lmem,OP_DST}, 0, 0, 0 },
  { "delay", 1,1,1,0x4D00, 0xFF00, {OP_Smem}, FL_SMR, 0, 0 },
  { "dld",   1,2,2,0x5600, 0xFE00, {OP_Lmem,OP_DST}, 0, 0, 0 },
  { "drsub", 1,2,2,0x5800, 0xFE00, {OP_Lmem,OP_SRC1}, 0, 0, 0 },
  { "dsadt", 1,2,2,0x5E00, 0xFE00, {OP_Lmem,OP_DST}, 0, 0, 0 },
  { "dst",   1,2,2,0x4E00, 0xFE00, {OP_SRC1,OP_Lmem}, FL_NR, 0, 0 },
  { "dsub",  1,2,2,0x5400, 0xFE00, {OP_Lmem,OP_SRC1}, 0, 0, 0 },
  { "dsubt", 1,2,2,0x5C00, 0xFE00, {OP_Lmem,OP_DST}, 0, 0, 0 },
  { "exp",   1,1,1,0xF48E, 0xFEFF, {OP_SRC1}, 0, 0, 0 },
  { "fbacc", 1,1,1,0xF4E6, 0xFEFF, {OP_SRC1}, B_BACC|FL_FAR|FL_NR, 0, 0 },
  { "fbaccd",1,1,1,0xF6E6, 0xFEFF, {OP_SRC1}, B_BACC|FL_FAR|FL_DELAY|FL_NR, 0, 0 },
  { "fcala", 1,1,1,0xF4E7, 0xFEFF, {OP_SRC1}, B_BACC|FL_FAR|FL_NR, 0, 0 },
  { "fcalad",1,1,1,0xF6E7, 0xFEFF, {OP_SRC1}, B_BACC|FL_FAR|FL_DELAY|FL_NR, 0, 0 },
  { "firs",  2,3,3,0xE000, 0xFF00, {OP_Xmem,OP_Ymem,OP_pmad}, 0, 0, 0 },
  { "frame", 1,1,1,0xEE00, 0xFF00, {OP_k8}, 0, 0, 0 },
  { "fret",  1,0,0,0xF4E4, 0xFFFF, {OP_None}, B_RET|FL_FAR|FL_NR, 0, 0 },
  { "fretd", 1,0,0,0xF6E4, 0xFFFF, {OP_None}, B_RET|FL_FAR|FL_DELAY|FL_NR, 0, 0 },
  { "frete", 1,0,0,0xF4E5, 0xFFFF, {OP_None}, B_RET|FL_FAR|FL_NR, 0, 0 },
  { "freted",1,0,0,0xF6E5, 0xFFFF, {OP_None}, B_RET|FL_FAR|FL_DELAY|FL_NR, 0, 0 },
  { "idle",  1,1,1,0xF4E1, 0xFCFF, {OP_123}, FL_NR, 0, 0 },
  { "intr",  1,1,1,0xF7C0, 0xFFE0, {OP_031}, B_BRANCH|FL_NR, 0, 0 },
  { "ld",    1,2,3,0xF482, 0xFCFF, {OP_SRC,OP_ASM,OPT|OP_DST}, 0, 0, 0 },/*SRC*/
  { "ld",    1,2,3,0xF440, 0xFCE0, {OP_SRC,OPT|OP_SHIFT,OP_DST}, 0, 0, 0 },/*SRC*/
  /* alternate syntax */
  { "ld",    1,2,3,0xF440, 0xFCE0, {OP_SRC,OP_SHIFT,OPT|OP_DST}, 0, 0, 0 },/*SRC*/
  { "ld",    1,2,2,0xE800, 0xFE00, {OP_k8u,OP_DST}, 0, 0, 0 },/*SRC*/
  { "ld",    1,2,2,0xED00, 0xFFE0, {OP_k5,OP_ASM}, 0, 0, 0 },/*SRC*/
  { "ld",    1,2,2,0xF4A0, 0xFFF8, {OP_k3,OP_ARP}, FL_NR, 0, 0 },/*SRC*/
  { "ld",    1,2,2,0xEA00, 0xFE00, {OP_k9,OP_DP}, FL_NR, 0, 0 },/*PREFER */
  { "ld",    1,2,2,0x3000, 0xFF00, {OP_Smem,OP_T}, FL_SMR, 0, 0 },/*SRC*/
  { "ld",    1,2,2,0x4600, 0xFF00, {OP_Smem,OP_DP}, FL_SMR, 0, 0 },/*SRC*/
  { "ld",    1,2,2,0x3200, 0xFF00, {OP_Smem,OP_ASM}, FL_SMR, 0, 0 },/*SRC*/
  { "ld",    1,2,2,0x1000, 0xFE00, {OP_Smem,OP_DST}, FL_SMR, 0, 0 },
  { "ld",    1,3,3,0x1400, 0xFE00, {OP_Smem,OP_TS,OP_DST}, FL_SMR, 0, 0 },
  { "ld",    1,3,3,0x4400, 0xFE00, {OP_Smem,OP_16,OP_DST}, FL_SMR, 0, 0 },
  { "ld",    1,3,3,0x9400, 0xFE00, {OP_Xmem,OP_SHFT,OP_DST}, 0, 0, 0 },/*PREFER*/
  { "ld",    2,2,3,0x6F00, 0xFF00, {OP_Smem,OPT|OP_SHIFT,OP_DST},
    FL_EXT|FL_SMR, 0x0C40, 0xFEE0 },
  { "ld",    2,2,3,0xF020, 0xFEF0, {OP_lk,OPT|OP_SHFT,OP_DST}, 0, 0, 0 },
  { "ld",    2,3,3,0xF062, 0xFEFF, {OP_lk,OP_16,OP_DST}, 0, 0, 0 },
  { "ldm",   1,2,2,0x4800, 0xFE00, {OP_MMR,OP_DST}, 0, 0, 0 },
  { "ldr",   1,2,2,0x1600, 0xFE00, {OP_Smem,OP_DST}, FL_SMR, 0, 0 },
  { "ldu",   1,2,2,0x1200, 0xFE00, {OP_Smem,OP_DST}, FL_SMR, 0, 0 },
  { "ldx",   2,3,3,0xF062, 0xFEFF, {OP_xpmad_ms7,OP_16,OP_DST}, FL_FAR, 0, 0},/*pseudo-op*/
  { "lms",   1,2,2,0xE100, 0xFF00, {OP_Xmem,OP_Ymem}, 0, 0, 0 },
  { "ltd",   1,1,1,0x4C00, 0xFF00, {OP_Smem}, FL_SMR, 0, 0 },
  { "mac",   1,2,2,0x2800, 0xFE00, {OP_Smem,OP_SRC1}, FL_SMR, 0, 0 },
  { "mac",   1,3,4,0xB000, 0xFC00, {OP_Xmem,OP_Ymem,OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "mac",   2,2,3,0xF067, 0xFCFF, {OP_lk,OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "mac",   2,3,4,0x6400, 0xFC00, {OP_Smem,OP_lk,OP_SRC,OPT|OP_DST}, FL_SMR, 0, 0 },
  { "macr",  1,2,2,0x2A00, 0xFE00, {OP_Smem,OP_SRC1}, FL_SMR, 0, 0 },
  { "macr",  1,3,4,0xB400, 0xFC00, {OP_Xmem,OP_Ymem,OP_SRC,OPT|OP_DST},FL_SMR, 0, 0 },
  { "maca",  1,2,3,0xF488, 0xFCFF, {OP_T,OP_SRC,OPT|OP_DST}, FL_SMR, 0, 0 },/*SRC*/
  { "maca",  1,1,2,0x3500, 0xFF00, {OP_Smem,OPT|OP_B}, FL_SMR, 0, 0 },
  { "macar", 1,2,3,0xF489, 0xFCFF, {OP_T,OP_SRC,OPT|OP_DST}, FL_SMR, 0, 0 },/*SRC*/
  { "macar", 1,1,2,0x3700, 0xFF00, {OP_Smem,OPT|OP_B}, FL_SMR, 0, 0 },
  { "macd",  2,3,3,0x7A00, 0xFE00, {OP_Smem,OP_pmad,OP_SRC1}, FL_SMR, 0, 0 },
  { "macp",  2,3,3,0x7800, 0xFE00, {OP_Smem,OP_pmad,OP_SRC1}, FL_SMR, 0, 0 },
  { "macsu", 1,3,3,0xA600, 0xFE00, {OP_Xmem,OP_Ymem,OP_SRC1}, 0, 0, 0 },
  { "mar",   1,1,1,0x6D00, 0xFF00, {OP_Smem}, 0, 0, 0 },
  { "mas",   1,2,2,0x2C00, 0xFE00, {OP_Smem,OP_SRC1}, FL_SMR, 0, 0 },
  { "mas",   1,3,4,0xB800, 0xFC00, {OP_Xmem,OP_Ymem,OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "masr",  1,2,2,0x2E00, 0xFE00, {OP_Smem,OP_SRC1}, FL_SMR, 0, 0 },
  { "masr",  1,3,4,0xBC00, 0xFC00, {OP_Xmem,OP_Ymem,OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "masa",  1,2,3,0xF48A, 0xFCFF, {OP_T,OP_SRC,OPT|OP_DST}, 0, 0, 0 },/*SRC*/
  { "masa",  1,1,2,0x3300, 0xFF00, {OP_Smem,OPT|OP_B}, FL_SMR, 0, 0 },
  { "masar", 1,2,3,0xF48B, 0xFCFF, {OP_T,OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "max",   1,1,1,0xF486, 0xFEFF, {OP_DST}, 0, 0, 0 },
  { "min",   1,1,1,0xF487, 0xFEFF, {OP_DST}, 0, 0, 0 },
  { "mpy",   1,2,2,0x2000, 0xFE00, {OP_Smem,OP_DST}, FL_SMR, 0, 0 },
  { "mpy",   1,3,3,0xA400, 0xFE00, {OP_Xmem,OP_Ymem,OP_DST}, 0, 0, 0 },
  { "mpy",   2,3,3,0x6200, 0xFE00, {OP_Smem,OP_lk,OP_DST}, FL_SMR, 0, 0 },
  { "mpy",   2,2,2,0xF066, 0xFEFF, {OP_lk,OP_DST}, 0, 0, 0 },
  { "mpyr",  1,2,2,0x2200, 0xFE00, {OP_Smem,OP_DST}, FL_SMR, 0, 0 },
  { "mpya",  1,1,1,0xF48C, 0xFEFF, {OP_DST}, 0, 0, 0 }, /*SRC*/
  { "mpya",  1,1,1,0x3100, 0xFF00, {OP_Smem}, FL_SMR, 0, 0 },
  { "mpyu",  1,2,2,0x2400, 0xFE00, {OP_Smem,OP_DST}, FL_SMR, 0, 0 },
  { "mvdd",  1,2,2,0xE500, 0xFF00, {OP_Xmem,OP_Ymem}, 0, 0, 0 },
  { "mvdk",  2,2,2,0x7100, 0xFF00, {OP_Smem,OP_dmad}, FL_SMR, 0, 0 },
  { "mvdm",  2,2,2,0x7200, 0xFF00, {OP_dmad,OP_MMR}, 0, 0, 0 },
  { "mvdp",  2,2,2,0x7D00, 0xFF00, {OP_Smem,OP_pmad}, FL_SMR, 0, 0 },
  { "mvkd",  2,2,2,0x7000, 0xFF00, {OP_dmad,OP_Smem}, 0, 0, 0 },
  { "mvmd",  2,2,2,0x7300, 0xFF00, {OP_MMR,OP_dmad}, 0, 0, 0 },
  { "mvmm",  1,2,2,0xE700, 0xFF00, {OP_MMRX,OP_MMRY}, FL_NR, 0, 0 },
  { "mvpd",  2,2,2,0x7C00, 0xFF00, {OP_pmad,OP_Smem}, 0, 0, 0 },
  { "neg",   1,1,2,0xF484, 0xFCFF, {OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "nop",   1,0,0,0xF495, 0xFFFF, {OP_None}, 0, 0, 0 },
  { "norm",  1,1,2,0xF48F, 0xFCFF, {OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "or",    1,1,3,0xF0A0, 0xFCE0, {OP_SRC,OPT|OP_SHIFT,OPT|OP_DST}, 0, 0, 0 },/*SRC*/
  { "or",    1,2,2,0x1A00, 0xFE00, {OP_Smem,OP_SRC1}, FL_SMR, 0, 0 },
  { "or",    2,2,4,0xF040, 0xFCF0, {OP_lk,OPT|OP_SHFT,OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "or",    2,3,4,0xF064, 0xFCFF, {OP_lk,OP_16,OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "orm",   2,2,2,0x6900, 0xFF00, {OP_lk,OP_Smem}, FL_NR|FL_SMR, 0, 0 },
  { "poly",  1,1,1,0x3600, 0xFF00, {OP_Smem}, FL_SMR, 0, 0 },
  { "popd",  1,1,1,0x8B00, 0xFF00, {OP_Smem}, 0, 0, 0 },
  { "popm",  1,1,1,0x8A00, 0xFF00, {OP_MMR}, 0, 0, 0 },
  { "portr", 2,2,2,0x7400, 0xFF00, {OP_PA,OP_Smem}, 0, 0, 0 },
  { "portw", 2,2,2,0x7500, 0xFF00, {OP_Smem,OP_PA}, FL_SMR, 0, 0 },
  { "pshd",  1,1,1,0x4B00, 0xFF00, {OP_Smem}, FL_SMR, 0, 0 },
  { "pshm",  1,1,1,0x4A00, 0xFF00, {OP_MMR}, 0, 0, 0 },
  { "ret",   1,0,0,0xFC00, 0xFFFF, {OP_None}, B_RET|FL_NR, 0, 0 },
  { "retd",  1,0,0,0xFE00, 0xFFFF, {OP_None}, B_RET|FL_DELAY|FL_NR, 0, 0 },
  { "rc",    1,1,3,0xFC00, 0xFF00, {OP_CC,OPT|OP_CC,OPT|OP_CC},
    B_RET|FL_NR, 0, 0 },
  { "rcd",   1,1,3,0xFE00, 0xFF00, {OP_CC,OPT|OP_CC,OPT|OP_CC},
    B_RET|FL_DELAY|FL_NR, 0, 0 },
  { "reada", 1,1,1,0x7E00, 0xFF00, {OP_Smem}, 0, 0, 0 },
  { "reset", 1,0,0,0xF7E0, 0xFFFF, {OP_None}, FL_NR, 0, 0 },
  { "rete",  1,0,0,0xF4EB, 0xFFFF, {OP_None}, B_RET|FL_NR, 0, 0 },
  { "reted", 1,0,0,0xF6EB, 0xFFFF, {OP_None}, B_RET|FL_DELAY|FL_NR, 0, 0 },
  { "retf",  1,0,0,0xF49B, 0xFFFF, {OP_None}, B_RET|FL_NR, 0, 0 },
  { "retfd", 1,0,0,0xF69B, 0xFFFF, {OP_None}, B_RET|FL_DELAY|FL_NR, 0, 0 },
  { "rnd",   1,1,2,0xF49F, 0xFCFF, {OP_SRC,OPT|OP_DST}, FL_LP|FL_NR, 0, 0 },
  { "rol",   1,1,1,0xF491, 0xFEFF, {OP_SRC1}, 0, 0, 0 },
  { "roltc", 1,1,1,0xF492, 0xFEFF, {OP_SRC1}, 0, 0, 0 },
  { "ror",   1,1,1,0xF490, 0xFEFF, {OP_SRC1}, 0, 0, 0 },
  { "rpt",   1,1,1,0x4700, 0xFF00, {OP_Smem}, B_REPEAT|FL_NR|FL_SMR, 0, 0 },
  { "rpt",   1,1,1,0xEC00, 0xFF00, {OP_k8u}, B_REPEAT|FL_NR, 0, 0 },
  { "rpt",   2,1,1,0xF070, 0xFFFF, {OP_lku}, B_REPEAT|FL_NR, 0, 0 },
  { "rptb",  2,1,1,0xF072, 0xFFFF, {OP_pmad}, FL_NR, 0, 0 },
  { "rptbd", 2,1,1,0xF272, 0xFFFF, {OP_pmad}, FL_DELAY|FL_NR, 0, 0 },
  { "rptz",  2,2,2,0xF071, 0xFEFF, {OP_DST,OP_lku}, B_REPEAT|FL_NR, 0, 0 },
  { "rsbx",  1,1,2,0xF4B0, 0xFDF0, {OPT|OP_N,OP_SBIT}, FL_NR, 0, 0 },
  { "saccd", 1,3,3,0x9E00, 0xFE00, {OP_SRC1,OP_Xmem,OP_CC2}, 0, 0, 0 },
  { "sat",   1,1,1,0xF483, 0xFEFF, {OP_SRC1}, 0, 0, 0 },
  { "sfta",  1,2,3,0xF460, 0xFCE0, {OP_SRC,OP_SHIFT,OPT|OP_DST}, 0, 0, 0 },
  { "sftc",  1,1,1,0xF494, 0xFEFF, {OP_SRC1}, 0, 0, 0 },
  { "sftl",  1,2,3,0xF0E0, 0xFCE0, {OP_SRC,OP_SHIFT,OPT|OP_DST}, 0, 0, 0 },
  { "sqdst", 1,2,2,0xE200, 0xFF00, {OP_Xmem,OP_Ymem}, 0, 0, 0 },
  { "squr",  1,2,2,0xF48D, 0xFEFF, {OP_A,OP_DST}, 0, 0, 0 },/*SRC*/
  { "squr",  1,2,2,0x2600, 0xFE00, {OP_Smem,OP_DST}, FL_SMR, 0, 0 },
  { "squra", 1,2,2,0x3800, 0xFE00, {OP_Smem,OP_SRC1}, FL_SMR, 0, 0 },
  { "squrs", 1,2,2,0x3A00, 0xFE00, {OP_Smem,OP_SRC1}, FL_SMR, 0, 0 },
  { "srccd", 1,2,2,0x9D00, 0xFF00, {OP_Xmem,OP_CC2}, 0, 0, 0 },
  { "ssbx",  1,1,2,0xF5B0, 0xFDF0, {OPT|OP_N,OP_SBIT}, FL_NR, 0, 0 },
  { "st",    1,2,2,0x8C00, 0xFF00, {OP_T,OP_Smem}, 0, 0, 0 },
  { "st",    1,2,2,0x8D00, 0xFF00, {OP_TRN,OP_Smem}, 0, 0, 0 },
  { "st",    2,2,2,0x7600, 0xFF00, {OP_lk,OP_Smem}, 0, 0, 0 },
  { "sth",   1,2,2,0x8200, 0xFE00, {OP_SRC1,OP_Smem}, 0, 0, 0 },
  { "sth",   1,3,3,0x8600, 0xFE00, {OP_SRC1,OP_ASM,OP_Smem}, 0, 0, 0 },
  { "sth",   1,3,3,0x9A00, 0xFE00, {OP_SRC1,OP_SHFT,OP_Xmem}, 0, 0, 0 },
  { "sth",   2,2,3,0x6F00, 0xFF00, {OP_SRC1,OPT|OP_SHIFT,OP_Smem},
    FL_EXT, 0x0C60, 0xFEE0 },
  { "stl",   1,2,2,0x8000, 0xFE00, {OP_SRC1,OP_Smem}, 0, 0, 0 },
  { "stl",   1,3,3,0x8400, 0xFE00, {OP_SRC1,OP_ASM,OP_Smem}, 0, 0, 0 },
  { "stl",   1,3,3,0x9800, 0xFE00, {OP_SRC1,OP_SHFT,OP_Xmem}, 0, 0, 0 },
  { "stl",   2,2,3,0x6F00, 0xFF00, {OP_SRC1,OPT|OP_SHIFT,OP_Smem},
    FL_EXT, 0x0C80, 0xFEE0 },
  { "stlm",  1,2,2,0x8800, 0xFE00, {OP_SRC1,OP_MMR}, 0, 0, 0 },
  { "stm",   2,2,2,0x7700, 0xFF00, {OP_lk,OP_MMR}, 0, 0, 0 },
  { "strcd", 1,2,2,0x9C00, 0xFF00, {OP_Xmem,OP_CC2}, 0, 0, 0 },
  { "sub",   1,1,3,0xF420, 0xFCE0, {OP_SRC,OPT|OP_SHIFT,OPT|OP_DST}, 0, 0, 0 },/*SRC*/
  { "sub",   1,2,3,0xF481, 0xFCFF, {OP_SRC,OP_ASM,OPT|OP_DST}, 0, 0, 0 },/*SRC*/
  { "sub",   1,2,2,0x0800, 0xFE00, {OP_Smem,OP_SRC1}, FL_SMR, 0, 0 },
  { "sub",   1,3,3,0x0C00, 0xFE00, {OP_Smem,OP_TS,OP_SRC1}, FL_SMR, 0, 0 },
  { "sub",   1,3,4,0x4000, 0xFC00, {OP_Smem,OP_16,OP_SRC,OPT|OP_DST}, FL_SMR, 0, 0 },
  { "sub",   1,3,3,0x9200, 0xFE00, {OP_Xmem,OP_SHFT,OP_SRC1}, 0, 0, 0 }, /*PREFER*/
  { "sub",   2,2,4,0x6F00, 0xFF00, {OP_Smem,OPT|OP_SHIFT,OP_SRC,OPT|OP_DST},
    FL_EXT|FL_SMR, 0x0C20, 0xFCE0 },
  { "sub",   1,3,3,0xA200, 0xFE00, {OP_Xmem,OP_Ymem,OP_DST}, 0, 0, 0 },
  { "sub",   2,2,4,0xF010, 0xFCF0, {OP_lk,OPT|OP_SHFT,OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "sub",   2,3,4,0xF061, 0xFCFF, {OP_lk,OP_16,OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "subb",  1,2,2,0x0E00, 0xFE00, {OP_Smem,OP_SRC1}, FL_SMR, 0, 0 },
  { "subc",  1,2,2,0x1E00, 0xFE00, {OP_Smem,OP_SRC1}, FL_SMR, 0, 0 },
  { "subs",  1,2,2,0x0A00, 0xFE00, {OP_Smem,OP_SRC1}, FL_SMR, 0, 0 },
  { "trap",  1,1,1,0xF4C0, 0xFFE0, {OP_031}, B_BRANCH|FL_NR, 0, 0 },
  { "writa", 1,1,1,0x7F00, 0xFF00, {OP_Smem}, FL_SMR, 0, 0 },
  { "xc",    1,2,4,0xFD00, 0xFD00, {OP_12,OP_CC,OPT|OP_CC,OPT|OP_CC}, FL_NR, 0, 0 },
  { "xor",   1,1,3,0xF0C0, 0xFCE0, {OP_SRC,OPT|OP_SHIFT,OPT|OP_DST}, 0, 0, 0 },/*SRC*/
  { "xor",   1,2,2,0x1C00, 0xFE00, {OP_Smem,OP_SRC1}, FL_SMR, 0, 0 },
  { "xor",   2,2,4,0xF050, 0xFCF0, {OP_lku,OPT|OP_SHFT,OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "xor",   2,3,4,0xF065, 0xFCFF, {OP_lku,OP_16,OP_SRC,OPT|OP_DST}, 0, 0, 0 },
  { "xorm",  2,2,2,0x6A00, 0xFF00, {OP_lku,OP_Smem}, FL_NR|FL_SMR, 0, 0 },
  { NULL, 0,0,0,0,0, {}, 0, 0, 0 },
};

/* assume all parallel instructions have at least three operands */
const partemplate tic54x_paroptab[] = {
  { "ld","mac", 1,1,2,0xA800, 0xFE00, {OP_Xmem,OP_DST},{OP_Ymem,OPT|OP_RND},},
  { "ld","macr",1,1,2,0xAA00, 0xFE00, {OP_Xmem,OP_DST},{OP_Ymem,OPT|OP_RND},},
  { "ld","mas", 1,1,2,0xAC00, 0xFE00, {OP_Xmem,OP_DST},{OP_Ymem,OPT|OP_RND},},
  { "ld","masr",1,1,2,0xAE00, 0xFE00, {OP_Xmem,OP_DST},{OP_Ymem,OPT|OP_RND},},
  { "st","add", 1,2,2,0xC000, 0xFC00, {OP_SRC,OP_Ymem},{OP_Xmem,OP_DST}, },
  { "st","ld",  1,2,2,0xC800, 0xFC00, {OP_SRC,OP_Ymem},{OP_Xmem,OP_DST}, },
  { "st","ld",  1,2,2,0xE400, 0xFC00, {OP_SRC,OP_Ymem},{OP_Xmem,OP_T}, },
  { "st","mac", 1,2,2,0xD000, 0xFC00, {OP_SRC,OP_Ymem},{OP_Xmem,OP_DST}, },
  { "st","macr",1,2,2,0xD400, 0xFC00, {OP_SRC,OP_Ymem},{OP_Xmem,OP_DST}, },
  { "st","mas", 1,2,2,0xD800, 0xFC00, {OP_SRC,OP_Ymem},{OP_Xmem,OP_DST}, },
  { "st","masr",1,2,2,0xDC00, 0xFC00, {OP_SRC,OP_Ymem},{OP_Xmem,OP_DST}, },
  { "st","mpy", 1,2,2,0xCC00, 0xFC00, {OP_SRC,OP_Ymem},{OP_Xmem,OP_DST}, },
  { "st","sub", 1,2,2,0xC400, 0xFC00, {OP_SRC,OP_Ymem},{OP_Xmem,OP_DST}, },
  { NULL,NULL, 0, 0, 0, 0, 0, {}, {}, },
};
