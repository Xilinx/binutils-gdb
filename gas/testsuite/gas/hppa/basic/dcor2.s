	.LEVEL 2.0
	.SPACE $PRIVATE$
	.SUBSPA $DATA$,QUAD=1,ALIGN=8,ACCESS=31
	.SUBSPA $BSS$,QUAD=1,ALIGN=8,ACCESS=31,ZERO,SORT=82
	.SPACE $TEXT$
	.SUBSPA $LIT$,QUAD=0,ALIGN=8,ACCESS=44
	.SUBSPA $CODE$,QUAD=0,ALIGN=8,ACCESS=44,CODE_ONLY

	.SPACE $TEXT$
	.SUBSPA $CODE$

	.align 4
; PA2.0 decimal correction instruction tests.  
;
; We could/should test some of the corner cases for register and 
; immediate fields.  We should also check the assorted field
; selectors to make sure they're handled correctly.
	dcor,* %r4,%r5
	dcor,*sbz %r4,%r5
	dcor,*shz %r4,%r5
	dcor,*sdc %r4,%r5
	dcor,*sbc %r4,%r5
	dcor,*shc %r4,%r5
	dcor,*tr %r4,%r5
	dcor,*nbz %r4,%r5
	dcor,*nhz %r4,%r5
	dcor,*ndc %r4,%r5
	dcor,*nbc %r4,%r5
	dcor,*nhc %r4,%r5

	dcor,i,* %r4,%r5
	dcor,i,*sbz %r4,%r5
	dcor,i,*shz %r4,%r5
	dcor,i,*sdc %r4,%r5
	dcor,i,*sbc %r4,%r5
	dcor,i,*shc %r4,%r5
	dcor,i,*tr %r4,%r5
	dcor,i,*nbz %r4,%r5
	dcor,i,*nhz %r4,%r5
	dcor,i,*ndc %r4,%r5
	dcor,i,*nbc %r4,%r5
	dcor,i,*nhc %r4,%r5

