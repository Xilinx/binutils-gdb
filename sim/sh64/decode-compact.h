/* Decode header for sh64_compact.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1996, 1997, 1998, 1999, 2000 Free Software Foundation, Inc.

This file is part of the GNU Simulators.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/

#ifndef SH64_COMPACT_DECODE_H
#define SH64_COMPACT_DECODE_H

extern const IDESC *sh64_compact_decode (SIM_CPU *, IADDR,
                                  CGEN_INSN_INT, CGEN_INSN_INT,
                                  ARGBUF *);
extern void sh64_compact_init_idesc_table (SIM_CPU *);
extern void sh64_compact_sem_init_idesc_table (SIM_CPU *);
extern void sh64_compact_semf_init_idesc_table (SIM_CPU *);

/* Enum declaration for instructions in cpu family sh64.  */
typedef enum sh64_compact_insn_type {
  SH64_COMPACT_INSN_X_INVALID, SH64_COMPACT_INSN_X_AFTER, SH64_COMPACT_INSN_X_BEFORE, SH64_COMPACT_INSN_X_CTI_CHAIN
 , SH64_COMPACT_INSN_X_CHAIN, SH64_COMPACT_INSN_X_BEGIN, SH64_COMPACT_INSN_ADD_COMPACT, SH64_COMPACT_INSN_ADDI_COMPACT
 , SH64_COMPACT_INSN_ADDC_COMPACT, SH64_COMPACT_INSN_ADDV_COMPACT, SH64_COMPACT_INSN_AND_COMPACT, SH64_COMPACT_INSN_ANDI_COMPACT
 , SH64_COMPACT_INSN_ANDB_COMPACT, SH64_COMPACT_INSN_BF_COMPACT, SH64_COMPACT_INSN_BFS_COMPACT, SH64_COMPACT_INSN_BRA_COMPACT
 , SH64_COMPACT_INSN_BRAF_COMPACT, SH64_COMPACT_INSN_BRK_COMPACT, SH64_COMPACT_INSN_BSR_COMPACT, SH64_COMPACT_INSN_BSRF_COMPACT
 , SH64_COMPACT_INSN_BT_COMPACT, SH64_COMPACT_INSN_BTS_COMPACT, SH64_COMPACT_INSN_CLRMAC_COMPACT, SH64_COMPACT_INSN_CLRS_COMPACT
 , SH64_COMPACT_INSN_CLRT_COMPACT, SH64_COMPACT_INSN_CMPEQ_COMPACT, SH64_COMPACT_INSN_CMPEQI_COMPACT, SH64_COMPACT_INSN_CMPGE_COMPACT
 , SH64_COMPACT_INSN_CMPGT_COMPACT, SH64_COMPACT_INSN_CMPHI_COMPACT, SH64_COMPACT_INSN_CMPHS_COMPACT, SH64_COMPACT_INSN_CMPPL_COMPACT
 , SH64_COMPACT_INSN_CMPPZ_COMPACT, SH64_COMPACT_INSN_CMPSTR_COMPACT, SH64_COMPACT_INSN_DIV0S_COMPACT, SH64_COMPACT_INSN_DIV0U_COMPACT
 , SH64_COMPACT_INSN_DIV1_COMPACT, SH64_COMPACT_INSN_DMULSL_COMPACT, SH64_COMPACT_INSN_DMULUL_COMPACT, SH64_COMPACT_INSN_DT_COMPACT
 , SH64_COMPACT_INSN_EXTSB_COMPACT, SH64_COMPACT_INSN_EXTSW_COMPACT, SH64_COMPACT_INSN_EXTUB_COMPACT, SH64_COMPACT_INSN_EXTUW_COMPACT
 , SH64_COMPACT_INSN_FABS_COMPACT, SH64_COMPACT_INSN_FADD_COMPACT, SH64_COMPACT_INSN_FCMPEQ_COMPACT, SH64_COMPACT_INSN_FCMPGT_COMPACT
 , SH64_COMPACT_INSN_FCNVDS_COMPACT, SH64_COMPACT_INSN_FCNVSD_COMPACT, SH64_COMPACT_INSN_FDIV_COMPACT, SH64_COMPACT_INSN_FIPR_COMPACT
 , SH64_COMPACT_INSN_FLDS_COMPACT, SH64_COMPACT_INSN_FLDI0_COMPACT, SH64_COMPACT_INSN_FLDI1_COMPACT, SH64_COMPACT_INSN_FLOAT_COMPACT
 , SH64_COMPACT_INSN_FMAC_COMPACT, SH64_COMPACT_INSN_FMOV1_COMPACT, SH64_COMPACT_INSN_FMOV2_COMPACT, SH64_COMPACT_INSN_FMOV3_COMPACT
 , SH64_COMPACT_INSN_FMOV4_COMPACT, SH64_COMPACT_INSN_FMOV5_COMPACT, SH64_COMPACT_INSN_FMOV6_COMPACT, SH64_COMPACT_INSN_FMOV7_COMPACT
 , SH64_COMPACT_INSN_FMUL_COMPACT, SH64_COMPACT_INSN_FNEG_COMPACT, SH64_COMPACT_INSN_FRCHG_COMPACT, SH64_COMPACT_INSN_FSCHG_COMPACT
 , SH64_COMPACT_INSN_FSQRT_COMPACT, SH64_COMPACT_INSN_FSTS_COMPACT, SH64_COMPACT_INSN_FSUB_COMPACT, SH64_COMPACT_INSN_FTRC_COMPACT
 , SH64_COMPACT_INSN_FTRV_COMPACT, SH64_COMPACT_INSN_JMP_COMPACT, SH64_COMPACT_INSN_JSR_COMPACT, SH64_COMPACT_INSN_LDC_COMPACT
 , SH64_COMPACT_INSN_LDCL_COMPACT, SH64_COMPACT_INSN_LDS_FPSCR_COMPACT, SH64_COMPACT_INSN_LDSL_FPSCR_COMPACT, SH64_COMPACT_INSN_LDS_FPUL_COMPACT
 , SH64_COMPACT_INSN_LDSL_FPUL_COMPACT, SH64_COMPACT_INSN_LDS_MACH_COMPACT, SH64_COMPACT_INSN_LDSL_MACH_COMPACT, SH64_COMPACT_INSN_LDS_MACL_COMPACT
 , SH64_COMPACT_INSN_LDSL_MACL_COMPACT, SH64_COMPACT_INSN_LDS_PR_COMPACT, SH64_COMPACT_INSN_LDSL_PR_COMPACT, SH64_COMPACT_INSN_MACL_COMPACT
 , SH64_COMPACT_INSN_MACW_COMPACT, SH64_COMPACT_INSN_MOV_COMPACT, SH64_COMPACT_INSN_MOVI_COMPACT, SH64_COMPACT_INSN_MOVB1_COMPACT
 , SH64_COMPACT_INSN_MOVB2_COMPACT, SH64_COMPACT_INSN_MOVB3_COMPACT, SH64_COMPACT_INSN_MOVB4_COMPACT, SH64_COMPACT_INSN_MOVB5_COMPACT
 , SH64_COMPACT_INSN_MOVB6_COMPACT, SH64_COMPACT_INSN_MOVB7_COMPACT, SH64_COMPACT_INSN_MOVB8_COMPACT, SH64_COMPACT_INSN_MOVB9_COMPACT
 , SH64_COMPACT_INSN_MOVB10_COMPACT, SH64_COMPACT_INSN_MOVL1_COMPACT, SH64_COMPACT_INSN_MOVL2_COMPACT, SH64_COMPACT_INSN_MOVL3_COMPACT
 , SH64_COMPACT_INSN_MOVL4_COMPACT, SH64_COMPACT_INSN_MOVL5_COMPACT, SH64_COMPACT_INSN_MOVL6_COMPACT, SH64_COMPACT_INSN_MOVL7_COMPACT
 , SH64_COMPACT_INSN_MOVL8_COMPACT, SH64_COMPACT_INSN_MOVL9_COMPACT, SH64_COMPACT_INSN_MOVL10_COMPACT, SH64_COMPACT_INSN_MOVL11_COMPACT
 , SH64_COMPACT_INSN_MOVW1_COMPACT, SH64_COMPACT_INSN_MOVW2_COMPACT, SH64_COMPACT_INSN_MOVW3_COMPACT, SH64_COMPACT_INSN_MOVW4_COMPACT
 , SH64_COMPACT_INSN_MOVW5_COMPACT, SH64_COMPACT_INSN_MOVW6_COMPACT, SH64_COMPACT_INSN_MOVW7_COMPACT, SH64_COMPACT_INSN_MOVW8_COMPACT
 , SH64_COMPACT_INSN_MOVW9_COMPACT, SH64_COMPACT_INSN_MOVW10_COMPACT, SH64_COMPACT_INSN_MOVW11_COMPACT, SH64_COMPACT_INSN_MOVA_COMPACT
 , SH64_COMPACT_INSN_MOVCAL_COMPACT, SH64_COMPACT_INSN_MOVT_COMPACT, SH64_COMPACT_INSN_MULL_COMPACT, SH64_COMPACT_INSN_MULSW_COMPACT
 , SH64_COMPACT_INSN_MULUW_COMPACT, SH64_COMPACT_INSN_NEG_COMPACT, SH64_COMPACT_INSN_NEGC_COMPACT, SH64_COMPACT_INSN_NOP_COMPACT
 , SH64_COMPACT_INSN_NOT_COMPACT, SH64_COMPACT_INSN_OCBI_COMPACT, SH64_COMPACT_INSN_OCBP_COMPACT, SH64_COMPACT_INSN_OCBWB_COMPACT
 , SH64_COMPACT_INSN_OR_COMPACT, SH64_COMPACT_INSN_ORI_COMPACT, SH64_COMPACT_INSN_ORB_COMPACT, SH64_COMPACT_INSN_PREF_COMPACT
 , SH64_COMPACT_INSN_ROTCL_COMPACT, SH64_COMPACT_INSN_ROTCR_COMPACT, SH64_COMPACT_INSN_ROTL_COMPACT, SH64_COMPACT_INSN_ROTR_COMPACT
 , SH64_COMPACT_INSN_RTS_COMPACT, SH64_COMPACT_INSN_SETS_COMPACT, SH64_COMPACT_INSN_SETT_COMPACT, SH64_COMPACT_INSN_SHAD_COMPACT
 , SH64_COMPACT_INSN_SHAL_COMPACT, SH64_COMPACT_INSN_SHAR_COMPACT, SH64_COMPACT_INSN_SHLD_COMPACT, SH64_COMPACT_INSN_SHLL_COMPACT
 , SH64_COMPACT_INSN_SHLL2_COMPACT, SH64_COMPACT_INSN_SHLL8_COMPACT, SH64_COMPACT_INSN_SHLL16_COMPACT, SH64_COMPACT_INSN_SHLR_COMPACT
 , SH64_COMPACT_INSN_SHLR2_COMPACT, SH64_COMPACT_INSN_SHLR8_COMPACT, SH64_COMPACT_INSN_SHLR16_COMPACT, SH64_COMPACT_INSN_STC_GBR_COMPACT
 , SH64_COMPACT_INSN_STCL_GBR_COMPACT, SH64_COMPACT_INSN_STS_FPSCR_COMPACT, SH64_COMPACT_INSN_STSL_FPSCR_COMPACT, SH64_COMPACT_INSN_STS_FPUL_COMPACT
 , SH64_COMPACT_INSN_STSL_FPUL_COMPACT, SH64_COMPACT_INSN_STS_MACH_COMPACT, SH64_COMPACT_INSN_STSL_MACH_COMPACT, SH64_COMPACT_INSN_STS_MACL_COMPACT
 , SH64_COMPACT_INSN_STSL_MACL_COMPACT, SH64_COMPACT_INSN_STS_PR_COMPACT, SH64_COMPACT_INSN_STSL_PR_COMPACT, SH64_COMPACT_INSN_SUB_COMPACT
 , SH64_COMPACT_INSN_SUBC_COMPACT, SH64_COMPACT_INSN_SUBV_COMPACT, SH64_COMPACT_INSN_SWAPB_COMPACT, SH64_COMPACT_INSN_SWAPW_COMPACT
 , SH64_COMPACT_INSN_TASB_COMPACT, SH64_COMPACT_INSN_TRAPA_COMPACT, SH64_COMPACT_INSN_TST_COMPACT, SH64_COMPACT_INSN_TSTI_COMPACT
 , SH64_COMPACT_INSN_TSTB_COMPACT, SH64_COMPACT_INSN_XOR_COMPACT, SH64_COMPACT_INSN_XORI_COMPACT, SH64_COMPACT_INSN_XORB_COMPACT
 , SH64_COMPACT_INSN_XTRCT_COMPACT, SH64_COMPACT_INSN_MAX
} SH64_COMPACT_INSN_TYPE;

