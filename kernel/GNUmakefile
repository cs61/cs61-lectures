QEMUIMAGEFILES = weensyos.img
all: $(QEMUIMAGEFILES) attack
include build/flags.mk

# Place local configuration options, such as `CC=clang`, in
# `config.mk` so you don't have to list them every time.
CONFIG ?= config.mk
-include $(CONFIG)

# `$(V)` controls whether the makefiles print verbose commands (the shell
# commands run by Make) or brief commands (like `COMPILE`).
# For brief commands, run `make all`.
# For verbose commands, run `make V=1 all`.
V = 0
ifeq ($(V),1)
cxxcompile = $(CXX) $(CPPFLAGS) $(DEPCFLAGS) $(1)
assemble = $(CXX) $(CPPFLAGS) $(DEPCFLAGS) $(ASFLAGS) $(1)
link = $(LD) $(LDFLAGS) $(1)
run = $(1) $(3)
else
cxxcompile = @/bin/echo " " $(2) && $(CXX) $(CPPFLAGS) $(DEPCFLAGS) $(1)
assemble = @/bin/echo " " $(2) && $(CXX) $(CPPFLAGS) $(DEPCFLAGS) $(ASFLAGS) $(1)
link = @/bin/echo " " $(2) $(patsubst %.full,%,$@) && $(LD) $(LDFLAGS) $(1)
run = @$(if $(2),/bin/echo " " $(2) $(3) &&,) $(1) $(3)
endif

# `$(D)` controls how QEMU responds to faults. Run `make D=1 run` to
# ask QEMU to print debugging information about interrupts and CPU resets,
# and to quit after the first triple fault instead of rebooting.
#
# `$(NCPU)` controls the number of CPUs QEMU should use. It defaults to 1.
NCPU = 1
LOG ?= file:log.txt
QEMUOPT = -net none -parallel $(LOG) -smp $(NCPU)
ifeq ($(D),1)
QEMUOPT += -d int,cpu_reset,guest_errors -no-reboot -D qemu.log
$(info * Writing detailed QEMU log to `qemu.log`)
else ifeq ($(D),2)
QEMUOPT += -d guest_errors -no-reboot -D qemu.log
$(info * Writing detailed QEMU log to `qemu.log`)
endif
ifneq ($(NOGDB),1)
QEMUGDB ?= -gdb tcp::12949
endif
ifeq ($(STOP),1)
QEMUOPT += -S
endif

QEMURANDSEED := $(strip $(shell od -N8 -tu8 -An /dev/urandom))


# Sets of object files

BOOT_OBJS = $(OBJDIR)/bootentry.o $(OBJDIR)/boot.o

KERNEL_OBJS = $(OBJDIR)/k-exception.ko \
	$(OBJDIR)/kernel.ko $(OBJDIR)/k-vmiter.ko \
	$(OBJDIR)/k-hardware.ko \
	$(OBJDIR)/lib.ko
KERNEL_LINKER_FILES = build/kernel.ld

PROCESSES = $(patsubst %.cc,%,$(wildcard p-*.cc))
PROCESS_LIB_OBJS = $(OBJDIR)/lib.uo $(OBJDIR)/u-lib.uo
PROCESS_OBJS = $(patsubst %,obj/%.uo,$(PROCESSES)) $(PROCESS_LIB_OBJS)
PROCESS_LINKER_FILES = build/process.ld


-include build/rules.mk


# Define `WEENSYOS_FIRST_PROCESS` if appropriate
RUNCMD_LASTWORD := $(filter run-%,$(MAKECMDGOALS))
ifeq ($(words $(RUNCMD_LASTWORD)),1)
RUNCMD_LASTWORD := $(lastword $(subst -, ,$(RUNCMD_LASTWORD)))
ifneq ($(filter p-$(RUNCMD_LASTWORD),$(PROCESSES) p-friends),)
RUNSUFFIX := $(RUNCMD_LASTWORD)
WEENSYOS_FIRST_PROCESS := $(RUNCMD_LASTWORD)
endif
endif
WEENSYOS_FIRST_PROCESS ?= friends

ifneq ($(strip $(WEENSYOS_FIRST_PROCESS)),$(DEP_WEENSYOS_FIRST_PROCESS))
FIRST_PROCESS_BUILDSTAMP := $(shell echo "DEP_WEENSYOS_FIRST_PROCESS:=$(WEENSYOS_FIRST_PROCESS)" > $(DEPSDIR)/_first_process.d)
$(OBJDIR)/k-firstprocess.h: always
$(OBJDIR)/firstprocess.gdb: always
endif
ifeq ($(wildcard $(OBJDIR)/k-firstprocess.h),)
KERNELBUILDSTAMPS += $(OBJDIR)/k-firstprocess.h
endif
ifneq ($(strip $(PROCESSES)),$(DEP_PROCESSES))
PROCESSES_BUILDSTAMP := $(shell echo "DEP_PROCESSES:=$(PROCESSES)" > $(DEPSDIR)/_processes.d)
$(OBJDIR)/k-foreachimage.h: always
endif
ifeq ($(wildcard $(OBJDIR)/k-foreachimage.h),)
KERNELBUILDSTAMPS += $(OBJDIR)/k-foreachimage.h
endif


