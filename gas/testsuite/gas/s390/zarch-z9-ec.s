.text
foo:
	lpdfr	%f6,%f2
	lndfr	%f6,%f2
	cpsdr	%f6,%f1,%f2
	lcdfr	%f6,%f2
	ldgr	%f6,%r2
	lgdr	%r2,%f6
	adtr	%f6,%f2,%f4
	axtr	%f6,%f2,%f4
	cdtr	%f6,%f2
	cxtr	%f6,%f2
	kdtr	%f6,%f2
	kxtr	%f6,%f2
	cedtr	%f6,%f2
	cextr	%f6,%f2
	cdgtr	%f6,%r2
	cxgtr	%f6,%r2
	cdstr	%f6,%r2
	cxstr	%f6,%r2
	cdutr	%f6,%r2
	cxutr	%f6,%r2
	cgdtr	%r2,1,%f6
	cgxtr	%r2,1,%f6
	csdtr	%r2,%f6
	csxtr	%r2,%f6
	cudtr	%r2,%f6
	cuxtr	%r2,%f6
	ddtr	%f6,%f2,%f4
	dxtr	%f6,%f2,%f4
	eedtr	%r2,%f6
	eextr	%r2,%f6
	esdtr	%r2,%f6
	esxtr	%r2,%f6
	iedtr	%f6,%f2,%r4
	iextr	%f6,%f2,%r4
	ltdtr	%f6,%f2
	ltxtr	%f6,%f2
	fidtr	%f6,1,%f2,3
	fixtr	%f6,1,%f2,3
	lfas	3(%r1)
	ldetr	%f6,%f2,1
	lxdtr	%f6,%f2,1
	ledtr	%f6,1,%f2,3
	ldxtr	%f6,1,%f2,3
	mdtr	%f6,%f2,%f4
	mxtr	%f6,%f2,%f4
	qadtr	%f6,%f2,%f4,1
	qaxtr	%f6,%f2,%f4,1
	rrdtr	%f6,%f2,%f4,1
	rrxtr	%f6,%f2,%f4,1
	srnmt	3(%r1)
	sfasr	%r2
	sldt	%f6,%f2,3(%r1,%r4)
	slxt	%f6,%f2,3(%r1,%r4)
	srdt	%f6,%f2,3(%r1,%r4)
	srxt	%f6,%f2,3(%r1,%r4)
	sdtr	%f6,%f2,%f4
	sxtr	%f6,%f2,%f4
	tcet	%f6,3(%r1,%r2)
	tcdt	%f6,3(%r1,%r2)
	tcxt	%f6,3(%r1,%r2)
	tget	%f6,3(%r1,%r2)
	tgdt	%f6,3(%r1,%r2)
	tgxt	%f6,3(%r1,%r2)
