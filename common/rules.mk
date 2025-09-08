# compiler flags
CFLAGS := -std=gnu2x -Wall -Wextra -Wshadow -Wvla -g $(DEFS) $(CFLAGS)
CXXFLAGS := -std=gnu++2a -Wall -Wextra -Wshadow -Wvla -g $(DEFS) $(CXXFLAGS)

O ?= -O3
ifeq ($(filter 0 1 2 3 s z g fast,$(O)),$(strip $(O)))
override O := -O$(O)
endif

PTHREAD ?= 0
ifeq ($(PTHREAD),1)
CFLAGS += -pthread
CXXFLAGS += -pthread
WANT_TSAN ?= 1
endif

PIE ?= 1
ifeq ($(PIE),0)
LDFLAGS += -no-pie
endif

STATIC ?= 0
ifeq ($(STATIC),1)
LDFLAGS += -static
endif

# maybe skip x86 versions in ARM Docker
X86 ?= 1
ifneq ($(X86),1)
 ifneq ($(findstring /usr/x86_64-linux-gnu/bin:,$(PATH)),)
PATH := $(subst /usr/x86_64-linux-gnu/bin:,,$(PATH))
 endif
endif

# find best GCC and Clang
ifndef BEST_GCC
BEST_GCC := $(shell for s in -15 -14 -13 ""; do if gcc$$s --version 2>/dev/null | grep -e 'Free Software' >/dev/null; then echo gcc$$s; exit; fi; done; echo false)
endif
ifndef BEST_GXX
BEST_GXX := $(shell for s in -15 -14 -13 ""; do if g++$$s --version 2>/dev/null | grep -e 'Free Software' >/dev/null; then echo g++$$s; exit; fi; done; echo false)
endif
ifndef BEST_CLANG
BEST_CLANG := $(shell for s in -21 -20 -19 -18 ""; do if clang$$s --version 2>/dev/null | grep -e 'LLVM\|clang' >/dev/null; then echo clang$$s; exit; fi; done; echo false)
endif
ifndef BEST_CLANGXX
BEST_CLANGXX := $(shell for s in -21 -20 -19 -18 ""; do if clang++$$s --version 2>/dev/null | grep -e 'LLVM\|clang' >/dev/null; then echo clang++$$s; exit; fi; done; echo false)
endif

# compiler variant
ifndef COMPILER
 ifeq ($(BEST_GCC),false)
COMPILER := clang
 endif
endif
ifeq ($(origin CC),default)
 ifeq ($(COMPILER),clang)
CC = $(BEST_CLANG)
 else
CC = $(BEST_GCC)
 endif
endif
ifeq ($(origin CXX),default)
 ifeq ($(COMPILER),clang)
CXX = $(BEST_CLANGXX)
 else
CXX = $(BEST_GXX)
 endif
endif

ISCLANG := $(shell if $(CXX) --version 2>/dev/null | grep -e 'LLVM\|clang' >/dev/null; then echo 1; else echo 0; fi)
ifeq ($(ISCLANG),1)
BADCXXFLAGS ?= -fno-if-conversion -fno-if-conversion2
endif

# sanitizer arguments; sanitizer defaults to on
ifndef SAN
SAN := $(or $(SANITIZE),$(ASAN),$(UBSAN),1)
endif
ifndef TSAN
 ifneq ($(and $(filter-out 0,$(WANT_TSAN)),$(filter 0,$(or $(ASAN),0))),)
TSAN := $(SAN)
 endif
endif
ifndef ASAN
 ifeq ($(filter-out 0,$(or $(TSAN),0)),)
ASAN := $(if $(strip $(shell $(CC) -v 2>&1 | grep 'build=aarch.*target=x86')),0,1)
 endif
endif

check_for_sanitizer = $(if $(strip $(shell $(CC) -fsanitize=$(1) -x c -o /dev/null /dev/null 2>&1 | grep main)),1,$(info ** WARNING: The `$(CC)` compiler does not support `-fsanitize=$(1)`.))
SANFLAGS :=
ifneq ($(and $(filter-out 0,$(TSAN)),$(filter-out 0,$(ASAN))),)
$(error "ASAN=1 conflicts with TSAN=1, pick one or the other")
endif
ifeq ($(TSAN),1)
 ifeq ($(call check_for_sanitizer,thread),1)
SANFLAGS += -fsanitize=thread
 endif
else
 ifeq ($(or $(ASAN),$(LSAN),$(LEAKSAN)),1)
  ifeq ($(call check_for_sanitizer,address),1)
