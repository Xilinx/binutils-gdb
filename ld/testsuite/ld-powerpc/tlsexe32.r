#source: tls32.s
#source: tlslib32.s
#as: -a32
#ld: -melf32ppc
#readelf: -WSsrl
#target: powerpc*-*-*

There are 21 section headers.*

Section Headers:
 +\[Nr\] Name +Type +Addr +Off +Size +ES Flg Lk Inf Al
 +\[ 0\] +NULL +00000000 000000 000000 00 +0 +0 +0
 +\[ 1\] \.interp +.*
 +\[ 2\] \.hash +.*
 +\[ 3\] \.dynsym +.*
 +\[ 4\] \.dynstr +.*
 +\[ 5\] \.rela\.dyn +.*
 +\[ 6\] \.rela\.plt +.*
 +\[ 7\] \.text +PROGBITS +0180028c 00028c 000070 00 +AX +0 +0 +1
 +\[ 8\] \.sdata2 +PROGBITS +.*
 +\[ 9\] \.tdata +PROGBITS +018102fc 0002fc 00001c 00 WAT +0 +0 +4
 +\[10\] \.tbss +NOBITS +01810318 000318 00001c 00 WAT +0 +0 +4
 +\[11\] \.dynamic +DYNAMIC +01810318 000318 0000a0 08 +WA +4 +0 +4
 +\[12\] \.data +PROGBITS +018103b8 0003b8 000000 00 +WA +0 +0 +1
 +\[13\] \.got +PROGBITS +018103b8 0003b8 00001c 04 WAX +0 +0 +4
 +\[14\] \.sdata +PROGBITS +018103d4 0003d4 000000 00 +WA +0 +0 +4
 +\[15\] \.sbss +NOBITS +018103d4 0003d4 000000 00 +WA +0 +0 +1
 +\[16\] \.plt +NOBITS +.*
 +\[17\] \.bss +NOBITS +.*
 +\[18\] \.shstrtab +STRTAB +.*
 +\[19\] \.symtab +SYMTAB +.*
 +\[20\] \.strtab +STRTAB +.*
#...

Elf file type is EXEC \(Executable file\)
Entry point 0x180028c
There are 6 program headers, starting at offset 52

Program Headers:
 +Type +Offset +VirtAddr +PhysAddr +FileSiz MemSiz +Flg Align
 +PHDR +0x000034 0x01800034 0x01800034 0x000e0 0x000e0 R E 0x4
 +INTERP +0x000114 0x01800114 0x01800114 0x00011 0x00011 R +0x1
 +\[Requesting program interpreter: .*\]
 +LOAD +0x000000 0x01800000 0x01800000 0x002fc 0x002fc R E 0x10000
 +LOAD +0x0002fc 0x018102fc 0x018102fc 0x000d8 0x0012c RWE 0x10000
 +DYNAMIC +0x000318 0x01810318 0x01810318 0x000a0 0x000a0 RW +0x4
 +TLS +0x0002fc 0x018102fc 0x018102fc 0x0001c 0x00038 R +0x4

 Section to Segment mapping:
 +Segment Sections\.\.\.
 +00 +
 +01 +\.interp 
 +02 +\.interp \.hash \.dynsym \.dynstr \.rela\.dyn \.rela\.plt \.text 
 +03 +\.tdata \.tbss \.dynamic \.got \.plt 
 +04 +\.tbss \.dynamic 
 +05 +\.tdata \.tbss 

Relocation section '\.rela\.dyn' at offset 0x268 contains 2 entries:
 Offset +Info +Type +Sym\. Value +Symbol's Name \+ Addend
018103c8 +00000249 R_PPC_TPREL32 +00000000 +gd \+ 0
018103cc +00000444 R_PPC_DTPMOD32 +00000000 +ld \+ 0

Relocation section '\.rela\.plt' at offset 0x280 contains 1 entries:
 Offset +Info +Type +Sym\. Value +Symbol's Name \+ Addend
