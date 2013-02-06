	.data
data_label:
	.extern big_external_data_label,0x4000
	.extern small_external_data_label,4
	.comm big_external_common,0x4000
	.comm small_external_common,4
	.lcomm big_local_common,0x4000
	.lcomm small_local_common,4

# the small symbols should have space allocated in the sbss section
# but this is not yet supported in the assembler, so space is allocated
# in the .bss section and the relocations are not gp-relative. this will
# be updated when gp-relative relocations are added
	.text
	ldb	r4,0(r0)
	ldb	r4,4(r0)
	ldb	r4,0x7ffc(r0)
	ldb	r4,-0x8000(r0)
	ldb	r4,0(r5)
	ldb	r4,4(r5)
	ldb	r4,0x7ffc(r5)
	ldb	r4,-0x8000(r5)
	ldb	r4,data_label(r0)
	ldb	r4,big_external_data_label(r0)
	ldb	r4,small_external_data_label(r0)
	ldb	r4,big_external_common(r0)
	ldb	r4,small_external_common(r0)
	ldb	r4,big_local_common(r0)
	ldb	r4,small_local_common(r0)
	ldb	r4,data_label+4(r0)
	ldb	r4,big_external_data_label+4(r0)
	ldb	r4,small_external_data_label+4(r0)
	ldb	r4,big_external_common+4(r0)
	ldb	r4,small_external_common+4(r0)
	ldb	r4,big_local_common+4(r0)
	ldb	r4,small_local_common+4(r0)
	ldb	r4,data_label-0x8000(r0)
	ldb	r4,big_external_data_label-0x8000(r0)
	ldb	r4,small_external_data_label-0x8000(r0)
	ldb	r4,big_external_common-0x8000(r0)
	ldb	r4,small_external_common-0x8000(r0)
	ldb	r4,big_local_common-0x8000(r0)
	ldb	r4,small_local_common-0x8000(r0)
	ldb	r4,data_label+0x10000(r0)
	ldb	r4,data_label(r5)
	ldb	r4,big_external_data_label(r5)
	ldb	r4,small_external_data_label(r5)
	ldb	r4,big_external_common(r5)
	ldb	r4,small_external_common(r5)
	ldb	r4,big_local_common(r5)
	ldb	r4,small_local_common(r5)
	ldb	r4,data_label+4(r5)
	ldb	r4,big_external_data_label+4(r5)
	ldb	r4,small_external_data_label+4(r5)
	ldb	r4,big_external_common+4(r5)
	ldb	r4,small_external_common+4(r5)
	ldb	r4,big_local_common+4(r5)
	ldb	r4,small_local_common+4(r5)
	ldb	r4,data_label-0x8000(r5)
	ldb	r4,big_external_data_label-0x8000(r5)
	ldb	r4,small_external_data_label-0x8000(r5)
	ldb	r4,big_external_common-0x8000(r5)
	ldb	r4,small_external_common-0x8000(r5)
	ldb	r4,big_local_common-0x8000(r5)
	ldb	r4,small_local_common-0x8000(r5)
	
	ldbio	r4,0(r0)
	ldbio	r4,4(r0)
	ldbio	r4,0x7ffc(r0)
	ldbio	r4,-0x8000(r0)
	ldbio	r4,0(r5)
	ldbio	r4,4(r5)
	ldbio	r4,0x7ffc(r5)
	ldbio	r4,-0x8000(r5)
	ldbio	r4,data_label(r0)
	ldbio	r4,big_external_data_label(r0)
	ldbio	r4,small_external_data_label(r0)
	ldbio	r4,big_external_common(r0)
	ldbio	r4,small_external_common(r0)
	ldbio	r4,big_local_common(r0)
	ldbio	r4,small_local_common(r0)
	ldbio	r4,data_label+4(r0)
	ldbio	r4,big_external_data_label+4(r0)
	ldbio	r4,small_external_data_label+4(r0)
	ldbio	r4,big_external_common+4(r0)
	ldbio	r4,small_external_common+4(r0)
	ldbio	r4,big_local_common+4(r0)
	ldbio	r4,small_local_common+4(r0)
	ldbio	r4,data_label-0x8000(r0)
	ldbio	r4,big_external_data_label-0x8000(r0)
	ldbio	r4,small_external_data_label-0x8000(r0)
	ldbio	r4,big_external_common-0x8000(r0)
	ldbio	r4,small_external_common-0x8000(r0)
	ldbio	r4,big_local_common-0x8000(r0)
	ldbio	r4,small_local_common-0x8000(r0)
	ldbio	r4,data_label+0x10000(r0)
	ldbio	r4,data_label(r5)
	ldbio	r4,big_external_data_label(r5)
	ldbio	r4,small_external_data_label(r5)
	ldbio	r4,big_external_common(r5)
	ldbio	r4,small_external_common(r5)
	ldbio	r4,big_local_common(r5)
	ldbio	r4,small_local_common(r5)
	ldbio	r4,data_label+4(r5)
	ldbio	r4,big_external_data_label+4(r5)
	ldbio	r4,small_external_data_label+4(r5)
	ldbio	r4,big_external_common+4(r5)
	ldbio	r4,small_external_common+4(r5)
	ldbio	r4,big_local_common+4(r5)
	ldbio	r4,small_local_common+4(r5)
	ldbio	r4,data_label-0x8000(r5)
	ldbio	r4,big_external_data_label-0x8000(r5)
	ldbio	r4,small_external_data_label-0x8000(r5)
	ldbio	r4,big_external_common-0x8000(r5)
	ldbio	r4,small_external_common-0x8000(r5)
	ldbio	r4,big_local_common-0x8000(r5)
	ldbio	r4,small_local_common-0x8000(r5)
