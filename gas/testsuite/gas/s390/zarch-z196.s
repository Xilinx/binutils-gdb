.text
foo:
	ahhhr	%r6,%r7,%r8
	ahhlr	%r6,%r7,%r8
	aih	%r6,-65000
	alhhhr	%r6,%r7,%r8
	alhhlr	%r6,%r7,%r8
	alsih	%r6,65000
	alsihn	%r6,65000
	brcth	%r6,.
	chhr	%r6,%r7
	chlr	%r6,%r7
	chf	%r6,5555(%r7,%r8)
	cih	%r6,65000
	clhhr	%r6,%r7
	clhlr	%r6,%r7
	clhf	%r6,5555(%r7,%r8)
	clih	%r6,650000
	lbh	%r6,-5555(%r7,%r8)
	lhh	%r6,-5555(%r7,%r8)
	lfh	%r6,-5555(%r7,%r8)
	llch	%r6,-5555(%r7,%r8)
	llhh	%r6,-5555(%r7,%r8)
	risbhg	%r6,%r7,12,13,14
	risblg	%r6,%r7,12,13,14
	stch	%r6,-5555(%r7,%r8)
	sthh	%r6,-5555(%r7,%r8)
	stfh	%r6,-5555(%r7,%r8)
	shhhr	%r6,%r7,%r8
	shhlr	%r6,%r7,%r8
	slhhhr	%r6,%r7,%r8
	slhhlr	%r6,%r7,%r8
	laa	%r6,%r7,-5555(%r8)
	laag	%r6,%r7,-5555(%r8)
	laal	%r6,%r7,-5555(%r8)
	laalg	%r6,%r7,-5555(%r8)
	lan	%r6,%r7,-5555(%r8)
	lang	%r6,%r7,-5555(%r8)
	lax	%r6,%r7,-5555(%r8)
	laxg	%r6,%r7,-5555(%r8)
	lao	%r6,%r7,-5555(%r8)
	laog	%r6,%r7,-5555(%r8)
	lpd	%r6,2222(%r7),1111(%r8)
	lpdg	%r6,2222(%r7),1111(%r8)

	locro	%r6,%r7
	locrh	%r6,%r7
	locrp	%r6,%r7
	locrnle	%r6,%r7
	locrl	%r6,%r7
	locrm	%r6,%r7
	locrnhe	%r6,%r7
	locrlh	%r6,%r7
	locrne	%r6,%r7
	locrnz	%r6,%r7
	locre	%r6,%r7
	locrz	%r6,%r7
	locrnlh	%r6,%r7
	locrhe	%r6,%r7
	locrnl	%r6,%r7
	locrnm	%r6,%r7
	locrle	%r6,%r7
	locrnh	%r6,%r7
	locrnp	%r6,%r7
	locrno	%r6,%r7
	locr	%r6,%r7,8

	locgro	%r6,%r7
	locgrh	%r6,%r7
	locgrp	%r6,%r7
	locgrnle %r6,%r7
	locgrl	%r6,%r7
	locgrm	%r6,%r7
	locgrnhe %r6,%r7
	locgrlh	%r6,%r7
	locgrne	%r6,%r7
	locgrnz	%r6,%r7
	locgre	%r6,%r7
	locgrz	%r6,%r7
	locgrnlh %r6,%r7
	locgrhe	%r6,%r7
	locgrnl	%r6,%r7
	locgrnm	%r6,%r7
	locgrle	%r6,%r7
	locgrnh	%r6,%r7
	locgrnp	%r6,%r7
	locgrno	%r6,%r7
	locgr	%r6,%r7,8

	loco	%r6,-5555(%r7)
	loch	%r6,-5555(%r7)
	locp	%r6,-5555(%r7)
	locnle	%r6,-5555(%r7)
	locl	%r6,-5555(%r7)
	locm	%r6,-5555(%r7)
	locnhe	%r6,-5555(%r7)
	loclh	%r6,-5555(%r7)
	locne	%r6,-5555(%r7)
	locnz	%r6,-5555(%r7)
	loce	%r6,-5555(%r7)
	locz	%r6,-5555(%r7)
	locnlh	%r6,-5555(%r7)
	loche	%r6,-5555(%r7)
	locnl	%r6,-5555(%r7)
	locnm	%r6,-5555(%r7)
	locle	%r6,-5555(%r7)
	locnh	%r6,-5555(%r7)
	locnp	%r6,-5555(%r7)
	locno	%r6,-5555(%r7)
	loc	%r6,-5555(%r7),8

	lgoco	%r6,-5555(%r7)
	lgoch	%r6,-5555(%r7)
	lgocp	%r6,-5555(%r7)
	lgocnle	%r6,-5555(%r7)
	lgocl	%r6,-5555(%r7)
	lgocm	%r6,-5555(%r7)
	lgocnhe	%r6,-5555(%r7)
	lgoclh	%r6,-5555(%r7)
	lgocne	%r6,-5555(%r7)
	lgocnz	%r6,-5555(%r7)
	lgoce	%r6,-5555(%r7)
	lgocz	%r6,-5555(%r7)
	lgocnlh	%r6,-5555(%r7)
	lgoche	%r6,-5555(%r7)
	lgocnl	%r6,-5555(%r7)
	lgocnm	%r6,-5555(%r7)
	lgocle	%r6,-5555(%r7)
	lgocnh	%r6,-5555(%r7)
	lgocnp	%r6,-5555(%r7)
	lgocno	%r6,-5555(%r7)
	lgoc	%r6,-5555(%r7),8

	stoco	%r6,-5555(%r7)
	stoch	%r6,-5555(%r7)
	stocp	%r6,-5555(%r7)
	stocnle	%r6,-5555(%r7)
	stocl	%r6,-5555(%r7)
	stocm	%r6,-5555(%r7)
	stocnhe	%r6,-5555(%r7)
	stoclh	%r6,-5555(%r7)
	stocne	%r6,-5555(%r7)
	stocnz	%r6,-5555(%r7)
	stoce	%r6,-5555(%r7)
	stocz	%r6,-5555(%r7)
	stocnlh	%r6,-5555(%r7)
	stoche	%r6,-5555(%r7)
	stocnl	%r6,-5555(%r7)
	stocnm	%r6,-5555(%r7)
	stocle	%r6,-5555(%r7)
	stocnh	%r6,-5555(%r7)
	stocnp	%r6,-5555(%r7)
	stocno	%r6,-5555(%r7)
	stoc	%r6,-5555(%r7),8

	stgoco	%r6,-5555(%r7)
	stgoch	%r6,-5555(%r7)
	stgocp	%r6,-5555(%r7)
	stgocnle %r6,-5555(%r7)
	stgocl	%r6,-5555(%r7)
	stgocm	%r6,-5555(%r7)
	stgocnhe %r6,-5555(%r7)
	stgoclh	%r6,-5555(%r7)
	stgocne	%r6,-5555(%r7)
	stgocnz	%r6,-5555(%r7)
	stgoce	%r6,-5555(%r7)
	stgocz	%r6,-5555(%r7)
	stgocnlh %r6,-5555(%r7)
	stgoche	%r6,-5555(%r7)
	stgocnl	%r6,-5555(%r7)
	stgocnm	%r6,-5555(%r7)
	stgocle	%r6,-5555(%r7)
	stgocnh	%r6,-5555(%r7)
	stgocnp	%r6,-5555(%r7)
	stgocno	%r6,-5555(%r7)
	stgoc	%r6,-5555(%r7),8

	ark	%r6,%r7,%r8
	agrk	%r6,%r7,%r8
	ahik	%r6,%r7,-32000
	aghik	%r6,%r7,-32000
	alrk	%r6,%r7,%r8
	algrk	%r6,%r7,%r8
	alhsik	%r6,%r7,-32000
	alghsik	%r6,%r7,-32000
	nrk	%r6,%r7,%r8
	ngrk	%r6,%r7,%r8
	xrk	%r6,%r7,%r8
	xgrk	%r6,%r7,%r8
	ork	%r6,%r7,%r8
	ogrk	%r6,%r7,%r8
	slak	%r6,%r7,-5555(%r8)
	sllk	%r6,%r7,-5555(%r8)
	srak	%r6,%r7,-5555(%r8)
	srlk	%r6,%r7,-5555(%r8)
	srk	%r6,%r7,%r8
	sgrk	%r6,%r7,%r8
	slrk	%r6,%r7,%r8
	slgrk	%r6,%r7,%r8
	popcnt	%r6,%r7
	rrbm	%r6,%r7

	cefbra %f5,3,%r9,7
	cdfbra %f5,3,%r9,7
	cxfbra %f5,3,%r9,7
	cegbra %f5,3,%r9,7
	cdgbra %f5,3,%r9,7
	cxgbra %f5,3,%r9,7
	celfbr %f5,3,%r9,7
	cdlfbr %f5,3,%r9,7
	cxlfbr %f5,3,%r9,7
	celgbr %f5,3,%r9,7
	cdlgbr %f5,3,%r9,7
	cxlgbr %f5,3,%r9,7
	cfebra %r5,3,%f9,7
	cfdbra %r5,3,%f9,7
	cfxbra %r5,3,%f9,7
	cgebra %r5,3,%f9,7
	cgdbra %r5,3,%f9,7
	cgxbra %r5,3,%f9,7
	clfebr %r5,3,%f9,7
	clfdbr %r5,3,%f9,7
	clfxbr %r5,3,%f9,7
	clgebr %r5,3,%f9,7
	clgdbr %r5,3,%f9,7
	clgxbr %r5,3,%f9,7
	fiebra %f5,3,%f9,7
	fidbra %f5,3,%f9,7
	fixbra %f5,3,%f9,7
	ledbra %f5,3,%f9,7
	ldxbra %f5,3,%f9,7
	lexbra %f5,3,%f9,7
	adtra %f3,%f5,%f9,7
	axtra %f3,%f5,%f9,7
	cdgtra %f5,3,%r9,7
	cdftr %f5,3,%r9,7
	cxftr %f5,3,%r9,7
	cxgtra %f5,3,%r9,7
	cdlgtr %f5,3,%r9,7
	cxlgtr %f5,3,%r9,7
	cdlftr %f5,3,%r9,7
	cxlftr %f5,3,%r9,7
	cgdtra %r5,3,%f9,7
	cgxtra %r5,3,%f9,7
	cfdtr %r5,3,%f9,7
	cfxtr %r5,3,%f9,7
	clgdtr %r5,3,%f9,7
	clgxtr %r5,3,%f9,7
	clfdtr %r5,3,%f9,7
	clfxtr %r5,3,%f9,7
	ddtra %f3,%f5,%f9,7
	dxtra %f3,%f5,%f9,7
	mdtra %f3,%f5,%f9,7
	mxtra %f3,%f5,%f9,7
	sdtra %f3,%f5,%f9,7
	sxtra %f3,%f5,%f9,7
	srnmb 4000(%r7)
