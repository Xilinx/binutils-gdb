/* Decode header for fr30bf.

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

#ifndef FR30BF_DECODE_H
#define FR30BF_DECODE_H

extern const IDESC *fr30bf_decode (SIM_CPU *, IADDR,
                                  CGEN_INSN_INT,
                                  ARGBUF *);
extern void fr30bf_init_idesc_table (SIM_CPU *);
extern void fr30bf_sem_init_idesc_table (SIM_CPU *);
extern void fr30bf_semf_init_idesc_table (SIM_CPU *);

/* Enum declaration for instructions in cpu family fr30bf.  */
typedef enum fr30bf_insn_type {
  FR30BF_INSN_X_INVALID, FR30BF_INSN_X_AFTER, FR30BF_INSN_X_BEFORE, FR30BF_INSN_X_CTI_CHAIN
 , FR30BF_INSN_X_CHAIN, FR30BF_INSN_X_BEGIN, FR30BF_INSN_ADD, FR30BF_INSN_ADDI
 , FR30BF_INSN_ADD2, FR30BF_INSN_ADDC, FR30BF_INSN_ADDN, FR30BF_INSN_ADDNI
 , FR30BF_INSN_ADDN2, FR30BF_INSN_SUB, FR30BF_INSN_SUBC, FR30BF_INSN_SUBN
 , FR30BF_INSN_CMP, FR30BF_INSN_CMPI, FR30BF_INSN_CMP2, FR30BF_INSN_AND
 , FR30BF_INSN_OR, FR30BF_INSN_EOR, FR30BF_INSN_ANDM, FR30BF_INSN_ANDH
 , FR30BF_INSN_ANDB, FR30BF_INSN_ORM, FR30BF_INSN_ORH, FR30BF_INSN_ORB
 , FR30BF_INSN_EORM, FR30BF_INSN_EORH, FR30BF_INSN_EORB, FR30BF_INSN_BANDL
 , FR30BF_INSN_BORL, FR30BF_INSN_BEORL, FR30BF_INSN_BANDH, FR30BF_INSN_BORH
 , FR30BF_INSN_BEORH, FR30BF_INSN_BTSTL, FR30BF_INSN_BTSTH, FR30BF_INSN_MUL
 , FR30BF_INSN_MULU, FR30BF_INSN_MULH, FR30BF_INSN_MULUH, FR30BF_INSN_DIV0S
 , FR30BF_INSN_DIV0U, FR30BF_INSN_DIV1, FR30BF_INSN_DIV2, FR30BF_INSN_DIV3
 , FR30BF_INSN_DIV4S, FR30BF_INSN_LSL, FR30BF_INSN_LSLI, FR30BF_INSN_LSL2
 , FR30BF_INSN_LSR, FR30BF_INSN_LSRI, FR30BF_INSN_LSR2, FR30BF_INSN_ASR
 , FR30BF_INSN_ASRI, FR30BF_INSN_ASR2, FR30BF_INSN_LDI8, FR30BF_INSN_LDI20
 , FR30BF_INSN_LDI32, FR30BF_INSN_LD, FR30BF_INSN_LDUH, FR30BF_INSN_LDUB
 , FR30BF_INSN_LDR13, FR30BF_INSN_LDR13UH, FR30BF_INSN_LDR13UB, FR30BF_INSN_LDR14
 , FR30BF_INSN_LDR14UH, FR30BF_INSN_LDR14UB, FR30BF_INSN_LDR15, FR30BF_INSN_LDR15GR
 , FR30BF_INSN_LDR15DR, FR30BF_INSN_LDR15PS, FR30BF_INSN_ST, FR30BF_INSN_STH
 , FR30BF_INSN_STB, FR30BF_INSN_STR13, FR30BF_INSN_STR13H, FR30BF_INSN_STR13B
 , FR30BF_INSN_STR14, FR30BF_INSN_STR14H, FR30BF_INSN_STR14B, FR30BF_INSN_STR15
 , FR30BF_INSN_STR15GR, FR30BF_INSN_STR15DR, FR30BF_INSN_STR15PS, FR30BF_INSN_MOV
 , FR30BF_INSN_MOVDR, FR30BF_INSN_MOVPS, FR30BF_INSN_MOV2DR, FR30BF_INSN_MOV2PS
 , FR30BF_INSN_JMP, FR30BF_INSN_JMPD, FR30BF_INSN_CALLR, FR30BF_INSN_CALLRD
 , FR30BF_INSN_CALL, FR30BF_INSN_CALLD, FR30BF_INSN_RET, FR30BF_INSN_RET_D
 , FR30BF_INSN_INT, FR30BF_INSN_INTE, FR30BF_INSN_RETI, FR30BF_INSN_BRAD
 , FR30BF_INSN_BRA, FR30BF_INSN_BNOD, FR30BF_INSN_BNO, FR30BF_INSN_BEQD
 , FR30BF_INSN_BEQ, FR30BF_INSN_BNED, FR30BF_INSN_BNE, FR30BF_INSN_BCD
 , FR30BF_INSN_BC, FR30BF_INSN_BNCD, FR30BF_INSN_BNC, FR30BF_INSN_BND
 , FR30BF_INSN_BN, FR30BF_INSN_BPD, FR30BF_INSN_BP, FR30BF_INSN_BVD
 , FR30BF_INSN_BV, FR30BF_INSN_BNVD, FR30BF_INSN_BNV, FR30BF_INSN_BLTD
 , FR30BF_INSN_BLT, FR30BF_INSN_BGED, FR30BF_INSN_BGE, FR30BF_INSN_BLED
 , FR30BF_INSN_BLE, FR30BF_INSN_BGTD, FR30BF_INSN_BGT, FR30BF_INSN_BLSD
 , FR30BF_INSN_BLS, FR30BF_INSN_BHID, FR30BF_INSN_BHI, FR30BF_INSN_DMOVR13
 , FR30BF_INSN_DMOVR13H, FR30BF_INSN_DMOVR13B, FR30BF_INSN_DMOVR13PI, FR30BF_INSN_DMOVR13PIH
 , FR30BF_INSN_DMOVR13PIB, FR30BF_INSN_DMOVR15PI, FR30BF_INSN_DMOV2R13, FR30BF_INSN_DMOV2R13H
 , FR30BF_INSN_DMOV2R13B, FR30BF_INSN_DMOV2R13PI, FR30BF_INSN_DMOV2R13PIH, FR30BF_INSN_DMOV2R13PIB
 , FR30BF_INSN_DMOV2R15PD, FR30BF_INSN_LDRES, FR30BF_INSN_STRES, FR30BF_INSN_COPOP
 , FR30BF_INSN_COPLD, FR30BF_INSN_COPST, FR30BF_INSN_COPSV, FR30BF_INSN_NOP
 , FR30BF_INSN_ANDCCR, FR30BF_INSN_ORCCR, FR30BF_INSN_STILM, FR30BF_INSN_ADDSP
 , FR30BF_INSN_EXTSB, FR30BF_INSN_EXTUB, FR30BF_INSN_EXTSH, FR30BF_INSN_EXTUH
 , FR30BF_INSN_LDM0, FR30BF_INSN_LDM1, FR30BF_INSN_STM0, FR30BF_INSN_STM1
 , FR30BF_INSN_ENTER, FR30BF_INSN_LEAVE, FR30BF_INSN_XCHB, FR30BF_INSN_MAX
} FR30BF_INSN_TYPE;

