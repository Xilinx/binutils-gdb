#source: tls.s
#source: tlslib.s
#as: -a64
#ld: -melf64ppc
#readelf: -WSsrl
#target: powerpc64*-*-*

There are 22 section headers.*

Section Headers:
 +\[Nr\] Name +Type +Address +Off +Size +ES Flg Lk Inf Al
 +\[ 0\] +NULL +0+ 0+ 0+ 0+ +0 +0 +0
 +\[ 1\] \.interp +PROGBITS +0+10000190 0+190 0+11 0+ +A +0 +0 +1
 +\[ 2\] \.hash +HASH +0+100001a8 0+1a8 0+40 04 +A +3 +0 +8
 +\[ 3\] \.dynsym +DYNSYM +0+100001e8 0+1e8 0+108 18 +A +4 +1 +8
 +\[ 4\] \.dynstr +STRTAB +0+100002f0 0+2f0 0+51 0+ +A +0 +0 +1
 +\[ 5\] \.rela\.dyn +RELA +0+10000348 0+348 0+30 18 +A +3 +0 +8
 +\[ 6\] \.rela\.plt +RELA +0+10000378 0+378 0+18 18 +A +3 +11 +8
 +\[ 7\] \.text +PROGBITS +0+10000390 0+390 0+dc 0+ +AX +0 +0 +4
 +\[ 8\] \.data +PROGBITS +0+10010470 0+470 0+ 0+ +WA +0 +0 +1
 +\[ 9\] \.branch_lt +PROGBITS +0+10010470 0+470 0+ 0+ +WA +0 +0 +8
 +\[10\] \.tdata +PROGBITS +0+10010470 0+470 0+38 0+ WAT +0 +0 +8
 +\[11\] \.tbss +NOBITS +0+100104a8 0+4a8 0+38 0+ WAT +0 +0 +8
 +\[12\] \.dynamic +DYNAMIC +0+100104a8 0+4a8 0+150 10 +WA +4 +0 +8
 +\[13\] \.ctors +PROGBITS +0+100105f8 0+628 0+ 0+ +W +0 +0 +1
 +\[14\] \.dtors +PROGBITS +0+100105f8 0+628 0+ 0+ +W +0 +0 +1
 +\[15\] \.got +PROGBITS +0+100105f8 0+5f8 0+30 08 +WA +0 +0 +8
 +\[16\] \.sbss +PROGBITS +0+10010628 0+628 0+ 0+ +W +0 +0 +1
 +\[17\] \.plt +NOBITS +0+10010628 0+628 0+30 18 +WA +0 +0 +8
 +\[18\] \.bss +NOBITS +0+10010658 0+628 0+ 0+ +WA +0 +0 +1
 +\[19\] \.shstrtab +STRTAB +0+ 0+628 0+98 0+ +0 +0 +1
 +\[20\] \.symtab +SYMTAB +0+ 0+c40 0+450 18 +21 +1d +8
 +\[21\] \.strtab +STRTAB +0+ 0+1090 0+8c 0+ +0 +0 +1
#...

Elf file type is EXEC \(Executable file\)
Entry point 0x100003ac
There are 6 program headers.*

Program Headers:
 +Type +Offset +VirtAddr +PhysAddr +FileSiz +MemSiz +Flg Align
 +PHDR +0x0+40 0x0+10000040 0x0+10000040 0x0+150 0x0+150 R E 0x8
 +INTERP +0x0+190 0x0+10000190 0x0+10000190 0x0+11 0x0+11 R +0x1
 +\[Requesting program interpreter: .*\]
 +LOAD +0x0+ 0x0+10000000 0x0+10000000 0x0+46c 0x0+46c R E 0x10000
 +LOAD +0x0+470 0x0+10010470 0x0+10010470 0x0+1b8 0x0+1e8 RW +0x10000
 +DYNAMIC +0x0+4a8 0x0+100104a8 0x0+100104a8 0x0+150 0x0+150 RW +0x8
 +TLS +0x0+470 0x0+10010470 0x0+10010470 0x0+38 0x0+70 R +0x8

 Section to Segment mapping:
 +Segment Sections\.\.\.
 +0+ +
 +01 +\.interp 
 +02 +\.interp \.hash \.dynsym \.dynstr \.rela\.dyn \.rela\.plt \.text 
 +03 +\.tdata \.tbss \.dynamic \.got \.plt 
 +04 +\.tbss \.dynamic 
 +05 +\.tdata \.tbss 

Relocation section '\.rela\.dyn' at offset .* contains 2 entries:
 +Offset +Info +Type +Symbol's Value +Symbol's Name \+ Addend
0+10010618 +0+200000049 R_PPC64_TPREL64 +0+ gd \+ 0
0+10010620 +0+70000004e R_PPC64_DTPREL64 +0+50 ld2 \+ 0

Relocation section '\.rela\.plt' at offset .* contains 1 entries:
 +Offset +Info +Type +Symbol's Value +Symbol's Name \+ Addend
