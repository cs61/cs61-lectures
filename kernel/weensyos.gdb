set $loaded = 1
set arch i386:x86-64
file obj/kernel.full
add-symbol-file obj/bootsector.full 0x7c00
source obj/firstprocess.gdb
target remote localhost:12949
source build/functions.gdb
display/5i $pc