/* Enum declaration for semantic formats in cpu family fr30bf.  */
typedef enum fr30bf_sfmt_type {
  FR30BF_SFMT_EMPTY, FR30BF_SFMT_ADD, FR30BF_SFMT_ADDI, FR30BF_SFMT_ADD2
 , FR30BF_SFMT_ADDC, FR30BF_SFMT_ADDN, FR30BF_SFMT_ADDNI, FR30BF_SFMT_ADDN2
 , FR30BF_SFMT_CMP, FR30BF_SFMT_CMPI, FR30BF_SFMT_CMP2, FR30BF_SFMT_AND
 , FR30BF_SFMT_ANDM, FR30BF_SFMT_ANDH, FR30BF_SFMT_ANDB, FR30BF_SFMT_BANDL
 , FR30BF_SFMT_BTSTL, FR30BF_SFMT_MUL, FR30BF_SFMT_MULU, FR30BF_SFMT_MULH
 , FR30BF_SFMT_DIV0S, FR30BF_SFMT_DIV0U, FR30BF_SFMT_DIV1, FR30BF_SFMT_DIV2
 , FR30BF_SFMT_DIV3, FR30BF_SFMT_DIV4S, FR30BF_SFMT_LSL, FR30BF_SFMT_LSLI
 , FR30BF_SFMT_LDI8, FR30BF_SFMT_LDI20, FR30BF_SFMT_LDI32, FR30BF_SFMT_LD
 , FR30BF_SFMT_LDR13, FR30BF_SFMT_LDR14, FR30BF_SFMT_LDR14UH, FR30BF_SFMT_LDR14UB
 , FR30BF_SFMT_LDR15, FR30BF_SFMT_LDR15GR, FR30BF_SFMT_LDR15DR, FR30BF_SFMT_LDR15PS
 , FR30BF_SFMT_ST, FR30BF_SFMT_STR13, FR30BF_SFMT_STR14, FR30BF_SFMT_STR14H
 , FR30BF_SFMT_STR14B, FR30BF_SFMT_STR15, FR30BF_SFMT_STR15GR, FR30BF_SFMT_STR15DR
 , FR30BF_SFMT_STR15PS, FR30BF_SFMT_MOV, FR30BF_SFMT_MOVDR, FR30BF_SFMT_MOVPS
 , FR30BF_SFMT_MOV2DR, FR30BF_SFMT_MOV2PS, FR30BF_SFMT_JMP, FR30BF_SFMT_CALLR
 , FR30BF_SFMT_CALL, FR30BF_SFMT_RET, FR30BF_SFMT_INT, FR30BF_SFMT_INTE
 , FR30BF_SFMT_RETI, FR30BF_SFMT_BRAD, FR30BF_SFMT_BNOD, FR30BF_SFMT_BEQD
 , FR30BF_SFMT_BCD, FR30BF_SFMT_BND, FR30BF_SFMT_BVD, FR30BF_SFMT_BLTD
 , FR30BF_SFMT_BLED, FR30BF_SFMT_BLSD, FR30BF_SFMT_DMOVR13, FR30BF_SFMT_DMOVR13H
 , FR30BF_SFMT_DMOVR13B, FR30BF_SFMT_DMOVR13PI, FR30BF_SFMT_DMOVR13PIH, FR30BF_SFMT_DMOVR13PIB
 , FR30BF_SFMT_DMOVR15PI, FR30BF_SFMT_DMOV2R13, FR30BF_SFMT_DMOV2R13H, FR30BF_SFMT_DMOV2R13B
 , FR30BF_SFMT_DMOV2R13PI, FR30BF_SFMT_DMOV2R13PIH, FR30BF_SFMT_DMOV2R13PIB, FR30BF_SFMT_DMOV2R15PD
 , FR30BF_SFMT_LDRES, FR30BF_SFMT_COPOP, FR30BF_SFMT_ANDCCR, FR30BF_SFMT_STILM
 , FR30BF_SFMT_ADDSP, FR30BF_SFMT_EXTSB, FR30BF_SFMT_EXTUB, FR30BF_SFMT_EXTSH
 , FR30BF_SFMT_EXTUH, FR30BF_SFMT_LDM0, FR30BF_SFMT_LDM1, FR30BF_SFMT_STM0
 , FR30BF_SFMT_STM1, FR30BF_SFMT_ENTER, FR30BF_SFMT_LEAVE, FR30BF_SFMT_XCHB
} FR30BF_SFMT_TYPE;

/* Function unit handlers (user written).  */

extern int fr30bf_model_fr30_1_u_stm (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/, INT /*reglist*/);
extern int fr30bf_model_fr30_1_u_ldm (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/, INT /*reglist*/);
extern int fr30bf_model_fr30_1_u_store (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/, INT /*Ri*/, INT /*Rj*/);
extern int fr30bf_model_fr30_1_u_load (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/, INT /*Rj*/, INT /*Ri*/);
extern int fr30bf_model_fr30_1_u_cti (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/, INT /*Ri*/);
extern int fr30bf_model_fr30_1_u_exec (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/, INT /*Ri*/, INT /*Rj*/, INT /*Ri*/);

/* Profiling before/after handlers (user written) */

extern void fr30bf_model_insn_before (SIM_CPU *, int /*first_p*/);
extern void fr30bf_model_insn_after (SIM_CPU *, int /*last_p*/, int /*cycles*/);

#endif /* FR30BF_DECODE_H */
