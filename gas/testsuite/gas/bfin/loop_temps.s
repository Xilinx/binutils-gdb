	.align 4
.global _filter_mem16;
.type _filter_mem16, STT_FUNC;
_filter_mem16:
	[--sp] = ( r7:7 );

	[--SP] = R4;
	[--SP] = R5;
	[--SP] = P3;
	[--SP] = P4;
	LINK 68;
	[FP+28] = R0;
	[FP+32] = R1;
	[FP+36] = R2;
	[FP+-68] = SP;
	R0 = SP;
	[FP+-24] = R0;
	R0 = [FP+44];
	R3 = R0;
	R3 += 1;
	R2 = R3;
	R0 = R2;
	R1 = 0 (X);
	R2 = -1 (X);
	R0 = R0 & R2;
	R2 = 15 (X);
	R1 = R1 & R2;
	R2 = R0 >> 27;
	R7 = R1 << 5;
	[FP+-60] = R7;
	R7 = [FP+-60];
	R7 = R2 | R7;
	[FP+-60] = R7;
	R2 = R0 << 5;
	[FP+-64] = R2;
	R0 = [FP+-64];
	R1 = [FP+-60];
	[FP+-64] = R0;
	[FP+-60] = R1;
	R0 = -1 (X);
	R1 = [FP+-64];
	R1 = R1 & R0;
	[FP+-64] = R1;
	R0 = 15 (X);
	R2 = [FP+-60];
	R2 = R2 & R0;
	R2 = R3;
	R0 = R2;
	R1 = 0 (X);
	R2 = -1 (X);
	R0 = R0 & R2;
	R2 = 15 (X);
	R1 = R1 & R2;
	R2 = R0 >> 27;
	R7 = R1 << 5;
	[FP+-52] = R7;
	R7 = [FP+-52];
	R7 = R2 | R7;
	[FP+-52] = R7;
	R2 = R0 << 5;
	[FP+-56] = R2;
	R0 = [FP+-56];
	R1 = [FP+-52];
	[FP+-56] = R0;
	[FP+-52] = R1;
	R0 = -1 (X);
	R1 = [FP+-56];
	R1 = R1 & R0;
	[FP+-56] = R1;
	R0 = 15 (X);
	R2 = [FP+-52];
	R2 = R2 & R0;
	R0 = R3;
	R0 <<= 2;
	R0 += 3;
	R0 += 3;
	R0 >>= 2;
	P1 = R0;
	P2 = P1 << 2;
	SP -= P2;
	[FP+-48] = SP;
	R0 = [FP+-48];
	R0 += 3;
	R0 >>= 2;
	R0 <<= 2;
	[FP+-48] = R0;
	R0 = [FP+-48];
	[FP+-12] = R0;
	R0 = [FP+48];
	R0 += 1;
	R3 = R0 << 1;
	R2 = R3;
	R0 = R2;
	R1 = 0 (X);
	R2 = -1 (X);
	R0 = R0 & R2;
	R2 = 15 (X);
	R1 = R1 & R2;
	R2 = R0 >> 27;
	R7 = R1 << 5;
	[FP+-40] = R7;
	R7 = [FP+-40];
	R7 = R2 | R7;
	[FP+-40] = R7;
	R2 = R0 << 5;
	[FP+-44] = R2;
	R0 = [FP+-44];
	R1 = [FP+-40];
	[FP+-44] = R0;
	[FP+-40] = R1;
	R0 = -1 (X);
	R1 = [FP+-44];
	R1 = R1 & R0;
	[FP+-44] = R1;
	R0 = 15 (X);
	R2 = [FP+-40];
	R2 = R2 & R0;
	R2 = R3;
	R0 = R2;
	R1 = 0 (X);
	R2 = -1 (X);
	R0 = R0 & R2;
	R2 = 15 (X);
	R1 = R1 & R2;
	R2 = R0 >> 27;
	R7 = R1 << 5;
	[FP+-32] = R7;
	R7 = [FP+-32];
	R7 = R2 | R7;
	[FP+-32] = R7;
	R2 = R0 << 5;
	[FP+-36] = R2;
	R0 = [FP+-36];
	R1 = [FP+-32];
	[FP+-36] = R0;
	[FP+-32] = R1;
	R0 = -1 (X);
	R1 = [FP+-36];
	R1 = R1 & R0;
	[FP+-36] = R1;
	R0 = 15 (X);
	R2 = [FP+-32];
	R2 = R2 & R0;
	R0 = R3;
	R0 <<= 2;
	R0 += 3;
	R0 += 3;
	R0 >>= 2;
	P1 = R0;
	P2 = P1 << 2;
	SP -= P2;
	[FP+-28] = SP;
	R0 = [FP+-28];
	R0 += 3;
	R0 >>= 2;
	R0 <<= 2;
	[FP+-28] = R0;
	R0 = [FP+-28];
	[FP+-8] = R0;
	R0 = [FP+-12];
	R0 += 4;
	[FP+-16] = R0;
	R0 = [FP+-8];
	[FP+-20] = R0;
	R0 = 0 (X);
	[FP+-4] = R0;
	jump.s .L6;
