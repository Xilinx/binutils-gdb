#objdump: -dr --prefix-addresses
#name: NIOS2 stw

# Test the ld instruction

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> stw	r4,0\(zero\)
0+0004 <[^>]*> stw	r4,4\(zero\)
0+0008 <[^>]*> stw	r4,32764\(zero\)
0+000c <[^>]*> stw	r4,-32768\(zero\)
0+0010 <[^>]*> stw	r4,0\(r5\)
0+0014 <[^>]*> stw	r4,4\(r5\)
0+0018 <[^>]*> stw	r4,32764\(r5\)
0+001c <[^>]*> stw	r4,-32768\(r5\)
0+0020 <[^>]*> stw	r4,0\(zero\)
[	]*20: R_NIOS2_S16	.data
0+0024 <[^>]*> stw	r4,0\(zero\)
[	]*24: R_NIOS2_S16	big_external_data_label
0+0028 <[^>]*> stw	r4,0\(zero\)
[	]*28: R_NIOS2_S16	small_external_data_label
0+002c <[^>]*> stw	r4,0\(zero\)
[	]*2c: R_NIOS2_S16	big_external_common
0+0030 <[^>]*> stw	r4,0\(zero\)
[	]*30: R_NIOS2_S16	small_external_common
0+0034 <[^>]*> stw	r4,0\(zero\)
[	]*34: R_NIOS2_S16	.bss
0+0038 <[^>]*> stw	r4,0\(zero\)
[	]*38: R_NIOS2_S16	.bss\+0x4000
0+003c <[^>]*> stw	r4,0\(zero\)
[	]*3c: R_NIOS2_S16	.data\+0x4
0+0040 <[^>]*> stw	r4,0\(zero\)
[	]*40: R_NIOS2_S16	big_external_data_label\+0x4
0+0044 <[^>]*> stw	r4,0\(zero\)
[	]*44: R_NIOS2_S16	small_external_data_label\+0x4
0+0048 <[^>]*> stw	r4,0\(zero\)
[	]*48: R_NIOS2_S16	big_external_common\+0x4
0+004c <[^>]*> stw	r4,0\(zero\)
[	]*4c: R_NIOS2_S16	small_external_common\+0x4
0+0050 <[^>]*> stw	r4,0\(zero\)
[	]*50: R_NIOS2_S16	.bss\+0x4
0+0054 <[^>]*> stw	r4,0\(zero\)
[	]*54: R_NIOS2_S16	.bss\+0x4004
0+0058 <[^>]*> stw	r4,0\(zero\)
[	]*58: R_NIOS2_S16	.data-0x8000
0+005c <[^>]*> stw	r4,0\(zero\)
[	]*5c: R_NIOS2_S16	big_external_data_label-0x8000
0+0060 <[^>]*> stw	r4,0\(zero\)
[	]*60: R_NIOS2_S16	small_external_data_label-0x8000
0+0064 <[^>]*> stw	r4,0\(zero\)
[	]*64: R_NIOS2_S16	big_external_common-0x8000
0+0068 <[^>]*> stw	r4,0\(zero\)
[	]*68: R_NIOS2_S16	small_external_common-0x8000
0+006c <[^>]*> stw	r4,0\(zero\)
[	]*6c: R_NIOS2_S16	.bss-0x8000
0+0070 <[^>]*> stw	r4,0\(zero\)
[	]*70: R_NIOS2_S16	.bss-0x4000
0+0074 <[^>]*> stw	r4,0\(zero\)
[	]*74: R_NIOS2_S16	.data\+0x10000
0+0078 <[^>]*> stw	r4,0\(r5\)
[	]*78: R_NIOS2_S16	.data
0+007c <[^>]*> stw	r4,0\(r5\)
[	]*7c: R_NIOS2_S16	big_external_data_label
0+0080 <[^>]*> stw	r4,0\(r5\)
[	]*80: R_NIOS2_S16	small_external_data_label
0+0084 <[^>]*> stw	r4,0\(r5\)
[	]*84: R_NIOS2_S16	big_external_common
0+0088 <[^>]*> stw	r4,0\(r5\)
[	]*88: R_NIOS2_S16	small_external_common
0+008c <[^>]*> stw	r4,0\(r5\)
[	]*8c: R_NIOS2_S16	.bss
0+0090 <[^>]*> stw	r4,0\(r5\)
[	]*90: R_NIOS2_S16	.bss\+0x4000
0+0094 <[^>]*> stw	r4,0\(r5\)
[	]*94: R_NIOS2_S16	.data\+0x4
0+0098 <[^>]*> stw	r4,0\(r5\)
[	]*98: R_NIOS2_S16	big_external_data_label\+0x4
0+009c <[^>]*> stw	r4,0\(r5\)
[	]*9c: R_NIOS2_S16	small_external_data_label\+0x4
0+00a0 <[^>]*> stw	r4,0\(r5\)
[	]*a0: R_NIOS2_S16	big_external_common\+0x4
0+00a4 <[^>]*> stw	r4,0\(r5\)
[	]*a4: R_NIOS2_S16	small_external_common\+0x4
0+00a8 <[^>]*> stw	r4,0\(r5\)
[	]*a8: R_NIOS2_S16	.bss\+0x4
0+00ac <[^>]*> stw	r4,0\(r5\)
[	]*ac: R_NIOS2_S16	.bss\+0x4004
0+00b0 <[^>]*> stw	r4,0\(r5\)
[	]*b0: R_NIOS2_S16	.data-0x8000
0+00b4 <[^>]*> stw	r4,0\(r5\)
[	]*b4: R_NIOS2_S16	big_external_data_label-0x8000
0+00b8 <[^>]*> stw	r4,0\(r5\)
[	]*b8: R_NIOS2_S16	small_external_data_label-0x8000
0+00bc <[^>]*> stw	r4,0\(r5\)
[	]*bc: R_NIOS2_S16	big_external_common-0x8000
0+00c0 <[^>]*> stw	r4,0\(r5\)
[	]*c0: R_NIOS2_S16	small_external_common-0x8000
0+00c4 <[^>]*> stw	r4,0\(r5\)
[	]*c4: R_NIOS2_S16	.bss-0x8000
0+00c8 <[^>]*> stw	r4,0\(r5\)
[	]*c8: R_NIOS2_S16	.bss-0x4000
0+00cc <[^>]*> stwio	r4,0\(zero\)
0+00d0 <[^>]*> stwio	r4,4\(zero\)
0+00d4 <[^>]*> stwio	r4,32764\(zero\)
0+00d8 <[^>]*> stwio	r4,-32768\(zero\)
0+00dc <[^>]*> stwio	r4,0\(r5\)
0+00e0 <[^>]*> stwio	r4,4\(r5\)
0+00e4 <[^>]*> stwio	r4,32764\(r5\)
0+00e8 <[^>]*> stwio	r4,-32768\(r5\)
0+00ec <[^>]*> stwio	r4,0\(zero\)
[	]*ec: R_NIOS2_S16	.data
0+00f0 <[^>]*> stwio	r4,0\(zero\)
[	]*f0: R_NIOS2_S16	big_external_data_label
0+00f4 <[^>]*> stwio	r4,0\(zero\)
[	]*f4: R_NIOS2_S16	small_external_data_label
0+00f8 <[^>]*> stwio	r4,0\(zero\)
[	]*f8: R_NIOS2_S16	big_external_common
0+00fc <[^>]*> stwio	r4,0\(zero\)
[	]*fc: R_NIOS2_S16	small_external_common
0+0100 <[^>]*> stwio	r4,0\(zero\)
[	]*100: R_NIOS2_S16	.bss
0+0104 <[^>]*> stwio	r4,0\(zero\)
[	]*104: R_NIOS2_S16	.bss\+0x4000
0+0108 <[^>]*> stwio	r4,0\(zero\)
[	]*108: R_NIOS2_S16	.data\+0x4
0+010c <[^>]*> stwio	r4,0\(zero\)
[	]*10c: R_NIOS2_S16	big_external_data_label\+0x4
0+0110 <[^>]*> stwio	r4,0\(zero\)
[	]*110: R_NIOS2_S16	small_external_data_label\+0x4
0+0114 <[^>]*> stwio	r4,0\(zero\)
[	]*114: R_NIOS2_S16	big_external_common\+0x4
0+0118 <[^>]*> stwio	r4,0\(zero\)
[	]*118: R_NIOS2_S16	small_external_common\+0x4
0+011c <[^>]*> stwio	r4,0\(zero\)
[	]*11c: R_NIOS2_S16	.bss\+0x4
0+0120 <[^>]*> stwio	r4,0\(zero\)
[	]*120: R_NIOS2_S16	.bss\+0x4004
0+0124 <[^>]*> stwio	r4,0\(zero\)
[	]*124: R_NIOS2_S16	.data-0x8000
0+0128 <[^>]*> stwio	r4,0\(zero\)
[	]*128: R_NIOS2_S16	big_external_data_label-0x8000
0+012c <[^>]*> stwio	r4,0\(zero\)
[	]*12c: R_NIOS2_S16	small_external_data_label-0x8000
0+0130 <[^>]*> stwio	r4,0\(zero\)
[	]*130: R_NIOS2_S16	big_external_common-0x8000
0+0134 <[^>]*> stwio	r4,0\(zero\)
[	]*134: R_NIOS2_S16	small_external_common-0x8000
0+0138 <[^>]*> stwio	r4,0\(zero\)
[	]*138: R_NIOS2_S16	.bss-0x8000
0+013c <[^>]*> stwio	r4,0\(zero\)
[	]*13c: R_NIOS2_S16	.bss-0x4000
0+0140 <[^>]*> stwio	r4,0\(zero\)
[	]*140: R_NIOS2_S16	.data\+0x10000
0+0144 <[^>]*> stwio	r4,0\(r5\)
[	]*144: R_NIOS2_S16	.data
0+0148 <[^>]*> stwio	r4,0\(r5\)
[	]*148: R_NIOS2_S16	big_external_data_label
0+014c <[^>]*> stwio	r4,0\(r5\)
[	]*14c: R_NIOS2_S16	small_external_data_label
0+0150 <[^>]*> stwio	r4,0\(r5\)
[	]*150: R_NIOS2_S16	big_external_common
0+0154 <[^>]*> stwio	r4,0\(r5\)
[	]*154: R_NIOS2_S16	small_external_common
0+0158 <[^>]*> stwio	r4,0\(r5\)
[	]*158: R_NIOS2_S16	.bss
0+015c <[^>]*> stwio	r4,0\(r5\)
[	]*15c: R_NIOS2_S16	.bss\+0x4000
0+0160 <[^>]*> stwio	r4,0\(r5\)
[	]*160: R_NIOS2_S16	.data\+0x4
0+0164 <[^>]*> stwio	r4,0\(r5\)
[	]*164: R_NIOS2_S16	big_external_data_label\+0x4
0+0168 <[^>]*> stwio	r4,0\(r5\)
[	]*168: R_NIOS2_S16	small_external_data_label\+0x4
0+016c <[^>]*> stwio	r4,0\(r5\)
[	]*16c: R_NIOS2_S16	big_external_common\+0x4
0+0170 <[^>]*> stwio	r4,0\(r5\)
[	]*170: R_NIOS2_S16	small_external_common\+0x4
0+0174 <[^>]*> stwio	r4,0\(r5\)
[	]*174: R_NIOS2_S16	.bss\+0x4
0+0178 <[^>]*> stwio	r4,0\(r5\)
[	]*178: R_NIOS2_S16	.bss\+0x4004
0+017c <[^>]*> stwio	r4,0\(r5\)
[	]*17c: R_NIOS2_S16	.data-0x8000
0+0180 <[^>]*> stwio	r4,0\(r5\)
[	]*180: R_NIOS2_S16	big_external_data_label-0x8000
0+0184 <[^>]*> stwio	r4,0\(r5\)
[	]*184: R_NIOS2_S16	small_external_data_label-0x8000
0+0188 <[^>]*> stwio	r4,0\(r5\)
[	]*188: R_NIOS2_S16	big_external_common-0x8000
0+018c <[^>]*> stwio	r4,0\(r5\)
[	]*18c: R_NIOS2_S16	small_external_common-0x8000
0+0190 <[^>]*> stwio	r4,0\(r5\)
[	]*190: R_NIOS2_S16	.bss-0x8000
0+0194 <[^>]*> stwio	r4,0\(r5\)
[	]*194: R_NIOS2_S16	.bss-0x4000