/* Enum declaration for semantic formats in cpu family sh64.  */
typedef enum sh64_compact_sfmt_type {
  SH64_COMPACT_SFMT_EMPTY, SH64_COMPACT_SFMT_ADD_COMPACT, SH64_COMPACT_SFMT_ADDI_COMPACT, SH64_COMPACT_SFMT_ADDC_COMPACT
 , SH64_COMPACT_SFMT_ADDV_COMPACT, SH64_COMPACT_SFMT_AND_COMPACT, SH64_COMPACT_SFMT_ANDI_COMPACT, SH64_COMPACT_SFMT_ANDB_COMPACT
 , SH64_COMPACT_SFMT_BF_COMPACT, SH64_COMPACT_SFMT_BRA_COMPACT, SH64_COMPACT_SFMT_BRAF_COMPACT, SH64_COMPACT_SFMT_BRK_COMPACT
 , SH64_COMPACT_SFMT_BSR_COMPACT, SH64_COMPACT_SFMT_BSRF_COMPACT, SH64_COMPACT_SFMT_CLRMAC_COMPACT, SH64_COMPACT_SFMT_CLRS_COMPACT
 , SH64_COMPACT_SFMT_CLRT_COMPACT, SH64_COMPACT_SFMT_CMPEQ_COMPACT, SH64_COMPACT_SFMT_CMPEQI_COMPACT, SH64_COMPACT_SFMT_CMPPL_COMPACT
 , SH64_COMPACT_SFMT_DIV0S_COMPACT, SH64_COMPACT_SFMT_DIV0U_COMPACT, SH64_COMPACT_SFMT_DIV1_COMPACT, SH64_COMPACT_SFMT_DMULSL_COMPACT
 , SH64_COMPACT_SFMT_DT_COMPACT, SH64_COMPACT_SFMT_EXTSB_COMPACT, SH64_COMPACT_SFMT_FABS_COMPACT, SH64_COMPACT_SFMT_FADD_COMPACT
 , SH64_COMPACT_SFMT_FCMPEQ_COMPACT, SH64_COMPACT_SFMT_FCNVDS_COMPACT, SH64_COMPACT_SFMT_FCNVSD_COMPACT, SH64_COMPACT_SFMT_FIPR_COMPACT
 , SH64_COMPACT_SFMT_FLDS_COMPACT, SH64_COMPACT_SFMT_FLDI0_COMPACT, SH64_COMPACT_SFMT_FLOAT_COMPACT, SH64_COMPACT_SFMT_FMAC_COMPACT
 , SH64_COMPACT_SFMT_FMOV1_COMPACT, SH64_COMPACT_SFMT_FMOV2_COMPACT, SH64_COMPACT_SFMT_FMOV3_COMPACT, SH64_COMPACT_SFMT_FMOV4_COMPACT
 , SH64_COMPACT_SFMT_FMOV5_COMPACT, SH64_COMPACT_SFMT_FMOV6_COMPACT, SH64_COMPACT_SFMT_FMOV7_COMPACT, SH64_COMPACT_SFMT_FRCHG_COMPACT
 , SH64_COMPACT_SFMT_FSCHG_COMPACT, SH64_COMPACT_SFMT_FSTS_COMPACT, SH64_COMPACT_SFMT_FTRC_COMPACT, SH64_COMPACT_SFMT_FTRV_COMPACT
 , SH64_COMPACT_SFMT_JMP_COMPACT, SH64_COMPACT_SFMT_LDC_COMPACT, SH64_COMPACT_SFMT_LDCL_COMPACT, SH64_COMPACT_SFMT_LDS_FPSCR_COMPACT
 , SH64_COMPACT_SFMT_LDSL_FPSCR_COMPACT, SH64_COMPACT_SFMT_LDS_FPUL_COMPACT, SH64_COMPACT_SFMT_LDSL_FPUL_COMPACT, SH64_COMPACT_SFMT_LDS_MACH_COMPACT
 , SH64_COMPACT_SFMT_LDSL_MACH_COMPACT, SH64_COMPACT_SFMT_LDS_MACL_COMPACT, SH64_COMPACT_SFMT_LDSL_MACL_COMPACT, SH64_COMPACT_SFMT_LDS_PR_COMPACT
 , SH64_COMPACT_SFMT_LDSL_PR_COMPACT, SH64_COMPACT_SFMT_MACL_COMPACT, SH64_COMPACT_SFMT_MACW_COMPACT, SH64_COMPACT_SFMT_MOV_COMPACT
 , SH64_COMPACT_SFMT_MOVI_COMPACT, SH64_COMPACT_SFMT_MOVB1_COMPACT, SH64_COMPACT_SFMT_MOVB2_COMPACT, SH64_COMPACT_SFMT_MOVB3_COMPACT
 , SH64_COMPACT_SFMT_MOVB4_COMPACT, SH64_COMPACT_SFMT_MOVB5_COMPACT, SH64_COMPACT_SFMT_MOVB6_COMPACT, SH64_COMPACT_SFMT_MOVB7_COMPACT
 , SH64_COMPACT_SFMT_MOVB8_COMPACT, SH64_COMPACT_SFMT_MOVB9_COMPACT, SH64_COMPACT_SFMT_MOVB10_COMPACT, SH64_COMPACT_SFMT_MOVL4_COMPACT
 , SH64_COMPACT_SFMT_MOVL5_COMPACT, SH64_COMPACT_SFMT_MOVL7_COMPACT, SH64_COMPACT_SFMT_MOVL9_COMPACT, SH64_COMPACT_SFMT_MOVL10_COMPACT
 , SH64_COMPACT_SFMT_MOVL11_COMPACT, SH64_COMPACT_SFMT_MOVW4_COMPACT, SH64_COMPACT_SFMT_MOVW5_COMPACT, SH64_COMPACT_SFMT_MOVW9_COMPACT
 , SH64_COMPACT_SFMT_MOVW10_COMPACT, SH64_COMPACT_SFMT_MOVW11_COMPACT, SH64_COMPACT_SFMT_MOVA_COMPACT, SH64_COMPACT_SFMT_MOVCAL_COMPACT
 , SH64_COMPACT_SFMT_MOVT_COMPACT, SH64_COMPACT_SFMT_MULL_COMPACT, SH64_COMPACT_SFMT_NEGC_COMPACT, SH64_COMPACT_SFMT_NOP_COMPACT
 , SH64_COMPACT_SFMT_ROTCL_COMPACT, SH64_COMPACT_SFMT_RTS_COMPACT, SH64_COMPACT_SFMT_SHAD_COMPACT, SH64_COMPACT_SFMT_SHLL2_COMPACT
 , SH64_COMPACT_SFMT_STC_GBR_COMPACT, SH64_COMPACT_SFMT_STCL_GBR_COMPACT, SH64_COMPACT_SFMT_STS_FPSCR_COMPACT, SH64_COMPACT_SFMT_STSL_FPSCR_COMPACT
 , SH64_COMPACT_SFMT_STS_FPUL_COMPACT, SH64_COMPACT_SFMT_STSL_FPUL_COMPACT, SH64_COMPACT_SFMT_STS_MACH_COMPACT, SH64_COMPACT_SFMT_STSL_MACH_COMPACT
 , SH64_COMPACT_SFMT_STS_MACL_COMPACT, SH64_COMPACT_SFMT_STSL_MACL_COMPACT, SH64_COMPACT_SFMT_STS_PR_COMPACT, SH64_COMPACT_SFMT_STSL_PR_COMPACT
 , SH64_COMPACT_SFMT_TASB_COMPACT, SH64_COMPACT_SFMT_TRAPA_COMPACT, SH64_COMPACT_SFMT_TSTI_COMPACT, SH64_COMPACT_SFMT_TSTB_COMPACT
 , SH64_COMPACT_SFMT_XORI_COMPACT
} SH64_COMPACT_SFMT_TYPE;

/* Function unit handlers (user written).  */

extern int sh64_model_sh5_u_exec (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/);

/* Profiling before/after handlers (user written) */

extern void sh64_model_insn_before (SIM_CPU *, int /*first_p*/);
extern void sh64_model_insn_after (SIM_CPU *, int /*last_p*/, int /*cycles*/);

#endif /* SH64_COMPACT_DECODE_H */
