#source: sec-1.s
#source: start.s
#ld: -m elf64mmix
#objcopy_linked_file: -O mmo
#objdump: -sh

.*:     file format mmo

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 \.text         0+4  0+  0+  0+  2\*\*2
                  CONTENTS, ALLOC, LOAD, CODE
  1 \.a\.fourth\.section 0+10  0+20  0+20  0+  2\*\*2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  2 secname       0+1c  0+4  0+4  0+  2\*\*2
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  3 anothersec    0+14  2000000000000000  2000000000000000  0+  2\*\*2
                  CONTENTS, ALLOC, LOAD, DATA
  4 thirdsec      0+c  0+  0+  0+  2\*\*2
                  CONTENTS, READONLY
Contents of section \.text:
 0000 e3fd0001                             .*
Contents of section \.a\.fourth\.section:
 0020 00000000 0087a238 00000000 302a55a8  .*
Contents of section secname:
 0004 00000001 00000002 00000003 00000004  .*
 0014 ffffffff fffff827 50000000           .*
Contents of section anothersec:
 0000 0000000a 00000009 00000008 00000007  .*
 0010 25272900                             .*
Contents of section thirdsec:
 0000 00030d41 000186a2 26280000           .*
