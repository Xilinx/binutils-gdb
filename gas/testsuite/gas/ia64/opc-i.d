# objdump: -d
# name: ia64 opc-i

.*: +file format .*

Disassembly of section \.text:

0000000000000000 <_start>:
   0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
   6:	40 28 18 8c 38 80 	            pmpyshr2 r4=r5,r6,0
   c:	50 30 68 71       	            pmpyshr2\.u r4=r5,r6,16
  10:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
  16:	40 28 18 b4 3a 80 	            pmpy2\.r r4=r5,r6
  1c:	50 30 78 75       	            pmpy2\.l r4=r5,r6
  20:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
  26:	40 28 18 20 3a 80 	            mix1\.r r4=r5,r6
  2c:	50 30 40 75       	            mix2\.r r4=r5,r6
  30:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
  36:	40 28 18 20 3e 80 	            mix4\.r r4=r5,r6
  3c:	50 30 50 74       	            mix1\.l r4=r5,r6
  40:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
  46:	40 28 18 a8 3a 80 	            mix2\.l r4=r5,r6
  4c:	50 30 50 7c       	            mix4\.l r4=r5,r6
  50:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
  56:	40 28 18 80 3a 80 	            pack2\.uss r4=r5,r6
  5c:	50 30 10 75       	            pack2\.sss r4=r5,r6
  60:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
  66:	40 28 18 08 3e 80 	            pack4\.sss r4=r5,r6
  6c:	50 30 20 74       	            unpack1\.h r4=r5,r6
  70:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
  76:	40 28 18 90 3a 80 	            unpack2\.h r4=r5,r6
  7c:	50 30 20 7c       	            unpack4\.h r4=r5,r6
  80:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
  86:	40 28 18 18 3a 80 	            unpack1\.l r4=r5,r6
  8c:	50 30 30 75       	            unpack2\.l r4=r5,r6
  90:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
  96:	40 28 18 18 3e 80 	            unpack4\.l r4=r5,r6
  9c:	50 30 08 74       	            pmin1\.u r4=r5,r6
  a0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
  a6:	40 28 18 14 3a 80 	            pmax1\.u r4=r5,r6
  ac:	50 30 18 75       	            pmin2\.u r4=r5,r6
  b0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
  b6:	40 28 18 9c 3a 80 	            pmax2\.u r4=r5,r6
  bc:	50 30 58 74       	            psad1 r4=r5,r6
  c0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
  c6:	40 28 2c 28 3b 80 	            mux1 r4=r5,@rev
  cc:	50 40 50 76       	            mux1 r4=r5,@mix
  d0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
  d6:	40 28 24 28 3b 80 	            mux1 r4=r5,@shuf
  dc:	50 50 50 76       	            mux1 r4=r5,@alt
  e0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
  e6:	40 28 00 28 3b 80 	            mux1 r4=r5,@brcst
  ec:	50 00 50 77       	            mux2 r4=r5,0x0
  f0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
  f6:	40 28 fc ab 3b 80 	            mux2 r4=r5,0xff
  fc:	50 50 55 77       	            mux2 r4=r5,0xaa
 100:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 106:	40 30 14 88 38 80 	            pshr2 r4=r5,r6
 10c:	00 28 18 73       	            pshr2 r4=r5,0
 110:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 116:	40 80 14 8c 39 80 	            pshr2 r4=r5,8
 11c:	e0 2b 18 73       	            pshr2 r4=r5,31
 120:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 126:	40 30 14 08 3c 80 	            pshr4 r4=r5,r6
 12c:	00 28 18 7a       	            pshr4 r4=r5,0
 130:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 136:	40 80 14 0c 3d 80 	            pshr4 r4=r5,8
 13c:	e0 2b 18 7a       	            pshr4 r4=r5,31
 140:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 146:	40 30 14 80 38 80 	            pshr2\.u r4=r5,r6
 14c:	00 28 08 73       	            pshr2\.u r4=r5,0
 150:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 156:	40 80 14 84 39 80 	            pshr2\.u r4=r5,8
 15c:	e0 2b 08 73       	            pshr2\.u r4=r5,31
 160:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 166:	40 30 14 00 3c 80 	            pshr4\.u r4=r5,r6
 16c:	00 28 08 7a       	            pshr4\.u r4=r5,0
 170:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 176:	40 80 14 04 3d 80 	            pshr4\.u r4=r5,8
 17c:	e0 2b 08 7a       	            pshr4\.u r4=r5,31
 180:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 186:	40 30 14 88 3c 80 	            shr r4=r5,r6
 18c:	60 28 00 79       	            shr\.u r4=r5,r6
 190:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 196:	40 28 18 90 38 80 	            pshl2 r4=r5,r6
 19c:	50 f8 28 77       	            pshl2 r4=r5,0
 1a0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 1a6:	40 28 5c 94 3b 80 	            pshl2 r4=r5,8
 1ac:	50 00 28 77       	            pshl2 r4=r5,31
 1b0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 1b6:	40 28 18 10 3c 80 	            pshl4 r4=r5,r6
 1bc:	50 f8 28 7e       	            pshl4 r4=r5,0
 1c0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 1c6:	40 28 5c 14 3f 80 	            pshl4 r4=r5,8
 1cc:	50 00 28 7e       	            pshl4 r4=r5,31
 1d0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 1d6:	40 28 18 90 3c 80 	            shl r4=r5,r6
 1dc:	00 28 48 73       	            popcnt r4=r5
 1e0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 1e6:	40 28 18 00 2b 80 	            shrp r4=r5,r6,0
 1ec:	50 30 30 56       	            shrp r4=r5,r6,12
 1f0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 1f6:	40 28 18 7e 2b 80 	            shrp r4=r5,r6,63
 1fc:	10 28 3c 52       	            extr r4=r5,0,16
 200:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 206:	40 08 14 7c 29 80 	            extr r4=r5,0,63
 20c:	50 29 9c 52       	            extr r4=r5,10,40
 210:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 216:	40 00 14 1e 29 80 	            extr\.u r4=r5,0,16
 21c:	00 28 f8 52       	            extr\.u r4=r5,0,63
 220:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 226:	40 a0 14 4e 29 80 	            extr\.u r4=r5,10,40
 22c:	50 f8 3d 53       	            dep\.z r4=r5,0,16
 230:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 236:	40 28 fc fc 29 80 	            dep\.z r4=r5,0,63
 23c:	50 a8 9d 53       	            dep\.z r4=r5,10,40
 240:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 246:	40 00 fc 9f 29 80 	            dep\.z r4=0,0,16
 24c:	f0 ff fb 53       	            dep\.z r4=127,0,63
 250:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 256:	40 00 e8 e3 2d 80 	            dep\.z r4=-128,5,50
 25c:	50 ad 9f 53       	            dep\.z r4=85,10,40
 260:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 266:	40 f0 17 9e 2b 80 	            dep r4=0,r5,0,16
 26c:	e0 2f f8 5f       	            dep r4=-1,r5,0,63
 270:	0c 00 00 00 01 00 	\[MFI\]       nop\.m 0x0
 276:	00 00 00 02 00 80 	            nop\.f 0x0
 27c:	50 30 58 4d       	            dep r4=r5,r6,10,7
 280:	04 00 00 00 01 00 	\[MLI\]       nop\.m 0x0
 286:	00 00 00 00 00 80 	            movl r4=0x0
 28c:	00 00 00 60 
 290:	04 00 00 00 01 c0 	\[MLI\]       nop\.m 0x0
 296:	ff ff ff ff 7f 80 	            movl r4=0xffffffffffffffff
 29c:	f0 f7 ff 6f 
 2a0:	04 00 00 00 01 80 	\[MLI\]       nop\.m 0x0
 2a6:	90 78 56 34 12 80 	            movl r4=0x1234567890abcdef
 2ac:	f0 76 6d 66 
 2b0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 2b6:	00 00 00 00 00 e0 	            break\.i 0x0
 2bc:	ff ff 01 08       	            break\.i 0x1fffff
 2c0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 2c6:	00 00 00 02 00 e0 	            nop\.i 0x0
 2cc:	ff ff 05 08       	            nop\.i 0x1fffff
 2d0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 2d6:	30 25 fc ff 04 80 	            chk\.s\.i r4,0 <_start>
 2dc:	00 00 c4 00       	            mov r4=b0
 2e0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 2e6:	00 20 04 80 03 00 	            mov b0=r4
 2ec:	40 00 00 03       	            mov pr=r4,0x0
 2f0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 2f6:	a0 21 80 84 01 e0 	            mov pr=r4,0x1234
 2fc:	4f 80 7f 0b       	            mov pr=r4,0xfffffffffffffffe
 300:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 306:	00 00 00 00 01 e0 	            mov pr\.rot=0x0
 30c:	7f 00 00 02       	            mov pr\.rot=0x3ff0000
 310:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 316:	00 c0 ff 7f 05 80 	            mov pr\.rot=0xfffffffffc000000
 31c:	00 28 40 00       	            zxt1 r4=r5
 320:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 326:	40 00 14 22 00 80 	            zxt2 r4=r5
 32c:	00 28 48 00       	            zxt4 r4=r5
 330:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 336:	40 00 14 28 00 80 	            sxt1 r4=r5
 33c:	00 28 54 00       	            sxt2 r4=r5
 340:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 346:	40 00 14 2c 00 80 	            sxt4 r4=r5
 34c:	00 28 60 00       	            czx1\.l r4=r5
 350:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 356:	40 00 14 32 00 80 	            czx2\.l r4=r5
 35c:	00 28 70 00       	            czx1\.r r4=r5
 360:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 366:	40 00 14 3a 00 40 	            czx2\.r r4=r5
 36c:	00 20 0c 50       	            tbit\.z p2,p3=r4,0
 370:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 376:	20 14 10 06 28 40 	            tbit\.z\.unc p2,p3=r4,1
 37c:	40 20 0c 58       	            tbit\.z\.and p2,p3=r4,2
 380:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 386:	20 30 10 86 28 40 	            tbit\.z\.or p2,p3=r4,3
 38c:	80 20 0c 59       	            tbit\.z\.or\.andcm p2,p3=r4,4
 390:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 396:	20 54 10 86 28 40 	            tbit\.nz\.or p2,p3=r4,5
 39c:	c8 20 0c 58       	            tbit\.nz\.and p2,p3=r4,6
 3a0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 3a6:	30 74 10 84 2c 60 	            tbit\.nz\.or\.andcm p3,p2=r4,7
 3ac:	00 21 08 50       	            tbit\.z p3,p2=r4,8
 3b0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 3b6:	30 94 10 04 28 40 	            tbit\.z\.unc p3,p2=r4,9
 3bc:	48 21 0c 58       	            tbit\.nz\.and p2,p3=r4,10
 3c0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 3c6:	20 b4 10 86 28 40 	            tbit\.nz\.or p2,p3=r4,11
 3cc:	88 21 0c 59       	            tbit\.nz\.or\.andcm p2,p3=r4,12
 3d0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 3d6:	20 d0 10 86 28 40 	            tbit\.z\.or p2,p3=r4,13
 3dc:	c0 21 0c 58       	            tbit\.z\.and p2,p3=r4,14
 3e0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 3e6:	30 f0 10 84 2c 40 	            tbit\.z\.or\.andcm p3,p2=r4,15
 3ec:	10 20 0c 50       	            tnat\.z p2,p3=r4
 3f0:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 3f6:	20 0c 10 06 28 40 	            tnat\.z\.unc p2,p3=r4
 3fc:	10 20 0c 58       	            tnat\.z\.and p2,p3=r4
 400:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 406:	20 08 10 86 28 40 	            tnat\.z\.or p2,p3=r4
 40c:	10 20 0c 59       	            tnat\.z\.or\.andcm p2,p3=r4
 410:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 416:	20 0c 10 86 28 40 	            tnat\.nz\.or p2,p3=r4
 41c:	18 20 0c 58       	            tnat\.nz\.and p2,p3=r4
 420:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 426:	30 0c 10 84 2c 60 	            tnat\.nz\.or\.andcm p3,p2=r4
 42c:	10 20 08 50       	            tnat\.z p3,p2=r4
 430:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 436:	30 0c 10 04 28 40 	            tnat\.z\.unc p3,p2=r4
 43c:	18 20 0c 58       	            tnat\.nz\.and p2,p3=r4
 440:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 446:	20 0c 10 86 28 40 	            tnat\.nz\.or p2,p3=r4
 44c:	18 20 0c 59       	            tnat\.nz\.or\.andcm p2,p3=r4
 450:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 456:	20 08 10 86 28 40 	            tnat\.z\.or p2,p3=r4
 45c:	10 20 0c 58       	            tnat\.z\.and p2,p3=r4
 460:	00 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 466:	30 08 10 84 2c 60 	            tnat\.z\.or\.andcm p3,p2=r4
 46c:	40 88 04 07       	            mov b3=r4
 470:	01 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 476:	30 20 24 84 03 00 	            mov\.imp b3=r4,570 <_start\+0x570>
 47c:	00 00 04 00       	            nop\.i 0x0;;
	\.\.\.
 570:	01 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 576:	30 20 00 84 03 60 	            mov\.sptk b3=r4,670 <_start\+0x670>
 57c:	40 40 08 07       	            mov\.sptk\.imp b3=r4,670 <_start\+0x670>;;
	\.\.\.
 670:	01 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 676:	30 20 08 84 03 60 	            mov\.dptk b3=r4,770 <_start\+0x770>
 67c:	40 50 08 07       	            mov\.dptk\.imp b3=r4,770 <_start\+0x770>;;
	\.\.\.
 770:	01 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 776:	30 20 14 84 03 60 	            mov\.ret b3=r4,870 <_start\+0x870>
 77c:	40 68 08 07       	            mov\.ret\.imp b3=r4,870 <_start\+0x870>;;
	\.\.\.
 870:	01 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 876:	30 20 10 84 03 60 	            mov\.ret\.sptk b3=r4,970 <_start\+0x970>
 87c:	40 60 08 07       	            mov\.ret\.sptk\.imp b3=r4,970 <_start\+0x970>;;
	\.\.\.
 970:	01 00 00 00 01 00 	\[MII\]       nop\.m 0x0
 976:	30 20 18 84 03 60 	            mov\.ret\.dptk b3=r4,a70 <_start\+0xa70>
 97c:	40 70 08 07       	            mov\.ret\.dptk\.imp b3=r4,a70 <_start\+0xa70>;;
	\.\.\.