.L7:
	R1 = [FP+-20];
	R0 = [FP+-4];
	R0 <<= 2;
	R1 = R1 + R0;
	P1 = R1;
	R0 = [FP+-4];
	R1 = R0 << 1;
	R0 = [FP+32];
	R0 = R0 + R1;
	P2 = R0;
	R0 = W [P2] (Z);
	W [P1] = R0;
	R1 = [FP+-20];
	R0 = [FP+-4];
	R0 <<= 1;
	R0 += 1;
	R0 <<= 1;
	R1 = R1 + R0;
	P1 = R1;
	R0 = [FP+-4];
	R1 = R0 << 1;
	R0 = [FP+36];
	R0 = R0 + R1;
	P2 = R0;
	R0 = W [P2] (Z);
	W [P1] = R0;
	R0 = [FP+-4];
	R0 += 1;
	[FP+-4] = R0;
.L6:
	R1 = [FP+48];
	R0 = [FP+-4];
	cc =R0<R1;
	if cc jump .L7;
	R0 = [FP+48];
	P0 = [FP+-20];
	I0 = P0;
	B0 = P0;
	L0 = 0;
	P2 = [FP+-16];
	I2 = P2;
	L2 = 0;
	P4 = [FP+52];
	P0 = [FP+28];
	P1 = [FP+40];
	R1 = [P4++];
	R1 <<= 3;
	R1.L = R1 (RND);
	R2 = W[P0++];
	R1.L = R1.L + R2.L;
	W[P1++] = R1;
	R2 = PACK(R1.L, R2.L);
	[P2] = R2;
	R0 += -1;
	R3 = 0;
	LC0 = R0;
	LOOP filter_start256 LC0;
	LOOP_BEGIN filter_start256;
	R3 += 1;
	LC1 = R3;
	R1 = [P4++];
	A1 = R1;
	A0 = 0;
	I0 = B0;
	I2 = P2;
	P2 += 4;
	R4 = [I0++] || R5 = [I2--];
	LOOP filter_start_inner256 LC1;
	LOOP_BEGIN filter_start_inner256;
	A1 -= R4.H*R5.H, A0 += R4.L*R5.L (IS) || R4 = [I0++] || R5 = [I2--];
	LOOP_END filter_start_inner256;
	A0 += A1;
	R4 = A0;
	R4 <<= 3;
	R4.L = R4 (RND);
	R2 = W[P0++];
	R4.L = R4.L + R2.L;
	W[P1++] = R4;
	R2 = PACK(R4.L, R2.L);
	[P2] = R2;
	LOOP_END filter_start256;
	R0 = [FP+48];
	R0 <<= 1;
	I0 = B0;
	R0 <<= 1;
	L0 = R0;
	R0 = [FP+48];
	R2 = [FP+44];
	R2 = R2 - R0;
	R4 = [I0++];
	LC0 = R2;
	P3 = R0;
	R0 <<= 2;
	R0 += 8;
	I2 = P2;
	M0 = R0;
	A1 = A0 = 0;
	R5 = [I2--];
	LOOP filter_mid256 LC0;
	LOOP_BEGIN filter_mid256;
	LOOP filter_mid_inner256 LC1=P3;
	LOOP_BEGIN filter_mid_inner256;
	A1 -= R4.H*R5.H, A0 += R4.L*R5.L (IS) || R4 = [I0++] || R5 = [I2--];
	LOOP_END filter_mid_inner256;
	R0 = (A0 += A1) || I2 += M0;
	R0 = R0 << 3 || R5 = W[P0++];
	R0.L = R0 (RND);
	R0.L = R0.L + R5.L;
	R5 = PACK(R0.L, R5.L) || W[P1++] = R0;
	A1 = A0 = 0 || [I2--] = R5
	LOOP_END filter_mid256;
	I2 += 4;
	P2 = I2;
	P4 = [FP+52];
	R0 = [FP+48];
	LC0 = R0;
	P0 = B0;
	A1 = A0 = 0;
	LOOP mem_update256 LC0;
	LOOP_BEGIN mem_update256;
	I2 = P2;
	I0 = P0;
	P0 += 4;
	R0 = LC0;
	LC1 = R0;
	R5 = [I2--] || R4 = [I0++];
	LOOP mem_accum256 LC1;
	LOOP_BEGIN mem_accum256;
	A1 -= R4.H*R5.H, A0 += R4.L*R5.L (IS) || R4 = [I0++] || R5 = [I2--];
	LOOP_END mem_accum256;
	R0 = (A0 += A1);
	A1 = A0 = 0 || [P4++] = R0;
	LOOP_END mem_update256;
	L0 = 0;
	SP = [FP+-24];
	SP = [FP+-68];
	UNLINK;
	P4 = [SP++];
	P3 = [SP++];
	R5 = [SP++];
	R4 = [SP++];
	( r7:7 ) = [sp++];
	rts;
	.size	_filter_mem16, .-_filter_mem16
