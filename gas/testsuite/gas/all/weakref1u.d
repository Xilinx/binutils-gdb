#nm: --undefined-only --no-sort
#name: weakref tests, strong undefined syms
#source: weakref1.s
# aout turns undefined into *ABS* symbols.
# see weakref1.d for comments on the other not-targets
#not-target: *-*-aout ns32k-*-netbsd alpha*-*-osf* *-*-ecoff pdp11-*-aout

# the rest of this file is generated with the following script:
# # script begin
# sed -n 's:^[ 	]*\.weakref .*, \(u.*\)$:.* U \1:p' weakref1.s | uniq | while read line; do echo "#..."; echo "$line"; done
# echo \#pass
# # script output:
#...
.* U ua2
#...
.* U ua3
#...
.* U ua4
#...
.* U ub2
#...
.* U ub3
#...
.* U ub4
#...
.* U uc2
#...
.* U uc3
#...
.* U uc4
#...
.* U uc5
#...
.* U uc6
#...
.* U uc7
#...
.* U uc8
#...
.* U uc9
#...
.* U um5
#...
.* U uh8
#...
.* U uh9
#...
.* U ud5
#pass
