#as: -m40 --defsym TEST_ALL=1 --defsym TEST_C4X=1
#objdump: -d -z
#name: c4x registers
#source: registers.s

.*: +file format .*c4x.*

Disassembly of section .text:

00000000 <start>:
   0:	08000000.*
   1:	08010000.*
   2:	08020000.*
   3:	08030000.*
   4:	08040000.*
   5:	08050000.*
   6:	08060000.*
   7:	08070000.*
   8:	08080000.*
   9:	08090000.*
   a:	080a0000.*
   b:	080b0000.*
   c:	080c0000.*
   d:	080d0000.*
   e:	080e0000.*
   f:	080f0000.*
  10:	08100000.*
  11:	08110000.*
  12:	08120000.*
  13:	08130000.*
  14:	08140000.*
  15:	08150000.*
  16:	08160000.*
  17:	08170000.*
  18:	08180000.*
  19:	08190000.*
  1a:	081a0000.*
  1b:	081b0000.*
  1c:	081c0000.*
  1d:	081d0000.*
  1e:	081e0000.*
  1f:	081f0000.*
  20:	76800000.*
  21:	76810000.*
  22:	07000000.*
  23:	07010000.*
  24:	07020000.*
  25:	07030000.*
  26:	07040000.*
  27:	07050000.*
  28:	07060000.*
  29:	07070000.*
  2a:	071c0000.*
  2b:	071d0000.*
  2c:	071e0000.*
  2d:	071f0000.*
