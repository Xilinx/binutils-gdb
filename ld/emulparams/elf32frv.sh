MACHINE=
SCRIPT_NAME=elf
TEMPLATE_NAME=generic
EXTRA_EM_FILE=genelf
OUTPUT_FORMAT="elf32-frv"
TEXT_START_ADDR=0x10000
ARCH=frv
MAXPAGESIZE=256
ENTRY=_start
EMBEDDED=yes
OTHER_EXCLUDE_FILES='*frvend.o'
OTHER_END_SYMBOLS='__end = .;'
DATA_START_SYMBOLS='__data_start = . ;'
STACK_ADDR=0x200000
NOP=0x80000000
OTHER_GOT_SYMBOLS='
  . = ALIGN(8);  _gp = . + 2048;
  PROVIDE (gp = _gp);'
OTHER_READONLY_SECTIONS='
  .rofixup        : { *(.rofixup) }
'
