#objdump : -s -j .data
#name : .sleb128 tests

.*: .*

Contents of section .data:
#
# 0x76543210   :           000_0111 0110_010 1_0100_00 11_0010_0 001_0000
# 0x80000000   :           000_1000 0000_000 0_0000_00 00_0000_0 000_0000
# 0x87654321   :           000_1000 0111_011 0_0101_01 00_0011_0 010_0001
# 0xffffffff   :           ..................................... 111_1111
#
 0000 90e4d0b2 07808080 8008a186 95bb08ff .*
#
# 0xffffffff   :           000_1111 1111_111 1_1111_11 11_1111_1 ........
# -0x76543210  :           111_1000 1001_101 0_1011_11 00_1101_1 111_0000
# -0x80000000  :           111_1000 0000_000 0_0000_00 00_0000_0 000_0000
# -0x87654321  :           ........................... 11_1100_1 101_1111
#
 00.. ffffff0f f09bafcd 78808080 8078dff9 .*
#
# -0x87654321  :           111_0111 1000_100 1_1010_10 ..................
# -0xffffffff  :           111_0000 0000_000 0_0000_00 00_0000_0 000_0001
#    789abcdef :           111_1000 1001_101 0_1011_11 00_1101_1 110_1111
# 0x123456     :           ........ 0010_001 1_0100_01 01_0110_0
#
 00.. eac47781 80808070 ef9bafcd f8acd191 .*
#
# 0x123456     :           000_0001 ............................
#    789abcdef :           000_0111 0110_010 1_0100_00 11_0010_0 001_0001
# -0x123456    :           111_1110 1101_110 0_1011_10 01_1001_1
#    fffffffff :           000_0000 0000_000 0_0000_00 00_0000_0 000_0001
# -0x7ff       :                             ......... 00_0000_0
#
 00.. 0191e4d0 b287d3ae ee7e8180 80808080 .*
#
# -0x7ff       :                             1_1000_00 .........
#    000000000 :           000_0000 0000_000 0_0000_00 00_0000_0 000_0000
# -0x800       :                             1_1000_00 00_0000_0
#    fffffffff :           000_0000 0000_000 0_0000_00 00_0000_0 000_0001
# -0x7ffffff   : .................. 0000_000 0_0000_00 00_0000_0
#
 00.. 60808080 80808060 81808080 80808080 .*
#
# -0x7ffffff   : 11_1111_1 000_0000 ............................
#    000000000 :           000_0000 0000_000 0_0000_00 00_0000_0 000_0000
# -0x8000000   : 11_1111_1 000_0000 0000_000 0_0000_00 00_0000_0
# -0x100000000 :           ........ 0000_000 0_0000_00 00_0000_0 000_0000
#
 00.. 807f8080 80808080 8080807f 80808080 .*
#
# -0x100000000 :           111_0000 .....................................
#    000000000 :           000_0000 0000_000 0_0000_00 00_0000_0 000_0000
# -0x1000      :                             1_0000_00 00_0000_0
#
 00.. 70808080 80808040 00000000 00000000 .*
#pass
