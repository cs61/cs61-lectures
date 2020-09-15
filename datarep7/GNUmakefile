BASEPROGRAMS = simplealloc divzero ubsignedinc ubsignedloop ubsignedloop-noub
PROGRAMS = simplealloc-O0 simplealloc divzero \
	ubsignedinc-O0 ubsignedinc \
	ubsignedloop-O0 ubsignedloop ubsignedloop-noub \
	ubunsignedloop-O0 ubunsignedloop
all: $(PROGRAMS)


COMPILER ?= clang
PIE ?= 0
O ?= 3
include ../common/rules.mk
variants = $(1) $(addsuffix -O0,$(1)) $(addsuffix -clang,$(1)) $(addsuffix -clang-O0,$(1)) \
	$(addsuffix -gcc,$(1)) $(addsuffix -gcc-O0,$(1))


%.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(O) $(DEPCFLAGS) -o $@ -c $<

%-O0.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O0 $(DEPCFLAGS) -o $@ -c $<

%-clang.o: %.cc $(BUILDSTAMP)
	clang++ $(CPPFLAGS) $(CXXFLAGS) $(O) $(DEPCFLAGS) -o $@ -c $<

%-clang-O0.o: %.cc $(BUILDSTAMP)
	clang++ $(CPPFLAGS) $(CXXFLAGS) -O0 $(DEPCFLAGS) -o $@ -c $<

%-gcc.o: %.cc $(BUILDSTAMP)
	clang++ $(CPPFLAGS) $(CXXFLAGS) $(O) $(DEPCFLAGS) -o $@ -c $<

$(call variants,simplealloc divzero ubsignedinc): %: %.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^

$(call variants,ubsignedloop ubsignedloop-noub ubunsignedloop): %: %.o hexdump.o use_int.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^


clean:
	rm -rf $(call variants,$(BASEPROGRAMS)) *.o $(DEPSDIR)

.PHONY: all clean
.PRECIOUS: %-clang.o %-gcc.o %-clang-O0.o %-gcc-O0.o