# How to make object files

$(OBJDIR)/%.ko: %.cc $(KERNELBUILDSTAMPS)
	$(call cxxcompile,-O1 $(KERNELCXXFLAGS) $(DEBUGFLAGS) -DWEENSYOS_KERNEL -c $< -o $@,COMPILE $<)

$(OBJDIR)/%.ko: %.S $(OBJDIR)/k-asm.h $(KERNELBUILDSTAMPS)
	$(call assemble,-O2 -c $< -o $@,ASSEMBLE $<)

$(OBJDIR)/boot.o: $(OBJDIR)/%.o: boot.cc $(KERNELBUILDSTAMPS)
	$(call cxxcompile,-Os $(CXXFLAGS) -c $< -o $@,COMPILE $<)

$(OBJDIR)/bootentry.o: $(OBJDIR)/%.o: \
	bootentry.S $(OBJDIR)/k-asm.h $(KERNELBUILDSTAMPS)
	$(call assemble,-Os -c $< -o $@,ASSEMBLE $<)

$(OBJDIR)/%.uo: %.cc $(BUILDSTAMPS)
	$(call cxxcompile,-O1 $(CXXFLAGS) $(DEBUGFLAGS) -DWEENSYOS_PROCESS -c $< -o $@,COMPILE $<)

$(OBJDIR)/%.uo: %.S $(OBJDIR)/u-asm.h $(BUILDSTAMPS)
	$(call assemble,-O2 -c $< -o $@,ASSEMBLE $<)


# How to make supporting source files

$(OBJDIR)/k-asm.h: $(KERNELBUILDSTAMPS)
	$(call run,$(k_asm_h_input_command) | $(asm_h_build_command) > $@,CREATE $@)
	@if test ! -s $@; then echo '* Error creating $@!' 1>&2; exit 1; fi

$(OBJDIR)/u-asm.h: $(BUILDSTAMPS)
	$(call run,$(u_asm_h_input_command) | $(asm_h_build_command) > $@,CREATE $@)
	@if test ! -s $@; then echo '* Error creating $@!' 1>&2; exit 1; fi

$(OBJDIR)/k-firstprocess.h:
	$(call run,echo '#ifndef WEENSYOS_FIRST_PROCESS' >$@; echo '#define WEENSYOS_FIRST_PROCESS "$(WEENSYOS_FIRST_PROCESS)"' >>$@; echo '#endif' >>$@,CREATE $@)

$(OBJDIR)/k-foreachimage.h: build/mkforeachimage.awk $(PROCESSES_BUILDSTAMP)
	$(call run,awk -f build/mkforeachimage.awk -v processes="$(PROCESSES)" > $@,CREATE $@)

$(OBJDIR)/firstprocess.gdb:
	$(call run,if test "$(WEENSYOS_FIRST_PROCESS)" = friends; then echo "add-symbol-file obj/p-alice.full 0x100000"; echo "add-symbol-file obj/p-eve.full 0x140000"; else echo "add-symbol-file obj/p-$(WEENSYOS_FIRST_PROCESS).full 0x100000"; fi > $@,CREATE $@)

$(OBJDIR)/k-hardware.ko: $(OBJDIR)/k-foreachimage.h


# How to make binaries and the boot sector

PROCESS_BINARIES = $(patsubst %,obj/%,$(PROCESSES))

$(OBJDIR)/kernel.full: $(KERNEL_OBJS) $(PROCESS_BINARIES) $(KERNEL_LINKER_FILES)
	$(call link,-T $(KERNEL_LINKER_FILES) -o $@ $(KERNEL_OBJS) -b binary $(PROCESS_BINARIES),LINK)

$(OBJDIR)/p-%.full: $(OBJDIR)/p-%.uo $(PROCESS_LIB_OBJS) $(PROCESS_LINKER_FILES)
	$(call link,-T $(PROCESS_LINKER_FILES) -o $@ $< $(PROCESS_LIB_OBJS),LINK)

$(OBJDIR)/p-eve.full $(OBJDIR)/p-spawn.full: \
$(OBJDIR)/p-%.full: $(OBJDIR)/p-%.uo $(PROCESS_LIB_OBJS) build/process2.ld
	$(call link,-T build/process2.ld -o $@ $< $(PROCESS_LIB_OBJS),LINK)

$(OBJDIR)/kernel: $(OBJDIR)/kernel.full $(OBJDIR)/mkchickadeesymtab
	$(call run,$(OBJDUMP) -C -S -j .text -j .ctors $< >$@.asm)
	$(call run,$(NM) -n $< >$@.sym)
	$(call run,$(OBJCOPY) -j .text -j .rodata -j .data -j .bss -j .ctors -j .init_array $<,STRIP,$@)
	$(call run,$(OBJDIR)/mkchickadeesymtab $@)

$(OBJDIR)/%: $(OBJDIR)/%.full
	$(call run,$(OBJDUMP) -C -S -j .text -j .ctors $< >$@.asm)
	$(call run,$(NM) -n $< >$@.sym)
	$(call run,$(QUIETOBJCOPY) -j .text -j .rodata -j .data -j .bss -j .ctors -j .init_array $<,STRIP,$@)

