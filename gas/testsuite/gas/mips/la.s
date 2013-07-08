# Source file used to test the la macro.
	
	.data
data_label:
	.extern big_external_data_label,1000
	.extern small_external_data_label,1
	.comm big_external_common,1000
	.comm small_external_common,1
	.lcomm big_local_common,1000
	.lcomm small_local_common,1
	
	.text
	la	$4,0
	la	$4,1
	la	$4,0x8000
	la	$4,-0x8000
	la	$4,0x10000
	la	$4,0x1a5a5
	la	$4,0($5)
	la	$4,1($5)
	la	$4,0x8000($5)
	la	$4,-0x8000($5)
	la	$4,0x10000($5)
	la	$4,0x1a5a5($5)
	la	$4,data_label
	la	$4,big_external_data_label
	la	$4,small_external_data_label
	la	$4,big_external_common
	la	$4,small_external_common
	la	$4,big_local_common
	la	$4,small_local_common
	la	$4,data_label+1
	la	$4,big_external_data_label+1
	la	$4,small_external_data_label+1
	la	$4,big_external_common+1
	la	$4,small_external_common+1
	la	$4,big_local_common+1
	la	$4,small_local_common+1
	la	$4,data_label+0x8000
	la	$4,big_external_data_label+0x8000
	la	$4,small_external_data_label+0x8000
	la	$4,big_external_common+0x8000
	la	$4,small_external_common+0x8000
	la	$4,big_local_common+0x8000
	la	$4,small_local_common+0x8000
	la	$4,data_label-0x8000
	la	$4,big_external_data_label-0x8000
	la	$4,small_external_data_label-0x8000
	la	$4,big_external_common-0x8000
	la	$4,small_external_common-0x8000
	la	$4,big_local_common-0x8000
	la	$4,small_local_common-0x8000
	la	$4,data_label+0x10000
	la	$4,big_external_data_label+0x10000
	la	$4,small_external_data_label+0x10000
	la	$4,big_external_common+0x10000
	la	$4,small_external_common+0x10000
	la	$4,big_local_common+0x10000
	la	$4,small_local_common+0x10000
	la	$4,data_label+0x1a5a5
	la	$4,big_external_data_label+0x1a5a5
	la	$4,small_external_data_label+0x1a5a5
	la	$4,big_external_common+0x1a5a5
	la	$4,small_external_common+0x1a5a5
	la	$4,big_local_common+0x1a5a5
	la	$4,small_local_common+0x1a5a5
	la	$4,data_label($5)
	la	$4,big_external_data_label($5)
	la	$4,small_external_data_label($5)
	la	$4,big_external_common($5)
	la	$4,small_external_common($5)
	la	$4,big_local_common($5)
	la	$4,small_local_common($5)
	la	$4,data_label+1($5)
	la	$4,big_external_data_label+1($5)
	la	$4,small_external_data_label+1($5)
	la	$4,big_external_common+1($5)
	la	$4,small_external_common+1($5)
	la	$4,big_local_common+1($5)
	la	$4,small_local_common+1($5)
	la	$4,data_label+0x8000($5)
	la	$4,big_external_data_label+0x8000($5)
	la	$4,small_external_data_label+0x8000($5)
	la	$4,big_external_common+0x8000($5)
	la	$4,small_external_common+0x8000($5)
	la	$4,big_local_common+0x8000($5)
	la	$4,small_local_common+0x8000($5)
	la	$4,data_label-0x8000($5)
	la	$4,big_external_data_label-0x8000($5)
	la	$4,small_external_data_label-0x8000($5)
	la	$4,big_external_common-0x8000($5)
	la	$4,small_external_common-0x8000($5)
	la	$4,big_local_common-0x8000($5)
	la	$4,small_local_common-0x8000($5)
	la	$4,data_label+0x10000($5)
	la	$4,big_external_data_label+0x10000($5)
	la	$4,small_external_data_label+0x10000($5)
	la	$4,big_external_common+0x10000($5)
	la	$4,small_external_common+0x10000($5)
	la	$4,big_local_common+0x10000($5)
	la	$4,small_local_common+0x10000($5)
	la	$4,data_label+0x1a5a5($5)
	la	$4,big_external_data_label+0x1a5a5($5)
	la	$4,small_external_data_label+0x1a5a5($5)
	la	$4,big_external_common+0x1a5a5($5)
	la	$4,small_external_common+0x1a5a5($5)
	la	$4,big_local_common+0x1a5a5($5)
	la	$4,small_local_common+0x1a5a5($5)
	la	$4,($5)
	la	$4,(0x123456)
	la	$4,(0x123456)($5)
	la	$4,(big_external_data_label)
	la	$4,(big_external_data_label)($5)

# Force at least 8 (non-delay-slot) zero bytes, to make 'objdump' print ...
	.space  8