SANFLAGS += -fsanitize=address
  endif
 endif
 ifeq ($(or $(LSAN),$(LEAKSAN)),1)
  ifeq ($(call check_for_sanitizer,leak),1)
SANFLAGS += -fsanitize=leak
  endif
 endif
endif
ifeq ($(or $(UBSAN),1),1)
 ifeq ($(call check_for_sanitizer,undefined),1)
SANFLAGS += -fsanitize=undefined -fno-sanitize-recover=undefined
 endif
endif
ifeq ($(or $(TSAN),$(LSAN),$(LEAKSAN),$(SAN)),1)
CPPFLAGS += -DSAN=1
CFLAGS += $(SANFLAGS)
CXXFLAGS += $(SANFLAGS)
endif

# profiling
ifeq ($(or $(PROFILE),$(PG)),1)
CFLAGS += -pg
CXXFLAGS += -pg
endif

# NDEBUG
ifeq ($(NDEBUG),1)
CPPFLAGS += -DNDEBUG=1
CFLAGS += -Wno-unused
CXXFLAGS += -Wno-unused
endif

# these rules ensure dependencies are created
DEPCFLAGS = -MD -MF $(DEPSDIR)/$(patsubst %.o,%,$(@F)).d -MP
DEPSDIR := .deps
BUILDSTAMP := $(DEPSDIR)/rebuildstamp
DEPFILES := $(wildcard $(DEPSDIR)/*.d)
ifneq ($(DEPFILES),)
include $(DEPFILES)
endif

# when the C compiler or optimization flags change, rebuild all objects
ifneq ($(strip $(DEP_CC)),$(strip $(CC) $(CPPFLAGS) $(CFLAGS) $(O) X86=$(X86)))
DEP_CC := $(shell mkdir -p $(DEPSDIR); echo >$(BUILDSTAMP); echo "DEP_CC:=$(CC) $(CPPFLAGS) $(CFLAGS) $(O) X86=$(X86)" >$(DEPSDIR)/_cc.d)
endif
ifneq ($(strip $(DEP_CXX)),$(strip $(CXX) $(CPPFLAGS) $(CXXFLAGS) $(O) X86=$(X86) $(LDFLAGS)))
DEP_CXX := $(shell mkdir -p $(DEPSDIR); echo >$(BUILDSTAMP); echo "DEP_CXX:=$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(O) X86=$(X86) $(LDFLAGS)" >$(DEPSDIR)/_cxx.d)
endif


V = 0
ifeq ($(V),1)
run = $(1) $(3)
xrun = /bin/echo "$(1) $(3)" && $(1) $(3)
else
run = @$(if $(2),/bin/echo "  $(2) $(3)" &&,) $(1) $(3)
xrun = $(if $(2),/bin/echo "  $(2) $(3)" &&,) $(1) $(3)
endif
runquiet = @$(1) $(3)

CXX_LINK_PREREQUISITES = $(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^

CLEANASM = 1
ifeq ($(CLEANASM),1)
cleanasm = perl -ni -e '$$badsection = !!/\.note\.gnu/ if /^\s+\.section/; print if !/^(?:\# BB|\s+\.cfi|\s+\.p2align|\s+\.loc|\.LF[BE]|\.LVL|\s+\# =>This|\s+\# kill)/ && !$$badsection' $(1)
else
cleanasm = :
endif

DEFAULT_ASM_CXXFLAGS ?= $(O)
flagged_compile = @ARGS=$$(grep '^//!' $< | sed 's/.*!//$(patsubst %,;s/ % */ /,$(BADCXXFLAGS));s/^ | $$//'); \
	  if test -z "$$ARGS"; then ARGS="$(DEFAULT_ASM_CXXFLAGS)"; fi; \
	  $(call xrun,$(CXX) $(3) $$ARGS -o $(2) $(1),COMPILE $$ARGS $(1) -o $(2))
flagged_compile_S = $(call flagged_compile,$(1),$(2),$(filter-out -g,$(3) -S)) && { $(call cleanasm,$(2)); }
flagged_compile_c = $(call flagged_compile,$(1),$(2),$(3) -c)

buildasm:
	rm -f $(BUILDABLEASM)
	$(MAKE) $(BUILDABLEASM)


PERCENT := %

# cancel implicit rules we don't want
%: %.c
%.o: %.c
%: %.cc
%.o: %.cc
%: %.o
%.o: %.s

$(BUILDSTAMP):
	@mkdir -p $(@D)
	@echo >$@

always:
	@:

clean-hook:
	@:

.PHONY: always buildasm clean-hook
.PRECIOUS: %.o
