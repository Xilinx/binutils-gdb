	.section ".text"
start:
	add.	4,5,6
	add	4,5,6
	addc.	4,5,6
	addc	4,5,6
	addco.	4,5,6
	addco	4,5,6
	adde.	4,5,6
	adde	4,5,6
	addeo.	4,5,6
	addeo	4,5,6
	addi	4,5,13
	addi	4,5,-13
	addic.	4,5,13
	addic.	4,5,-13
	addic	4,5,13
	addic	4,5,-13
	addis	4,5,23
	addis	4,5,-23
	addme.	4,5
	addme	4,5
	addmeo.	4,5
	addmeo	4,5
	addo.	4,5,6
	addo	4,5,6
	addze.	4,5
	addze	4,5
	addzeo.	4,5
	addzeo	4,5
	and.	4,5,6
	and	4,5,6
	andc.	4,5,6
	andc	4,5,6
	andi.	4,5,6
	andis.	4,5,6
	attn
	ba      label_abs
	bc      0,1,foo
	bc-     0,1,foo
	bc+     0,1,foo
	bca     4,5,foo_abs
	bca-    4,5,foo_abs
	bca+    4,5,foo_abs
	bcctr   4,6,1
	bcctr-  4,6
	bcctr+  4,6
	bcctrl  4,6,1
	bcctrl- 4,6
	bcctrl+ 4,6
	bcl     0,1,foo
	bcl-    0,1,foo
	bcl+    0,1,foo
	bcla    4,5,foo_abs
	bcla-   4,5,foo_abs
	bcla+   4,5,foo_abs
	bclr    4,6,1
	bclr-   4,6
	bclr+   4,6
	bclrl   4,6,1
	bclrl-  4,6
	bclrl+  4,6
	b       label
	bla     label_abs
	bl      label
	bpermd	10,11,12
	cmp	1,1,7,8
	cmpb	10,11,12
	cmpi	1,1,10,13
	cmpi	1,1,10,-13
	cmpl	1,1,7,8
	cmpli	1,1,10,100
	cntlzd.	20,21
	cntlzd	20,21
	cntlzw.	20,21
	cntlzw	20,21
	crand	1,2,3
	crandc	1,2,3
	creqv	1,2,3
	crnand	1,2,3
	crnor	1,2,3
	cror	1,2,3
	crorc	1,2,3
	crxor	1,2,3
	dcba	10,11
	dcbf	10,11,0
	dcbf	10,11,1
	dcbfep	10,11
	dcbi	10,11
	dcblc	0,10,11
	dcblc	1,10,11
	dcbst	10,11
	dcbstep	10,11
	dcbt	10,11,0
	dcbt	10,11,1
	dcbtep	10,11,12
	dcbtls	0,10,11
	dcbtls	1,10,11
	dcbtst	10,11,0
	dcbtst	10,11,1
	dcbtstep 10,11,12
	dcbtstls 0,10,11
	dcbtstls 1,10,11
	dcbz	10,11
	dcbzep	10,11
	dci
	dci	10
	divd.	20,21,22
	divd	20,21,22
	divdo.	20,21,22
	divdo	20,21,22
	divdu.	20,21,22
	divdu	20,21,22
	divduo.	20,21,22
	divduo	20,21,22
	divw.	20,21,22
	divw	20,21,22
	divwo.	20,21,22
	divwo	20,21,22
	divwu.	20,21,22
	divwu	20,21,22
	divwuo.	20,21,22
	divwuo	20,21,22
	eqv.	20,21,22
	eqv	20,21,22
	eratilx	0,10,11
	eratilx	1,10,11
	eratilx	7,10,11
	erativax 10,11,12
	eratre	10,11,0
	eratre	10,11,3
	eratsx.	10,11,12
	eratsx	10,11,12
	eratwe	10,11,0
	eratwe	10,11,3
	extsb.	10,11
	extsb	10,11
	extsh.	10,11
	extsh	10,11
	extsw.	10,11
	extsw	10,11
	fabs.	20,21
	fabs	20,21
	fadd.	20,21,22
	fadd	20,21,22
	fadds.	20,21,22
	fadds	20,21,22
	fcfid.	20,21
	fcfid	20,21
	fcmpo	0,20,21
	fcmpo	1,20,21
	fcmpu	0,20,21
	fcmpu	1,20,21
	fcpsgn.	20,21,22
	fcpsgn	20,21,22
	fctid.	20,21
	fctid	20,21
	fctidz.	20,21
	fctidz	20,21
	fctiw.	20,21
	fctiw	20,21
	fctiwz.	20,21
	fctiwz	20,21
	fdiv.	20,21,22
	fdiv	20,21,22
	fdivs.	20,21,22
	fdivs	20,21,22
	fmadd.	20,21,22,23
	fmadd	20,21,22,23
	fmadds.	20,21,22,23
	fmadds	20,21,22,23
	fmr.	20,21
	fmr	20,21
	fmsub.	20,21,22,23
	fmsub	20,21,22,23
	fmsubs.	20,21,22,23
	fmsubs	20,21,22,23
	fmul.	20,21,22
	fmul	20,21,22
	fmuls.	20,21,22
	fmuls	20,21,22
	fnabs.	20,21
	fnabs	20,21
	fneg.	20,21
	fneg	20,21
	fnmadd.	20,21,22,23
	fnmadd	20,21,22,23
	fnmadds. 20,21,22,23
	fnmadds	20,21,22,23
	fnmsub.	20,21,22,23
	fnmsub	20,21,22,23
	fnmsubs. 20,21,22,23
	fnmsubs	20,21,22,23
	fre.	20,21
	fre	20,21
	fre.	20,21,0
	fre	20,21,0
	fre.	20,21,1
	fre	20,21,1
	fres.	20,21
	fres	20,21
	fres.	20,21,0
	fres	20,21,0
	fres.	20,21,1
	fres	20,21,1
	frim.	20,21
	frim	20,21
	frin.	20,21
	frin	20,21
	frip.	20,21
	frip	20,21
	friz.	20,21
	friz	20,21
	frsp.	20,21
	frsp	20,21
	frsqrte. 20,21
	frsqrte	20,21
	frsqrte. 20,21,0
	frsqrte	20,21,0
	frsqrte. 20,21,1
	frsqrte	20,21,1
	frsqrtes 20,21
	frsqrtes. 20,21
	frsqrtes 20,21,0
	frsqrtes. 20,21,0
	frsqrtes 20,21,1
	frsqrtes. 20,21,1
	fsel.	20,21,22,23
	fsel	20,21,22,23
	fsqrt.	20,21
	fsqrt	20,21
	fsqrts.	20,21
	fsqrts	20,21
	fsub.	20,21,22
	fsub	20,21,22
	fsubs.	20,21,22
	fsubs	20,21,22
	icbi	10,11
	icbiep	10,11
	icbt	0,10,11
	icbt	7,10,11
	icbtls	0,10,11
	icbtls	7,10,11
	ici	10
	icswx.	10,11,12
	icswx	10,11,12
	isel	10,11,12,23
	isync
	lbepx	10,11,12
	lbz	10,-17(11)
	lbz	10,17(11)
	lbzu	10,-1(11)
	lbzu	10,1(11)
	lbzux	10,11,13
	lbzx	10,11,13
	ld	10,-8(11)
	ld	10,8(11)
	ldarx	10,11,12,0
	ldarx	10,11,12,1
	ldbrx	10,11,12
	ldepx	10,11,12
	ldu	10,-8(11)
	ldu	10,8(11)
	ldux	10,11,12
	ldx	10,11,12
	lfd	20,-8(10)
	lfd	20,8(10)
	lfdepx	20,10,11
	lfdu	20,-8(10)
	lfdu	20,8(10)
	lfdux	20,10,11
	lfdx	20,10,11
	lfiwax	20,10,11
	lfiwzx	20,10,11
	lfs	20,-4(10)
	lfs	20,4(10)
	lfsu	20,-4(10)
	lfsu	20,4(10)
	lfsux	20,10,11
	lfsx	20,10,11
	lha	10,2(11)
	lhau	10,-2(11)
	lhaux	10,11,12
	lhax	10,11,12
	lhbrx	10,11,12
	lhepx	10,11,12
	lhz	10,-2(11)
	lhz	10,2(11)
	lhzu	10,-2(11)
	lhzu	10,2(11)
	lhzux	10,11,12
	lhzx	10,11,12
	lmw	20,16(10)
	lswi	10,11,1
	lswi	10,11,32
	lswx	10,11,12
	lwa	10,-4(11)
	lwa	10,4(11)
	lwarx	10,11,12,0
	lwarx	10,11,12,1
	lwaux	10,11,12
	lwax	10,11,12
	lwbrx	10,11,12
	lwepx	10,11,12
	lwz	10,-4(11)
	lwz	10,4(11)
	lwzu	10,-4(11)
	lwzu	10,4(11)
	lwzux	10,11,12
	lwzx	10,11,12
	mbar
	mbar	0
	eieio
	mbar	1
	mcrf	0,1
	mcrfs	3,4
	mcrxr	0
	mcrxr	3
	mfcr	3
	mfcr	3,0
	mfcr	3,0x01
	mfcr	3,0x80
	mfdcr.	10,234
	mfdcr	10,234
	mfdcrx.	10,11
	mfdcrx	10,11
	mffs.	20
	mffs	20
	mfmsr	10
	mfocrf	3,0x01
	mfocrf	3,0x80
	mfspr	10,234
	mftb	10,268
	mftb	10,269
	msgclr	10
	msgsnd	10
	mtcrf	0x00,3
	mtcrf	0x01,3
	mtcrf	0x80,3
	mtcrf	0xff,3
	mtdcr.	234,10
	mtdcr	234,10
	mtdcrx.	10,11
	mtdcrx	10,11
	mtfsb0.	3
	mtfsb0	3
	mtfsb1.	3
	mtfsb1	3
	mtfsf.	6,20
	mtfsf	6,20
	mtfsf.	6,20,0,0
	mtfsf	6,20,0,0
	mtfsf.	6,20,1,1
	mtfsf	6,20,1,1
	mtfsfi.	6,0
	mtfsfi	6,0
	mtfsfi.	6,13,0
	mtfsfi	6,13,0
	mtfsfi.	6,13,1
	mtfsfi	6,13,1
	mtmsr	10
	mtmsr	10,0
	mtmsr	10,1
	mtocrf	0x01,3
	mtocrf	0x80,3
	mtspr	234,10
	mulhd.	20,21,22
	mulhd	20,21,22
	mulhdu.	20,21,22
	mulhdu	20,21,22
	mulhw.	20,21,22
	mulhw	20,21,22
	mulhwu.	20,21,22
	mulhwu	20,21,22
	mulld.	20,21,22
	mulld	20,21,22
	mulldo.	20,21,22
	mulldo	20,21,22
	mulli	20,21,100
	mulli	20,21,-100
	mullw.	20,21,22
	mullw	20,21,22
	mullwo.	20,21,22
	mullwo	20,21,22
	nand.	20,21,22
	nand	20,21,22
	neg.	20,21
	neg	20,21
	nego.	20,21
	nego	20,21
	nor.	20,21,22
	nor	20,21,22
	or.	20,21,22
	or	20,21,22
	orc.	20,21,22
	orc	20,21,22
	ori	20,21,0x1000
	oris	20,21,0x1000
	popcntb	10,11
	popcntd	10,11
	popcntw	10,11
	prtyd	10,11
	prtyw	10,11
	rfci
	rfgi
	rfi
	rfmci
	rldcl.	10,11,12,0x3f
	rldcl	10,11,12,0x3f
	rldcr.	10,11,12,0x3f
	rldcr	10,11,12,0x3f
	rldic.	10,11,23,0x3f
	rldic	10,11,23,0x3f
	rldicl.	10,11,23,0x3f
	rldicl	10,11,23,0x3f
	rldicr.	10,11,23,0x3f
	rldicr	10,11,23,0x3f
	rldimi.	10,11,23,0x3f
	rldimi	10,11,23,0x3f
	rlwimi.	10,11,23,0,31
	rlwimi	10,11,23,0,31
	rlwinm.	10,11,23,0,31
	rlwinm	10,11,23,0,31
	rlwnm.	10,11,23,0,31
	rlwnm	10,11,23,0,31
	sc	
	sc	100
	sld.	10,11,12
	sld	10,11,12
	slw.	10,11,12
	slw	10,11,12
	srad.	10,11,12
	srad	10,11,12
	sradi.	10,11,0x3f
	sradi	10,11,0x3f
	sraw.	10,11,12
	sraw	10,11,12
	srawi.	10,11,31
	srawi	10,11,31
	srd.	10,11,12
	srd	10,11,12
	srw.	10,11,12
	srw	10,11,12
	stb	10,-1(11)
	stb	10,1(11)
	stbepx	10,11,12
	stbu	10,-1(11)
	stbu	10,1(11)
	stbux	10,11,12
	stbx	10,11,12
	std	10,-8(11)
	std	10,8(11)
	stdbrx	10,11,12
	stdcx.	10,11,12
	stdepx	10,11,12
	stdu	10,-8(11)
	stdu	10,8(11)
	stdux	10,11,12
	stdx	10,11,12
	stfd	20,-8(10)
	stfd	20,8(10)
	stfdepx	20,10,11
	stfdu	20,-8(10)
	stfdu	20,8(10)
	stfdux	20,10,11
	stfdx	20,10,11
	stfiwx	20,10,11
	stfs	20,-4(10)
	stfs	20,4(10)
	stfsu	20,-4(10)
	stfsu	20,4(10)
	stfsux	20,10,11
	stfsx	20,10,11
	sth	10,-2(11)
	sth	10,2(11)
	sth	10,-4(11)
	sth	10,4(11)
	sthbrx	10,11,12
	sthepx	10,11,12
	sthu	10,-2(11)
	sthu	10,2(11)
	sthux	10,11,12
	sthx	10,11,12
	stmw	20,16(10)
	stswi	10,11,1
	stswi	10,11,32
	stswx	10,11,12
	stwbrx	10,11,12
	stwcx.	10,11,12
	stwepx	10,11,12
	stwu	10,-4(11)
	stwu	10,4(11)
	stwux	10,11,12
	stwx	10,11,12
	subf.	20,21,22
	subf	20,21,22
	subfc.	20,21,22
	subfc	20,21,22
	subfco.	20,21,22
	subfco	20,21,22
	subfe.	20,21,22
	subfe	20,21,22
	subfeo.	20,21,22
	subfeo	20,21,22
	subfic	20,21,100
	subfic	20,21,-100
	subfme.	20,21
	subfme	20,21
	subfmeo. 20,21
	subfmeo	20,21
	subfo.	20,21,22
	subfo	20,21,22
	subfze.	20,21
	subfze	20,21
	subfzeo. 20,21
	subfzeo	20,21
	sync
	msync
	sync	0
	sync	1
	td	5,10,11
	tdi	5,10,100
	tdi	5,10,-100
	tlbilx	3,10,11
	tlbivax 10,11
	tlbre
	tlbre	10,11,7
	tlbsrx.	10,11
	tlbsx.	10,11,12
	tlbsx	10,11,12
	tlbsync
	tlbwe
	tlbwe	10,11,7
	tw	5,10,11
	twi	5,10,100
	twi	5,10,-100
	wait
	wait	0
	wait	1
	wait	2
	waitimpl
	waitrsv
	wchkall
	wchkall	0
	wchkall	3
	wclr	1,10,11
	wclrall	1
	wclrone	10,11
	wrtee	10
	wrteei	1
	xor.	10,11,12
	xor	10,11,12
	xori	10,11,0x1000
	xoris	10,11,0x1000
