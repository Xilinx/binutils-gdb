#source: seg.s
#ld: -T seg.t -z max-page-size=0x1000
#readelf: -l --wide

#...
Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           .*
  LOAD           0x0*001000 0xf*fffff000 0xf*fffff000 0x01000 0x01000 .*

 Section to Segment mapping:
  Segment Sections...
   00     .*
   01     reset boot 
