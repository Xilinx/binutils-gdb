# ld: -T overlay-size.t -Map tmpdir/overlay-size.map
# name: overlay size
# objdump: --headers
#
# The .bss[123] LMAs are deliberately blanked out.  We can't
# reliably map overlaid sections to segments.
#...
  0 \.bss1 +0+010 +0+20000 .*
#...
  1 \.bss2 +0+030 +0+20000 .*
#...
  2 \.bss3 +0+020 +0+20000 .*
#...
  3 \.mtext +0+020 +0+10000 +0+30000 .*
#...
  4 \.mbss +0+230 +0+20030 +0+20060 .*
#...
  5 \.text1 +0+080 +0+10020 +0+30020 .*
#...
  6 \.text2 +0+040 +0+10020 +0+300a0 .*
#...
  7 \.text3 +0+020 +0+10020 +0+300e0 .*
#...
  8 \.data1 +0+030 +0+20260 +0+30100 .*
#...
  9 \.data2 +0+040 +0+20260 +0+30130 .*
#...
 10 \.data3 +0+050 +0+20260 +0+30170 .*
#pass
