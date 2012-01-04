/* Simulator header for sh.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright 1996-2010, 2012 Free Software Foundation, Inc.

This file is part of the GNU simulators.

   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.

*/

#ifndef SH_ARCH_H
#define SH_ARCH_H

#define TARGET_BIG_ENDIAN 1

/* Enum declaration for model types.  */
typedef enum model_type {
  MODEL_SH2A_NOFPU, MODEL_SH2A_FPU, MODEL_SH4_NOFPU, MODEL_SH4
 , MODEL_SH4A_NOFPU, MODEL_SH4A, MODEL_SH4AL, MODEL_SH5
 , MODEL_SH5_MEDIA, MODEL_SH2, MODEL_SH2E, MODEL_SH3
 , MODEL_SH3E, MODEL_MAX
} MODEL_TYPE;

#define MAX_MODELS ((int) MODEL_MAX)

/* Enum declaration for unit types.  */
typedef enum unit_type {
  UNIT_NONE, UNIT_SH2A_NOFPU_U_MULR_GR, UNIT_SH2A_NOFPU_U_MULR, UNIT_SH2A_NOFPU_U_TRAP
 , UNIT_SH2A_NOFPU_U_WRITE_BACK, UNIT_SH2A_NOFPU_U_USE_MULTIPLY_RESULT, UNIT_SH2A_NOFPU_U_SHIFT, UNIT_SH2A_NOFPU_U_TAS
 , UNIT_SH2A_NOFPU_U_MULSW, UNIT_SH2A_NOFPU_U_MULL, UNIT_SH2A_NOFPU_U_DMUL, UNIT_SH2A_NOFPU_U_MACL
 , UNIT_SH2A_NOFPU_U_MACW, UNIT_SH2A_NOFPU_U_MULTIPLY, UNIT_SH2A_NOFPU_U_SET_MAC, UNIT_SH2A_NOFPU_U_LOAD_MAC
 , UNIT_SH2A_NOFPU_U_LOAD_VBR, UNIT_SH2A_NOFPU_U_LOAD_GBR, UNIT_SH2A_NOFPU_U_USE_GR, UNIT_SH2A_NOFPU_U_LOAD_GR
 , UNIT_SH2A_NOFPU_U_STC_VBR, UNIT_SH2A_NOFPU_U_LDCL_VBR, UNIT_SH2A_NOFPU_U_LDCL, UNIT_SH2A_NOFPU_U_USE_TBIT
 , UNIT_SH2A_NOFPU_U_LDC_GBR, UNIT_SH2A_NOFPU_U_LDC_SR, UNIT_SH2A_NOFPU_U_SET_SR_BIT, UNIT_SH2A_NOFPU_U_USE_PR
 , UNIT_SH2A_NOFPU_U_LOAD_PR, UNIT_SH2A_NOFPU_U_STS_PR, UNIT_SH2A_NOFPU_U_LDS_PR, UNIT_SH2A_NOFPU_U_MEMORY_ACCESS
 , UNIT_SH2A_NOFPU_U_LOGIC_B, UNIT_SH2A_NOFPU_U_JSR, UNIT_SH2A_NOFPU_U_JMP, UNIT_SH2A_NOFPU_U_BRANCH
 , UNIT_SH2A_NOFPU_U_SX, UNIT_SH2A_NOFPU_U_EXEC, UNIT_SH2A_FPU_U_USE_DR, UNIT_SH2A_FPU_U_LOAD_DR
 , UNIT_SH2A_FPU_U_SET_DR, UNIT_SH2A_FPU_U_MULR_GR, UNIT_SH2A_FPU_U_MULR, UNIT_SH2A_FPU_U_FCNV
 , UNIT_SH2A_FPU_U_FCMP, UNIT_SH2A_FPU_U_FSQRT, UNIT_SH2A_FPU_U_FDIV, UNIT_SH2A_FPU_U_FPU_LOAD_GR
 , UNIT_SH2A_FPU_U_USE_FPSCR, UNIT_SH2A_FPU_U_LDSL_FPSCR, UNIT_SH2A_FPU_U_LDS_FPSCR, UNIT_SH2A_FPU_U_USE_FPUL
 , UNIT_SH2A_FPU_U_FLDS_FPUL, UNIT_SH2A_FPU_U_LOAD_FPUL, UNIT_SH2A_FPU_U_SET_FPUL, UNIT_SH2A_FPU_U_FPU_MEMORY_ACCESS
 , UNIT_SH2A_FPU_U_USE_FR, UNIT_SH2A_FPU_U_SET_FR_0, UNIT_SH2A_FPU_U_SET_FR, UNIT_SH2A_FPU_U_LOAD_FR
 , UNIT_SH2A_FPU_U_MAYBE_FPU, UNIT_SH2A_FPU_U_FPU, UNIT_SH2A_FPU_U_TRAP, UNIT_SH2A_FPU_U_WRITE_BACK
 , UNIT_SH2A_FPU_U_USE_MULTIPLY_RESULT, UNIT_SH2A_FPU_U_SHIFT, UNIT_SH2A_FPU_U_TAS, UNIT_SH2A_FPU_U_MULSW
 , UNIT_SH2A_FPU_U_MULL, UNIT_SH2A_FPU_U_DMUL, UNIT_SH2A_FPU_U_MACL, UNIT_SH2A_FPU_U_MACW
 , UNIT_SH2A_FPU_U_MULTIPLY, UNIT_SH2A_FPU_U_SET_MAC, UNIT_SH2A_FPU_U_LOAD_MAC, UNIT_SH2A_FPU_U_LOAD_VBR
 , UNIT_SH2A_FPU_U_LOAD_GBR, UNIT_SH2A_FPU_U_USE_GR, UNIT_SH2A_FPU_U_LOAD_GR, UNIT_SH2A_FPU_U_STC_VBR
 , UNIT_SH2A_FPU_U_LDCL_VBR, UNIT_SH2A_FPU_U_LDCL, UNIT_SH2A_FPU_U_USE_TBIT, UNIT_SH2A_FPU_U_LDC_GBR
 , UNIT_SH2A_FPU_U_LDC_SR, UNIT_SH2A_FPU_U_SET_SR_BIT, UNIT_SH2A_FPU_U_USE_PR, UNIT_SH2A_FPU_U_LOAD_PR
 , UNIT_SH2A_FPU_U_STS_PR, UNIT_SH2A_FPU_U_LDS_PR, UNIT_SH2A_FPU_U_MEMORY_ACCESS, UNIT_SH2A_FPU_U_LOGIC_B
 , UNIT_SH2A_FPU_U_JSR, UNIT_SH2A_FPU_U_JMP, UNIT_SH2A_FPU_U_BRANCH, UNIT_SH2A_FPU_U_SX
 , UNIT_SH2A_FPU_U_EXEC, UNIT_SH4_NOFPU_U_OCB, UNIT_SH4_NOFPU_U_MULR_GR, UNIT_SH4_NOFPU_U_MULR
 , UNIT_SH4_NOFPU_U_TRAP, UNIT_SH4_NOFPU_U_WRITE_BACK, UNIT_SH4_NOFPU_U_USE_MULTIPLY_RESULT, UNIT_SH4_NOFPU_U_SHIFT
 , UNIT_SH4_NOFPU_U_TAS, UNIT_SH4_NOFPU_U_MULSW, UNIT_SH4_NOFPU_U_MULL, UNIT_SH4_NOFPU_U_DMUL
 , UNIT_SH4_NOFPU_U_MACL, UNIT_SH4_NOFPU_U_MACW, UNIT_SH4_NOFPU_U_MULTIPLY, UNIT_SH4_NOFPU_U_SET_MAC
 , UNIT_SH4_NOFPU_U_LOAD_MAC, UNIT_SH4_NOFPU_U_LOAD_VBR, UNIT_SH4_NOFPU_U_LOAD_GBR, UNIT_SH4_NOFPU_U_USE_GR
 , UNIT_SH4_NOFPU_U_LOAD_GR, UNIT_SH4_NOFPU_U_STC_VBR, UNIT_SH4_NOFPU_U_LDCL_VBR, UNIT_SH4_NOFPU_U_LDCL
 , UNIT_SH4_NOFPU_U_USE_TBIT, UNIT_SH4_NOFPU_U_LDC_GBR, UNIT_SH4_NOFPU_U_LDC_SR, UNIT_SH4_NOFPU_U_SET_SR_BIT
 , UNIT_SH4_NOFPU_U_USE_PR, UNIT_SH4_NOFPU_U_LOAD_PR, UNIT_SH4_NOFPU_U_STS_PR, UNIT_SH4_NOFPU_U_LDS_PR
 , UNIT_SH4_NOFPU_U_MEMORY_ACCESS, UNIT_SH4_NOFPU_U_LOGIC_B, UNIT_SH4_NOFPU_U_JSR, UNIT_SH4_NOFPU_U_JMP
 , UNIT_SH4_NOFPU_U_BRANCH, UNIT_SH4_NOFPU_U_SX, UNIT_SH4_NOFPU_U_EXEC, UNIT_SH4_U_FTRV
 , UNIT_SH4_U_FIPR, UNIT_SH4_U_OCB, UNIT_SH4_U_MULR_GR, UNIT_SH4_U_MULR
 , UNIT_SH4_U_USE_DR, UNIT_SH4_U_LOAD_DR, UNIT_SH4_U_SET_DR, UNIT_SH4_U_FCNV
 , UNIT_SH4_U_FCMP, UNIT_SH4_U_FSQRT, UNIT_SH4_U_FDIV, UNIT_SH4_U_FPU_LOAD_GR
 , UNIT_SH4_U_USE_FPSCR, UNIT_SH4_U_LDSL_FPSCR, UNIT_SH4_U_LDS_FPSCR, UNIT_SH4_U_USE_FPUL
 , UNIT_SH4_U_FLDS_FPUL, UNIT_SH4_U_LOAD_FPUL, UNIT_SH4_U_SET_FPUL, UNIT_SH4_U_FPU_MEMORY_ACCESS
 , UNIT_SH4_U_USE_FR, UNIT_SH4_U_SET_FR_0, UNIT_SH4_U_SET_FR, UNIT_SH4_U_LOAD_FR
 , UNIT_SH4_U_MAYBE_FPU, UNIT_SH4_U_FPU, UNIT_SH4_U_TRAP, UNIT_SH4_U_WRITE_BACK
 , UNIT_SH4_U_USE_MULTIPLY_RESULT, UNIT_SH4_U_SHIFT, UNIT_SH4_U_TAS, UNIT_SH4_U_MULSW
 , UNIT_SH4_U_MULL, UNIT_SH4_U_DMUL, UNIT_SH4_U_MACL, UNIT_SH4_U_MACW
 , UNIT_SH4_U_MULTIPLY, UNIT_SH4_U_SET_MAC, UNIT_SH4_U_LOAD_MAC, UNIT_SH4_U_LOAD_VBR
 , UNIT_SH4_U_LOAD_GBR, UNIT_SH4_U_USE_GR, UNIT_SH4_U_LOAD_GR, UNIT_SH4_U_STC_VBR
 , UNIT_SH4_U_LDCL_VBR, UNIT_SH4_U_LDCL, UNIT_SH4_U_USE_TBIT, UNIT_SH4_U_LDC_GBR
 , UNIT_SH4_U_LDC_SR, UNIT_SH4_U_SET_SR_BIT, UNIT_SH4_U_USE_PR, UNIT_SH4_U_LOAD_PR
 , UNIT_SH4_U_STS_PR, UNIT_SH4_U_LDS_PR, UNIT_SH4_U_MEMORY_ACCESS, UNIT_SH4_U_LOGIC_B
 , UNIT_SH4_U_JSR, UNIT_SH4_U_JMP, UNIT_SH4_U_BRANCH, UNIT_SH4_U_SX
 , UNIT_SH4_U_EXEC, UNIT_SH4A_NOFPU_U_OCB, UNIT_SH4A_NOFPU_U_MULR_GR, UNIT_SH4A_NOFPU_U_MULR
 , UNIT_SH4A_NOFPU_U_FCNV, UNIT_SH4A_NOFPU_U_FCMP, UNIT_SH4A_NOFPU_U_FSQRT, UNIT_SH4A_NOFPU_U_FDIV
 , UNIT_SH4A_NOFPU_U_FPU_LOAD_GR, UNIT_SH4A_NOFPU_U_USE_FPSCR, UNIT_SH4A_NOFPU_U_LDSL_FPSCR, UNIT_SH4A_NOFPU_U_LDS_FPSCR
 , UNIT_SH4A_NOFPU_U_USE_FPUL, UNIT_SH4A_NOFPU_U_FLDS_FPUL, UNIT_SH4A_NOFPU_U_LOAD_FPUL, UNIT_SH4A_NOFPU_U_SET_FPUL
 , UNIT_SH4A_NOFPU_U_FPU_MEMORY_ACCESS, UNIT_SH4A_NOFPU_U_USE_FR, UNIT_SH4A_NOFPU_U_SET_FR_0, UNIT_SH4A_NOFPU_U_SET_FR
 , UNIT_SH4A_NOFPU_U_LOAD_FR, UNIT_SH4A_NOFPU_U_MAYBE_FPU, UNIT_SH4A_NOFPU_U_FPU, UNIT_SH4A_NOFPU_U_TRAP
 , UNIT_SH4A_NOFPU_U_WRITE_BACK, UNIT_SH4A_NOFPU_U_USE_MULTIPLY_RESULT, UNIT_SH4A_NOFPU_U_SHIFT, UNIT_SH4A_NOFPU_U_TAS
 , UNIT_SH4A_NOFPU_U_MULSW, UNIT_SH4A_NOFPU_U_MULL, UNIT_SH4A_NOFPU_U_DMUL, UNIT_SH4A_NOFPU_U_MACL
 , UNIT_SH4A_NOFPU_U_MACW, UNIT_SH4A_NOFPU_U_MULTIPLY, UNIT_SH4A_NOFPU_U_SET_MAC, UNIT_SH4A_NOFPU_U_LOAD_MAC
 , UNIT_SH4A_NOFPU_U_LOAD_VBR, UNIT_SH4A_NOFPU_U_LOAD_GBR, UNIT_SH4A_NOFPU_U_USE_GR, UNIT_SH4A_NOFPU_U_LOAD_GR
 , UNIT_SH4A_NOFPU_U_STC_VBR, UNIT_SH4A_NOFPU_U_LDCL_VBR, UNIT_SH4A_NOFPU_U_LDCL, UNIT_SH4A_NOFPU_U_USE_TBIT
 , UNIT_SH4A_NOFPU_U_LDC_GBR, UNIT_SH4A_NOFPU_U_LDC_SR, UNIT_SH4A_NOFPU_U_SET_SR_BIT, UNIT_SH4A_NOFPU_U_USE_PR
 , UNIT_SH4A_NOFPU_U_LOAD_PR, UNIT_SH4A_NOFPU_U_STS_PR, UNIT_SH4A_NOFPU_U_LDS_PR, UNIT_SH4A_NOFPU_U_MEMORY_ACCESS
 , UNIT_SH4A_NOFPU_U_LOGIC_B, UNIT_SH4A_NOFPU_U_JSR, UNIT_SH4A_NOFPU_U_JMP, UNIT_SH4A_NOFPU_U_BRANCH
 , UNIT_SH4A_NOFPU_U_SX, UNIT_SH4A_NOFPU_U_EXEC, UNIT_SH4A_U_FTRV, UNIT_SH4A_U_FIPR
 , UNIT_SH4A_U_OCB, UNIT_SH4A_U_MULR_GR, UNIT_SH4A_U_MULR, UNIT_SH4A_U_FCNV
 , UNIT_SH4A_U_FCMP, UNIT_SH4A_U_FSQRT, UNIT_SH4A_U_FDIV, UNIT_SH4A_U_FPU_LOAD_GR
 , UNIT_SH4A_U_USE_FPSCR, UNIT_SH4A_U_LDSL_FPSCR, UNIT_SH4A_U_LDS_FPSCR, UNIT_SH4A_U_USE_FPUL
 , UNIT_SH4A_U_FLDS_FPUL, UNIT_SH4A_U_LOAD_FPUL, UNIT_SH4A_U_SET_FPUL, UNIT_SH4A_U_FPU_MEMORY_ACCESS
 , UNIT_SH4A_U_USE_FR, UNIT_SH4A_U_SET_FR_0, UNIT_SH4A_U_SET_FR, UNIT_SH4A_U_LOAD_FR
 , UNIT_SH4A_U_MAYBE_FPU, UNIT_SH4A_U_FPU, UNIT_SH4A_U_TRAP, UNIT_SH4A_U_WRITE_BACK
 , UNIT_SH4A_U_USE_MULTIPLY_RESULT, UNIT_SH4A_U_SHIFT, UNIT_SH4A_U_TAS, UNIT_SH4A_U_MULSW
 , UNIT_SH4A_U_MULL, UNIT_SH4A_U_DMUL, UNIT_SH4A_U_MACL, UNIT_SH4A_U_MACW
 , UNIT_SH4A_U_MULTIPLY, UNIT_SH4A_U_SET_MAC, UNIT_SH4A_U_LOAD_MAC, UNIT_SH4A_U_LOAD_VBR
 , UNIT_SH4A_U_LOAD_GBR, UNIT_SH4A_U_USE_GR, UNIT_SH4A_U_LOAD_GR, UNIT_SH4A_U_STC_VBR
 , UNIT_SH4A_U_LDCL_VBR, UNIT_SH4A_U_LDCL, UNIT_SH4A_U_USE_TBIT, UNIT_SH4A_U_LDC_GBR
 , UNIT_SH4A_U_LDC_SR, UNIT_SH4A_U_SET_SR_BIT, UNIT_SH4A_U_USE_PR, UNIT_SH4A_U_LOAD_PR
 , UNIT_SH4A_U_STS_PR, UNIT_SH4A_U_LDS_PR, UNIT_SH4A_U_MEMORY_ACCESS, UNIT_SH4A_U_LOGIC_B
 , UNIT_SH4A_U_JSR, UNIT_SH4A_U_JMP, UNIT_SH4A_U_BRANCH, UNIT_SH4A_U_SX
 , UNIT_SH4A_U_EXEC, UNIT_SH4AL_U_OCB, UNIT_SH4AL_U_MULR_GR, UNIT_SH4AL_U_MULR
 , UNIT_SH4AL_U_FCNV, UNIT_SH4AL_U_FCMP, UNIT_SH4AL_U_FSQRT, UNIT_SH4AL_U_FDIV
 , UNIT_SH4AL_U_FPU_LOAD_GR, UNIT_SH4AL_U_USE_FPSCR, UNIT_SH4AL_U_LDSL_FPSCR, UNIT_SH4AL_U_LDS_FPSCR
 , UNIT_SH4AL_U_USE_FPUL, UNIT_SH4AL_U_FLDS_FPUL, UNIT_SH4AL_U_LOAD_FPUL, UNIT_SH4AL_U_SET_FPUL
 , UNIT_SH4AL_U_FPU_MEMORY_ACCESS, UNIT_SH4AL_U_USE_FR, UNIT_SH4AL_U_SET_FR_0, UNIT_SH4AL_U_SET_FR
 , UNIT_SH4AL_U_LOAD_FR, UNIT_SH4AL_U_MAYBE_FPU, UNIT_SH4AL_U_FPU, UNIT_SH4AL_U_TRAP
 , UNIT_SH4AL_U_WRITE_BACK, UNIT_SH4AL_U_USE_MULTIPLY_RESULT, UNIT_SH4AL_U_SHIFT, UNIT_SH4AL_U_TAS
 , UNIT_SH4AL_U_MULSW, UNIT_SH4AL_U_MULL, UNIT_SH4AL_U_DMUL, UNIT_SH4AL_U_MACL
 , UNIT_SH4AL_U_MACW, UNIT_SH4AL_U_MULTIPLY, UNIT_SH4AL_U_SET_MAC, UNIT_SH4AL_U_LOAD_MAC
 , UNIT_SH4AL_U_LOAD_VBR, UNIT_SH4AL_U_LOAD_GBR, UNIT_SH4AL_U_USE_GR, UNIT_SH4AL_U_LOAD_GR
 , UNIT_SH4AL_U_STC_VBR, UNIT_SH4AL_U_LDCL_VBR, UNIT_SH4AL_U_LDCL, UNIT_SH4AL_U_USE_TBIT
 , UNIT_SH4AL_U_LDC_GBR, UNIT_SH4AL_U_LDC_SR, UNIT_SH4AL_U_SET_SR_BIT, UNIT_SH4AL_U_USE_PR
 , UNIT_SH4AL_U_LOAD_PR, UNIT_SH4AL_U_STS_PR, UNIT_SH4AL_U_LDS_PR, UNIT_SH4AL_U_MEMORY_ACCESS
 , UNIT_SH4AL_U_LOGIC_B, UNIT_SH4AL_U_JSR, UNIT_SH4AL_U_JMP, UNIT_SH4AL_U_BRANCH
 , UNIT_SH4AL_U_SX, UNIT_SH4AL_U_EXEC, UNIT_SH5_U_FTRV, UNIT_SH5_U_FIPR
 , UNIT_SH5_U_OCB, UNIT_SH5_U_MULR_GR, UNIT_SH5_U_MULR, UNIT_SH5_U_USE_DR
 , UNIT_SH5_U_LOAD_DR, UNIT_SH5_U_SET_DR, UNIT_SH5_U_FCNV, UNIT_SH5_U_FCMP
 , UNIT_SH5_U_FSQRT, UNIT_SH5_U_FDIV, UNIT_SH5_U_FPU_LOAD_GR, UNIT_SH5_U_USE_FPSCR
 , UNIT_SH5_U_LDSL_FPSCR, UNIT_SH5_U_LDS_FPSCR, UNIT_SH5_U_USE_FPUL, UNIT_SH5_U_FLDS_FPUL
 , UNIT_SH5_U_LOAD_FPUL, UNIT_SH5_U_SET_FPUL, UNIT_SH5_U_FPU_MEMORY_ACCESS, UNIT_SH5_U_USE_FR
 , UNIT_SH5_U_SET_FR_0, UNIT_SH5_U_SET_FR, UNIT_SH5_U_LOAD_FR, UNIT_SH5_U_MAYBE_FPU
 , UNIT_SH5_U_FPU, UNIT_SH5_U_TRAP, UNIT_SH5_U_WRITE_BACK, UNIT_SH5_U_USE_MULTIPLY_RESULT
 , UNIT_SH5_U_SHIFT, UNIT_SH5_U_TAS, UNIT_SH5_U_MULSW, UNIT_SH5_U_MULL
 , UNIT_SH5_U_DMUL, UNIT_SH5_U_MACL, UNIT_SH5_U_MACW, UNIT_SH5_U_MULTIPLY
 , UNIT_SH5_U_SET_MAC, UNIT_SH5_U_LOAD_MAC, UNIT_SH5_U_LOAD_VBR, UNIT_SH5_U_LOAD_GBR
 , UNIT_SH5_U_USE_GR, UNIT_SH5_U_LOAD_GR, UNIT_SH5_U_STC_VBR, UNIT_SH5_U_LDCL_VBR
 , UNIT_SH5_U_LDCL, UNIT_SH5_U_USE_TBIT, UNIT_SH5_U_LDC_GBR, UNIT_SH5_U_LDC_SR
 , UNIT_SH5_U_SET_SR_BIT, UNIT_SH5_U_USE_PR, UNIT_SH5_U_LOAD_PR, UNIT_SH5_U_STS_PR
 , UNIT_SH5_U_LDS_PR, UNIT_SH5_U_MEMORY_ACCESS, UNIT_SH5_U_LOGIC_B, UNIT_SH5_U_JSR
 , UNIT_SH5_U_JMP, UNIT_SH5_U_BRANCH, UNIT_SH5_U_SX, UNIT_SH5_U_EXEC
 , UNIT_SH5_MEDIA_U_PUTCFG, UNIT_SH5_MEDIA_U_GETCFG, UNIT_SH5_MEDIA_U_PT, UNIT_SH5_MEDIA_U_FTRVS
 , UNIT_SH5_MEDIA_U_FSQRTD, UNIT_SH5_MEDIA_U_FDIVD, UNIT_SH5_MEDIA_U_COND_BRANCH, UNIT_SH5_MEDIA_U_BLINK
 , UNIT_SH5_MEDIA_U_USE_TR, UNIT_SH5_MEDIA_U_USE_MTRX, UNIT_SH5_MEDIA_U_USE_FV, UNIT_SH5_MEDIA_U_USE_FP
 , UNIT_SH5_MEDIA_U_LOAD_MTRX, UNIT_SH5_MEDIA_U_LOAD_FV, UNIT_SH5_MEDIA_U_LOAD_FP, UNIT_SH5_MEDIA_U_SET_MTRX
 , UNIT_SH5_MEDIA_U_SET_FV, UNIT_SH5_MEDIA_U_SET_FP, UNIT_SH5_MEDIA_U_SET_GR, UNIT_SH5_MEDIA_U_FTRV
 , UNIT_SH5_MEDIA_U_FIPR, UNIT_SH5_MEDIA_U_OCB, UNIT_SH5_MEDIA_U_MULR_GR, UNIT_SH5_MEDIA_U_MULR
 , UNIT_SH5_MEDIA_U_USE_DR, UNIT_SH5_MEDIA_U_LOAD_DR, UNIT_SH5_MEDIA_U_SET_DR, UNIT_SH5_MEDIA_U_FCNV
 , UNIT_SH5_MEDIA_U_FCMP, UNIT_SH5_MEDIA_U_FSQRT, UNIT_SH5_MEDIA_U_FDIV, UNIT_SH5_MEDIA_U_FPU_LOAD_GR
 , UNIT_SH5_MEDIA_U_USE_FPSCR, UNIT_SH5_MEDIA_U_LDSL_FPSCR, UNIT_SH5_MEDIA_U_LDS_FPSCR, UNIT_SH5_MEDIA_U_USE_FPUL
 , UNIT_SH5_MEDIA_U_FLDS_FPUL, UNIT_SH5_MEDIA_U_LOAD_FPUL, UNIT_SH5_MEDIA_U_SET_FPUL, UNIT_SH5_MEDIA_U_FPU_MEMORY_ACCESS
 , UNIT_SH5_MEDIA_U_USE_FR, UNIT_SH5_MEDIA_U_SET_FR_0, UNIT_SH5_MEDIA_U_SET_FR, UNIT_SH5_MEDIA_U_LOAD_FR
 , UNIT_SH5_MEDIA_U_MAYBE_FPU, UNIT_SH5_MEDIA_U_FPU, UNIT_SH5_MEDIA_U_TRAP, UNIT_SH5_MEDIA_U_WRITE_BACK
 , UNIT_SH5_MEDIA_U_USE_MULTIPLY_RESULT, UNIT_SH5_MEDIA_U_SHIFT, UNIT_SH5_MEDIA_U_TAS, UNIT_SH5_MEDIA_U_MULSW
 , UNIT_SH5_MEDIA_U_MULL, UNIT_SH5_MEDIA_U_DMUL, UNIT_SH5_MEDIA_U_MACL, UNIT_SH5_MEDIA_U_MACW
 , UNIT_SH5_MEDIA_U_MULTIPLY, UNIT_SH5_MEDIA_U_SET_MAC, UNIT_SH5_MEDIA_U_LOAD_MAC, UNIT_SH5_MEDIA_U_LOAD_VBR
 , UNIT_SH5_MEDIA_U_LOAD_GBR, UNIT_SH5_MEDIA_U_USE_GR, UNIT_SH5_MEDIA_U_LOAD_GR, UNIT_SH5_MEDIA_U_STC_VBR
 , UNIT_SH5_MEDIA_U_LDCL_VBR, UNIT_SH5_MEDIA_U_LDCL, UNIT_SH5_MEDIA_U_USE_TBIT, UNIT_SH5_MEDIA_U_LDC_GBR
 , UNIT_SH5_MEDIA_U_LDC_SR, UNIT_SH5_MEDIA_U_SET_SR_BIT, UNIT_SH5_MEDIA_U_USE_PR, UNIT_SH5_MEDIA_U_LOAD_PR
 , UNIT_SH5_MEDIA_U_STS_PR, UNIT_SH5_MEDIA_U_LDS_PR, UNIT_SH5_MEDIA_U_MEMORY_ACCESS, UNIT_SH5_MEDIA_U_LOGIC_B
 , UNIT_SH5_MEDIA_U_JSR, UNIT_SH5_MEDIA_U_JMP, UNIT_SH5_MEDIA_U_BRANCH, UNIT_SH5_MEDIA_U_SX
 , UNIT_SH5_MEDIA_U_EXEC, UNIT_SH2_U_TRAP, UNIT_SH2_U_WRITE_BACK, UNIT_SH2_U_USE_MULTIPLY_RESULT
 , UNIT_SH2_U_SHIFT, UNIT_SH2_U_TAS, UNIT_SH2_U_MULSW, UNIT_SH2_U_MULL
 , UNIT_SH2_U_DMUL, UNIT_SH2_U_MACL, UNIT_SH2_U_MACW, UNIT_SH2_U_MULTIPLY
 , UNIT_SH2_U_SET_MAC, UNIT_SH2_U_LOAD_MAC, UNIT_SH2_U_LOAD_VBR, UNIT_SH2_U_LOAD_GBR
 , UNIT_SH2_U_USE_GR, UNIT_SH2_U_LOAD_GR, UNIT_SH2_U_STC_VBR, UNIT_SH2_U_LDCL_VBR
 , UNIT_SH2_U_LDCL, UNIT_SH2_U_USE_TBIT, UNIT_SH2_U_LDC_GBR, UNIT_SH2_U_LDC_SR
 , UNIT_SH2_U_SET_SR_BIT, UNIT_SH2_U_USE_PR, UNIT_SH2_U_LOAD_PR, UNIT_SH2_U_STS_PR
 , UNIT_SH2_U_LDS_PR, UNIT_SH2_U_MEMORY_ACCESS, UNIT_SH2_U_LOGIC_B, UNIT_SH2_U_JSR
 , UNIT_SH2_U_JMP, UNIT_SH2_U_BRANCH, UNIT_SH2_U_SX, UNIT_SH2_U_EXEC
 , UNIT_SH2E_U_FCNV, UNIT_SH2E_U_FCMP, UNIT_SH2E_U_FSQRT, UNIT_SH2E_U_FDIV
 , UNIT_SH2E_U_FPU_LOAD_GR, UNIT_SH2E_U_USE_FPSCR, UNIT_SH2E_U_LDSL_FPSCR, UNIT_SH2E_U_LDS_FPSCR
 , UNIT_SH2E_U_USE_FPUL, UNIT_SH2E_U_FLDS_FPUL, UNIT_SH2E_U_LOAD_FPUL, UNIT_SH2E_U_SET_FPUL
 , UNIT_SH2E_U_FPU_MEMORY_ACCESS, UNIT_SH2E_U_USE_FR, UNIT_SH2E_U_SET_FR_0, UNIT_SH2E_U_SET_FR
 , UNIT_SH2E_U_LOAD_FR, UNIT_SH2E_U_MAYBE_FPU, UNIT_SH2E_U_FPU, UNIT_SH2E_U_TRAP
 , UNIT_SH2E_U_WRITE_BACK, UNIT_SH2E_U_USE_MULTIPLY_RESULT, UNIT_SH2E_U_SHIFT, UNIT_SH2E_U_TAS
 , UNIT_SH2E_U_MULSW, UNIT_SH2E_U_MULL, UNIT_SH2E_U_DMUL, UNIT_SH2E_U_MACL
 , UNIT_SH2E_U_MACW, UNIT_SH2E_U_MULTIPLY, UNIT_SH2E_U_SET_MAC, UNIT_SH2E_U_LOAD_MAC
 , UNIT_SH2E_U_LOAD_VBR, UNIT_SH2E_U_LOAD_GBR, UNIT_SH2E_U_USE_GR, UNIT_SH2E_U_LOAD_GR
 , UNIT_SH2E_U_STC_VBR, UNIT_SH2E_U_LDCL_VBR, UNIT_SH2E_U_LDCL, UNIT_SH2E_U_USE_TBIT
 , UNIT_SH2E_U_LDC_GBR, UNIT_SH2E_U_LDC_SR, UNIT_SH2E_U_SET_SR_BIT, UNIT_SH2E_U_USE_PR
 , UNIT_SH2E_U_LOAD_PR, UNIT_SH2E_U_STS_PR, UNIT_SH2E_U_LDS_PR, UNIT_SH2E_U_MEMORY_ACCESS
 , UNIT_SH2E_U_LOGIC_B, UNIT_SH2E_U_JSR, UNIT_SH2E_U_JMP, UNIT_SH2E_U_BRANCH
 , UNIT_SH2E_U_SX, UNIT_SH2E_U_EXEC, UNIT_SH3_U_TRAP, UNIT_SH3_U_WRITE_BACK
 , UNIT_SH3_U_USE_MULTIPLY_RESULT, UNIT_SH3_U_SHIFT, UNIT_SH3_U_TAS, UNIT_SH3_U_MULSW
 , UNIT_SH3_U_MULL, UNIT_SH3_U_DMUL, UNIT_SH3_U_MACL, UNIT_SH3_U_MACW
 , UNIT_SH3_U_MULTIPLY, UNIT_SH3_U_SET_MAC, UNIT_SH3_U_LOAD_MAC, UNIT_SH3_U_LOAD_VBR
 , UNIT_SH3_U_LOAD_GBR, UNIT_SH3_U_USE_GR, UNIT_SH3_U_LOAD_GR, UNIT_SH3_U_STC_VBR
 , UNIT_SH3_U_LDCL_VBR, UNIT_SH3_U_LDCL, UNIT_SH3_U_USE_TBIT, UNIT_SH3_U_LDC_GBR
 , UNIT_SH3_U_LDC_SR, UNIT_SH3_U_SET_SR_BIT, UNIT_SH3_U_USE_PR, UNIT_SH3_U_LOAD_PR
 , UNIT_SH3_U_STS_PR, UNIT_SH3_U_LDS_PR, UNIT_SH3_U_MEMORY_ACCESS, UNIT_SH3_U_LOGIC_B
 , UNIT_SH3_U_JSR, UNIT_SH3_U_JMP, UNIT_SH3_U_BRANCH, UNIT_SH3_U_SX
 , UNIT_SH3_U_EXEC, UNIT_SH3E_U_FCNV, UNIT_SH3E_U_FCMP, UNIT_SH3E_U_FSQRT
 , UNIT_SH3E_U_FDIV, UNIT_SH3E_U_FPU_LOAD_GR, UNIT_SH3E_U_USE_FPSCR, UNIT_SH3E_U_LDSL_FPSCR
 , UNIT_SH3E_U_LDS_FPSCR, UNIT_SH3E_U_USE_FPUL, UNIT_SH3E_U_FLDS_FPUL, UNIT_SH3E_U_LOAD_FPUL
 , UNIT_SH3E_U_SET_FPUL, UNIT_SH3E_U_FPU_MEMORY_ACCESS, UNIT_SH3E_U_USE_FR, UNIT_SH3E_U_SET_FR_0
 , UNIT_SH3E_U_SET_FR, UNIT_SH3E_U_LOAD_FR, UNIT_SH3E_U_MAYBE_FPU, UNIT_SH3E_U_FPU
 , UNIT_SH3E_U_TRAP, UNIT_SH3E_U_WRITE_BACK, UNIT_SH3E_U_USE_MULTIPLY_RESULT, UNIT_SH3E_U_SHIFT
 , UNIT_SH3E_U_TAS, UNIT_SH3E_U_MULSW, UNIT_SH3E_U_MULL, UNIT_SH3E_U_DMUL
 , UNIT_SH3E_U_MACL, UNIT_SH3E_U_MACW, UNIT_SH3E_U_MULTIPLY, UNIT_SH3E_U_SET_MAC
 , UNIT_SH3E_U_LOAD_MAC, UNIT_SH3E_U_LOAD_VBR, UNIT_SH3E_U_LOAD_GBR, UNIT_SH3E_U_USE_GR
 , UNIT_SH3E_U_LOAD_GR, UNIT_SH3E_U_STC_VBR, UNIT_SH3E_U_LDCL_VBR, UNIT_SH3E_U_LDCL
 , UNIT_SH3E_U_USE_TBIT, UNIT_SH3E_U_LDC_GBR, UNIT_SH3E_U_LDC_SR, UNIT_SH3E_U_SET_SR_BIT
 , UNIT_SH3E_U_USE_PR, UNIT_SH3E_U_LOAD_PR, UNIT_SH3E_U_STS_PR, UNIT_SH3E_U_LDS_PR
 , UNIT_SH3E_U_MEMORY_ACCESS, UNIT_SH3E_U_LOGIC_B, UNIT_SH3E_U_JSR, UNIT_SH3E_U_JMP
 , UNIT_SH3E_U_BRANCH, UNIT_SH3E_U_SX, UNIT_SH3E_U_EXEC, UNIT_MAX
} UNIT_TYPE;

#define MAX_UNITS (9)

#endif /* SH_ARCH_H */
