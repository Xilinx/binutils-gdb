/* As the included file, but specifying silent ENOSYS.
#notarget: cris*-*-elf
#sim: --cris-unknown-syscall=enosys-quiet
#output: ENOSYS\n
#output: xyzzy\n
*/

#include "sigreturn1.c"