$(OBJDIR)/bootsector: $(BOOT_OBJS) build/boot.ld
	$(call link,-T build/boot.ld -z noexecstack -o $@.full $(BOOT_OBJS),LINK)
	$(call run,$(OBJDUMP) -C -S $@.full >$@.asm)
	$(call run,$(NM) -n $@.full >$@.sym)
	$(call run,$(OBJCOPY) -S -O binary -j .text $@.full $@)


# How to make host program for ensuring a loaded symbol table

$(OBJDIR)/mkchickadeesymtab: build/mkchickadeesymtab.cc $(BUILDSTAMPS)
	$(call run,$(HOSTCXX) $(HOSTCPPFLAGS) $(HOSTCXXFLAGS) $(DEPCFLAGS) -g -o $@,HOSTCOMPILE,$<)


# How to make host programs for constructing & checking file systems

$(OBJDIR)/%.o: %.cc $(BUILDSTAMPS)
	$(call run,$(HOSTCXX) $(HOSTCPPFLAGS) $(HOSTCXXFLAGS) $(DEPCFLAGS) -c -o $@,HOSTCOMPILE,$<)

$(OBJDIR)/%.o: build/%.cc $(BUILDSTAMPS)
	$(call run,$(HOSTCXX) $(HOSTCPPFLAGS) $(HOSTCXXFLAGS) $(DEPCFLAGS) -c -o $@,HOSTCOMPILE,$<)

$(OBJDIR)/mkbootdisk: build/mkbootdisk.cc $(BUILDSTAMPS)
	$(call run,$(HOSTCXX) $(HOSTCPPFLAGS) $(HOSTCXXFLAGS) $(DEPCFLAGS) -g -o $@,HOSTCOMPILE,$<)


weensyos.img: $(OBJDIR)/mkbootdisk $(OBJDIR)/bootsector $(OBJDIR)/kernel
	$(call run,$(OBJDIR)/mkbootdisk $(OBJDIR)/bootsector $(OBJDIR)/kernel > $@,CREATE $@)


# How to make `attack`

attack: attack.cc $(BUILDSTAMPS)
	$(call run,$(HOSTCXX) $(CPPFLAGS) $(HOSTCXXFLAGS) $(DEPCFLAGS) -o $@,HOSTCOMPILE,$<)


# How to run QEMU

QEMUIMG = -M q35 \
	-device piix4-ide,bus=pcie.0,id=piix4-ide \
	-drive file=weensyos.img,if=none,format=raw,id=bootdisk \
	-device ide-hd,drive=bootdisk,bus=piix4-ide.0

run: run-$(QEMUDISPLAY)
	@:
run-gdb-report:
	@if test "$(QEMUGDB)" = "-gdb tcp::12949"; then echo '* Run `gdb -ix weensyos.gdb` to connect gdb to qemu.' 1>&2; fi
run-graphic: $(QEMUIMAGEFILES) $(GDBFILES) check-qemu run-gdb-report
	$(call run,$(QEMU_PRELOAD) $(QEMU) $(QEMUOPT) $(QEMUGDB) $(QEMUIMG),QEMU $<)
run-console: $(QEMUIMAGEFILES) $(GDBFILES) check-qemu-console run-gdb-report
	$(call run,$(QEMU) $(QEMUOPT) -display curses $(QEMUGDB) $(QEMUIMG),QEMU $<)
run-monitor: $(QEMUIMAGEFILES) $(GDBFILES) check-qemu
	$(call run,$(QEMU_PRELOAD) $(QEMU) $(QEMUOPT) -monitor stdio $(QEMUIMG),QEMU $<)
run-gdb: run-gdb-$(QEMUDISPLAY)
	@:
run-gdb-graphic: $(QEMUIMAGEFILES) $(GDBFILES) check-qemu
	$(call run,$(QEMU_PRELOAD) $(QEMU) $(QEMUOPT) -gdb tcp::12949 $(QEMUIMG) &,QEMU $<)
	$(call run,sleep 0.5; gdb -ix weensyos.gdb,GDB)
run-gdb-console: $(QEMUIMAGEFILES) $(GDBFILES) check-qemu-console
	$(call run,$(QEMU) $(QEMUOPT) -display curses -gdb tcp::12949 $(QEMUIMG),QEMU $<)

run-$(RUNSUFFIX): run
run-graphic-$(RUNSUFFIX): run-graphic
run-console-$(RUNSUFFIX): run-console
run-monitor-$(RUNSUFFIX): run-monitor
run-gdb-$(RUNSUFFIX): run-gdb
run-gdb-graphic-$(RUNSUFFIX): run-gdb-graphic
run-gdb-console-$(RUNSUFFIX): run-gdb-console

# Stop all my qemus
stop kill:
	-killall -u $$(whoami) $(QEMU)
	@sleep 0.2; if ps -U $$(whoami) | grep $(QEMU) >/dev/null; then killall -9 -u $$(whoami) $(QEMU); fi