0181041c +00000315 R_PPC_JMP_SLOT +0181041c +__tls_get_addr \+ 0

Symbol table '\.dynsym' contains 10 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +0: 00000000 +0 NOTYPE +LOCAL +DEFAULT +UND 
 +1: 01810318 +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +2: 00000000 +0 TLS +GLOBAL DEFAULT +UND gd
 +3: 0181041c +0 FUNC +GLOBAL DEFAULT +UND __tls_get_addr
 +4: 00000000 +0 TLS +GLOBAL DEFAULT +UND ld
 +5: 01810428 +0 NOTYPE +GLOBAL DEFAULT +ABS __end
 +6: 018103d4 +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +7: 018103d4 +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +8: 018103bc +0 OBJECT +GLOBAL DEFAULT +ABS _GLOBAL_OFFSET_TABLE_
 +9: 01810428 +0 NOTYPE +GLOBAL DEFAULT +ABS _end

Symbol table '\.symtab' contains 47 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +0: 00000000 +0 NOTYPE +LOCAL +DEFAULT +UND 
 +1: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +1 
 +2: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +2 
 +3: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +3 
 +4: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +4 
 +5: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +5 
 +6: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +6 
 +7: 0180028c +0 SECTION LOCAL +DEFAULT +7 
 +8: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +8 
 +9: 018102fc +0 SECTION LOCAL +DEFAULT +9 
 +10: 01810318 +0 SECTION LOCAL +DEFAULT +10 
 +11: 01810318 +0 SECTION LOCAL +DEFAULT +11 
 +12: 018103b8 +0 SECTION LOCAL +DEFAULT +12 
 +13: 018103b8 +0 SECTION LOCAL +DEFAULT +13 
 +14: 018103d4 +0 SECTION LOCAL +DEFAULT +14 
 +15: 018103d4 +0 SECTION LOCAL +DEFAULT +15 
 +16: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +16 
 +17: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +17 
 +18: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +18 
 +19: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +19 
 +20: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +20 
 +21: 00000000 +0 TLS +LOCAL +DEFAULT +9 gd4
 +22: 00000004 +0 TLS +LOCAL +DEFAULT +9 ld4
 +23: 00000008 +0 TLS +LOCAL +DEFAULT +9 ld5
 +24: 0000000c +0 TLS +LOCAL +DEFAULT +9 ld6
 +25: 00000010 +0 TLS +LOCAL +DEFAULT +9 ie4
 +26: 00000014 +0 TLS +LOCAL +DEFAULT +9 le4
 +27: 00000018 +0 TLS +LOCAL +DEFAULT +9 le5
 +28: 01810318 +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +29: 00000000 +0 TLS +GLOBAL DEFAULT +UND gd
 +30: 00000030 +0 TLS +GLOBAL DEFAULT +10 le0
 +31: [0-9a-f]+ +0 FUNC +GLOBAL DEFAULT +UND __tls_get_addr
 +32: 00000020 +0 TLS +GLOBAL DEFAULT +10 ld0
 +33: 00000034 +0 TLS +GLOBAL DEFAULT +10 le1
 +34: 00000000 +0 TLS +GLOBAL DEFAULT +UND ld
 +35: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +7 _start
 +36: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS __end
 +37: [0-9a-f]+ +0 OBJECT +GLOBAL DEFAULT +14 _SDA_BASE_
 +38: 00000028 +0 TLS +GLOBAL DEFAULT +10 ld2
 +39: 00000024 +0 TLS +GLOBAL DEFAULT +10 ld1
 +40: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +41: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +42: 018103bc +0 OBJECT +GLOBAL DEFAULT +ABS _GLOBAL_OFFSET_TABLE_
 +43: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _end
 +44: 0000001c +0 TLS +GLOBAL DEFAULT +10 gd0
 +45: 0000002c +0 TLS +GLOBAL DEFAULT +10 ie0
 +46: [0-9a-f]+ +0 OBJECT +GLOBAL DEFAULT +8 _SDA2_BASE_