0+10010640 +0+300000015 R_PPC64_JMP_SLOT +0+ __tls_get_addr \+ 0

Symbol table '\.dynsym' contains 11 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +0: 0+ +0 NOTYPE +LOCAL +DEFAULT +UND 
 +1: 0+100104a8 +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +2: 0+ +0 TLS +GLOBAL DEFAULT +UND gd
 +3: 0+ +0 NOTYPE +GLOBAL DEFAULT +UND __tls_get_addr
 +4: 0+ +0 FUNC +GLOBAL DEFAULT +UND \.__tls_get_addr
 +5: 0+ +0 TLS +GLOBAL DEFAULT +UND ld
 +6: 0+10010658 +0 NOTYPE +GLOBAL DEFAULT +ABS __end
 +7: 0+50 +0 TLS +GLOBAL DEFAULT +11 ld2
 +8: 0+10010628 +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +9: 0+10010628 +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +10: 0+10010658 +0 NOTYPE +GLOBAL DEFAULT +ABS _end

Symbol table '\.symtab' contains 46 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +0: 0+ +0 NOTYPE +LOCAL +DEFAULT +UND 
 +1: 0+10000190 +0 SECTION LOCAL +DEFAULT +1 
 +2: 0+100001a8 +0 SECTION LOCAL +DEFAULT +2 
 +3: 0+100001e8 +0 SECTION LOCAL +DEFAULT +3 
 +4: 0+100002f0 +0 SECTION LOCAL +DEFAULT +4 
 +5: 0+10000348 +0 SECTION LOCAL +DEFAULT +5 
 +6: 0+10000378 +0 SECTION LOCAL +DEFAULT +6 
 +7: 0+10000390 +0 SECTION LOCAL +DEFAULT +7 
 +8: 0+10010470 +0 SECTION LOCAL +DEFAULT +8 
 +9: 0+10010470 +0 SECTION LOCAL +DEFAULT +9 
 +10: 0+10010470 +0 SECTION LOCAL +DEFAULT +10 
 +11: 0+100104a8 +0 SECTION LOCAL +DEFAULT +11 
 +12: 0+100104a8 +0 SECTION LOCAL +DEFAULT +12 
 +13: 0+100105f8 +0 SECTION LOCAL +DEFAULT +13 
 +14: 0+100105f8 +0 SECTION LOCAL +DEFAULT +14 
 +15: 0+100105f8 +0 SECTION LOCAL +DEFAULT +15 
 +16: 0+10010628 +0 SECTION LOCAL +DEFAULT +16 
 +17: 0+10010628 +0 SECTION LOCAL +DEFAULT +17 
 +18: 0+10010658 +0 SECTION LOCAL +DEFAULT +18 
 +19: 0+ +0 SECTION LOCAL +DEFAULT +19 
 +20: 0+ +0 SECTION LOCAL +DEFAULT +20 
 +21: 0+ +0 SECTION LOCAL +DEFAULT +21 
 +22: 0+ +0 TLS +LOCAL +DEFAULT +10 gd4
 +23: 0+8 +0 TLS +LOCAL +DEFAULT +10 ld4
 +24: 0+10 +0 TLS +LOCAL +DEFAULT +10 ld5
 +25: 0+18 +0 TLS +LOCAL +DEFAULT +10 ld6
 +26: 0+20 +0 TLS +LOCAL +DEFAULT +10 ie4
 +27: 0+28 +0 TLS +LOCAL +DEFAULT +10 le4
 +28: 0+30 +0 TLS +LOCAL +DEFAULT +10 le5
 +29: 0+100104a8 +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +30: 0+ +0 TLS +GLOBAL DEFAULT +UND gd
 +31: 0+60 +0 TLS +GLOBAL DEFAULT +11 le0
 +32: 0+ +0 NOTYPE +GLOBAL DEFAULT +UND __tls_get_addr
 +33: 0+ +0 FUNC +GLOBAL DEFAULT +UND \.__tls_get_addr
 +34: 0+40 +0 TLS +GLOBAL DEFAULT +11 ld0
 +35: 0+68 +0 TLS +GLOBAL DEFAULT +11 le1
 +36: 0+ +0 TLS +GLOBAL DEFAULT +UND ld
 +37: 0+100003ac +0 NOTYPE +GLOBAL DEFAULT +7 _start
 +38: 0+10010658 +0 NOTYPE +GLOBAL DEFAULT +ABS __end
 +39: 0+50 +0 TLS +GLOBAL DEFAULT +11 ld2
 +40: 0+48 +0 TLS +GLOBAL DEFAULT +11 ld1
 +41: 0+10010628 +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +42: 0+10010628 +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +43: 0+10010658 +0 NOTYPE +GLOBAL DEFAULT +ABS _end
 +44: 0+38 +0 TLS +GLOBAL DEFAULT +11 gd0
 +45: 0+58 +0 TLS +GLOBAL DEFAULT +11 ie0
