# ld: -T overlay-size.t -Map tmpdir/overlay-size.map
# name: overlay size
# objdump: --headers
#
# The .bss[123] LMAs are deliberately blanked out.  We can't
# reliably map overlaid sections to segments.
#...
 .. \.bss1 +0+010 +0+20000 .*
#...
 .. \.bss2 +0+030 +0+20000 .*
#...
 .. \.bss3 +0+020 +0+20000 .*
#...
 .. \.mtext +0+020 +0+10000 +0+30000 .*
#...
 .. \.mbss +0+230 +0+20030 .*
#...
 .. \.text1 +0+080 +0+10020 +0+30020 .*
#...
 .. \.text2 +0+040 +0+10020 +0+300a0 .*
#...
 .. \.text3 +0+020 +0+10020 +0+300e0 .*
#...
 .. \.data1 +0+030 +0+20260 +0+30100 .*
#...
 .. \.data2 +0+040 +0+20260 +0+30130 .*
#...
 .. \.data3 +0+050 +0+20260 +0+30170 .*
#pass
