#objdump: -dr --prefix-addresses
#name: MIPS ld-ilocks
#source: ld.s
#as:
# Test the ld macro.

.*: +file format .*mips.*

Disassembly of section .text:
[0-9a-f]+ <.text> lw	a0,0\(zero\)
[0-9a-f]+ <[^>]*> lw	a1,4\(zero\)
[0-9a-f]+ <[^>]*> lw	a0,1\(zero\)
[0-9a-f]+ <[^>]*> lw	a1,5\(zero\)
[0-9a-f]+ <[^>]*> lui	at,0x1
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[0-9a-f]+ <[^>]*> lw	a0,-32768\(zero\)
[0-9a-f]+ <[^>]*> lw	a1,-32764\(zero\)
[0-9a-f]+ <[^>]*> lui	at,0x1
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[0-9a-f]+ <[^>]*> lui	at,0x2
[0-9a-f]+ <[^>]*> lw	a0,-23131\(at\)
[0-9a-f]+ <[^>]*> lw	a1,-23127\(at\)
[0-9a-f]+ <[^>]*> lw	a0,0\(a1\)
[0-9a-f]+ <[^>]*> lw	a1,4\(a1\)
[0-9a-f]+ <[^>]*> lw	a0,1\(a1\)
[0-9a-f]+ <[^>]*> lw	a1,5\(a1\)
[0-9a-f]+ <[^>]*> lui	at,0x1
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[0-9a-f]+ <[^>]*> lw	a0,-32768\(a1\)
[0-9a-f]+ <[^>]*> lw	a1,-32764\(a1\)
[0-9a-f]+ <[^>]*> lui	at,0x1
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[0-9a-f]+ <[^>]*> lui	at,0x2
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-23131\(at\)
[0-9a-f]+ <[^>]*> lw	a1,-23127\(at\)
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a0,0\(gp\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,4\(gp\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a0,0\(gp\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lw	a1,4\(gp\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a0,-16384\(gp\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lw	a1,-16380\(gp\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lw	a0,1\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lw	a1,5\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a0,1\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,5\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a0,1\(gp\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,5\(gp\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a0,1\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a1,5\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a0,1\(gp\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lw	a1,5\(gp\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a0,1\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a1,5\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a0,-16383\(gp\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lw	a1,-16379\(gp\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lui	at,0x2
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lw	a0,-23131\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lw	a1,-23127\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lui	at,0x2
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a0,-23131\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,-23127\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x2
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lw	a0,-23131\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,-23127\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x2
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a0,-23131\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a1,-23127\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lui	at,0x2
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lw	a0,-23131\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lw	a1,-23127\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lui	at,0x2
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a0,-23131\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a1,-23127\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lui	at,0x2
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lw	a0,-23131\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lw	a1,-23127\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addu	at,a1,gp
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addu	at,a1,gp
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addu	at,a1,gp
[0-9a-f]+ <[^>]*> lw	a0,-16384\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lw	a1,-16380\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,1\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lw	a1,5\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,1\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,5\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addu	at,a1,gp
[0-9a-f]+ <[^>]*> lw	a0,1\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,5\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,1\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a1,5\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addu	at,a1,gp
[0-9a-f]+ <[^>]*> lw	a0,1\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lw	a1,5\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,1\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a1,5\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addu	at,a1,gp
[0-9a-f]+ <[^>]*> lw	a0,-16383\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lw	a1,-16379\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lui	at,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-32768\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lw	a1,-32764\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lui	at,0x1
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,0\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lw	a1,4\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lui	at,0x2
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-23131\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lw	a1,-23127\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lui	at,0x2
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-23131\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,-23127\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x2
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-23131\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lw	a1,-23127\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lui	at,0x2
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-23131\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lw	a1,-23127\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lui	at,0x2
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-23131\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lw	a1,-23127\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lui	at,0x2
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-23131\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lw	a1,-23127\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lui	at,0x2
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lw	a0,-23131\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lw	a1,-23127\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lwc1	f[45],0\(zero\)
[0-9a-f]+ <[^>]*> lwc1	f[45],4\(zero\)
[0-9a-f]+ <[^>]*> lwc1	f[45],1\(zero\)
[0-9a-f]+ <[^>]*> lwc1	f[45],5\(zero\)
[0-9a-f]+ <[^>]*> lui	at,0x1
[0-9a-f]+ <[^>]*> lwc1	f[45],-32768\(at\)
[0-9a-f]+ <[^>]*> lwc1	f[45],-32764\(at\)
[0-9a-f]+ <[^>]*> lwc1	f[45],-32768\(zero\)
[0-9a-f]+ <[^>]*> lwc1	f[45],-32764\(zero\)
[0-9a-f]+ <[^>]*> lwc1	f[45],0\(a1\)
[0-9a-f]+ <[^>]*> lwc1	f[45],4\(a1\)
[0-9a-f]+ <[^>]*> lwc1	f[45],1\(a1\)
[0-9a-f]+ <[^>]*> lwc1	f[45],5\(a1\)
[0-9a-f]+ <[^>]*> lui	at,0x1
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lwc1	f[45],-32768\(at\)
[0-9a-f]+ <[^>]*> lwc1	f[45],-32764\(at\)
[0-9a-f]+ <[^>]*> lwc1	f[45],-32768\(a1\)
[0-9a-f]+ <[^>]*> lwc1	f[45],-32764\(a1\)
[0-9a-f]+ <[^>]*> lui	at,0x2
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> lwc1	f[45],-23131\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lwc1	f[45],-23127\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> nop
[0-9a-f]+ <[^>]*> swc1	f[45],0\(zero\)
[0-9a-f]+ <[^>]*> swc1	f[45],4\(zero\)
[0-9a-f]+ <[^>]*> swc1	f[45],1\(zero\)
[0-9a-f]+ <[^>]*> swc1	f[45],5\(zero\)
[0-9a-f]+ <[^>]*> lui	at,0x1
[0-9a-f]+ <[^>]*> swc1	f[45],-32768\(at\)
[0-9a-f]+ <[^>]*> swc1	f[45],-32764\(at\)
[0-9a-f]+ <[^>]*> swc1	f[45],-32768\(zero\)
[0-9a-f]+ <[^>]*> swc1	f[45],-32764\(zero\)
[0-9a-f]+ <[^>]*> swc1	f[45],0\(a1\)
[0-9a-f]+ <[^>]*> swc1	f[45],4\(a1\)
[0-9a-f]+ <[^>]*> swc1	f[45],1\(a1\)
[0-9a-f]+ <[^>]*> swc1	f[45],5\(a1\)
[0-9a-f]+ <[^>]*> lui	at,0x1
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> swc1	f[45],-32768\(at\)
[0-9a-f]+ <[^>]*> swc1	f[45],-32764\(at\)
[0-9a-f]+ <[^>]*> swc1	f[45],-32768\(a1\)
[0-9a-f]+ <[^>]*> swc1	f[45],-32764\(a1\)
[0-9a-f]+ <[^>]*> lui	at,0x2
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addu	at,a1,at
[0-9a-f]+ <[^>]*> swc1	f[45],-23131\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> swc1	f[45],-23127\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> sw	a0,0\(zero\)
[0-9a-f]+ <[^>]*> sw	a1,4\(zero\)
[0-9a-f]+ <[^>]*> lui	a0,0x2
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> (d|)addu	a0,a0,a1
[0-9a-f]+ <[^>]*> ld	a0,-23131\(a0\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lui	at,0x2
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> (d|)addu	at,at,a1
[0-9a-f]+ <[^>]*> sd	a0,-23131\(at\)
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> nop

