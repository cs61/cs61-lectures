# Initialize compiler flags so `config.mk` can augment them

# Dependency files
DEPCFLAGS = -MD -MF $(DEPSDIR)/$(@F).d -MP

# Flags for building programs that run on the host (not in WeensyOS)
HOSTCPPFLAGS = $(DEFS) -I.
HOSTCFLAGS := -std=gnu2x $(CFLAGS) -Wall -W
HOSTCXXFLAGS := -std=gnu++2a $(CXXFLAGS) -Wall -W

# Flags for building WeensyOS kernel and process code

# - Preprocessor flags
CPPFLAGS = $(DEFS) -I.

# - Flags common to C and C++, and to kernel and user code
CCOMMONFLAGS := -m64 -mno-mmx -mno-sse -mno-sse2 -mno-sse3 \
	-mno-3dnow -ffreestanding -fno-pic -fno-stack-protector \
	-Wall -W -Wshadow -Wno-format -Wno-unused-parameter

# - Flags for C
CFLAGS := -std=gnu2x $(CCOMMONFLAGS) $(CFLAGS)

# - Flags for C++
CXXFLAGS := -std=gnu++2a -fno-exceptions -fno-rtti -ffunction-sections \
	$(CCOMMONFLAGS) $(CXXFLAGS)

# - Flags for debuggability (not used in boot loader)
DEBUGFLAGS := -gdwarf-4 -fno-omit-frame-pointer -fno-optimize-sibling-calls \
	-mno-omit-leaf-frame-pointer

# - Flags for kernel sources
KERNELCXXFLAGS := $(CXXFLAGS) -mno-red-zone $(SANITIZEFLAGS)

# - Assembler flags
ASFLAGS := $(CCOMMONFLAGS)

# - Linker flags
LDFLAGS := $(LDFLAGS) -Os --gc-sections -z max-page-size=0x1000 \
	-z noexecstack -static -nostdlib
