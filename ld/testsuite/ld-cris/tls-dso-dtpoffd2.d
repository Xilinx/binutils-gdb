#source: start1.s
#source: tls-dtpoffdx.s
#source: tls-gd-1.s
#source: tls128.s
#source: tls-x.s
#as: --no-underscore --em=criself --pic -I$srcdir/$subdir
#ld: -m crislinux --shared
#objdump: -d -s -t -r -p

# Linking a file with a DTPOFF from a .dtpoffd of a GD variable, PIC
# (globally defined here but overridable).  Making sure it links and
# that the offset is correct, locally.

.*:     file format elf32-cris

Program Header:
#...
     TLS off   .*
         filesz 0x0+84 memsz 0x0+84 flags r--
#...
SYMBOL TABLE:
#...
0+80 g       .tdata	0+4 x
#...
Contents of section .rela.dyn:
 01a4 b8220000 17030000 00000000           .*
Contents of section .text:
 01b0 41b20000 5fae0c00                    .*
#...
Contents of section .got:
 22ac 3c220000 00000000 00000000 00000000  .*
 22bc 00000000                             .*
Contents of section .debug_info:
 0000 80000000                             .*
#...
Disassembly of section .text:
#pass
