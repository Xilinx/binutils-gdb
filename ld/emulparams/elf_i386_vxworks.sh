SCRIPT_NAME=elf
OUTPUT_FORMAT="elf32-i386-vxworks"
TEXT_START_ADDR=0x08048000
MAXPAGESIZE="CONSTANT (MAXPAGESIZE)"
COMMONPAGESIZE="CONSTANT (COMMONPAGESIZE)"
NONPAGED_TEXT_START_ADDR=0x08048000
ARCH=i386
MACHINE=
NOP=0x90909090
TEMPLATE_NAME=elf32
GENERATE_SHLIB_SCRIPT=yes
GENERATE_PIE_SCRIPT=yes
NO_SMALL_DATA=yes
. ${srcdir}/emulparams/vxworks.sh
