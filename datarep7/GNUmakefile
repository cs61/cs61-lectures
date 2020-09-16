BASEPROGRAMS = simplealloc divzero ubsignedinc ubsignedloop ubsignedloop-noub
PROGRAMS = simplealloc-O0 simplealloc divzero \
	ubsignedinc-O0 ubsignedinc \
	ubsignedloop-O0 ubsignedloop ubsignedloop-noub \
	ubunsignedloop-O0 ubunsignedloop
all: $(PROGRAMS)

variants = $(1) $(addsuffix -O0,$(1)) \
	$(addsuffix -clang,$(1)) $(addsuffix -clang-O0,$(1)) \
	$(addsuffix -gcc,$(1)) $(addsuffix -gcc-O0,$(1))


COMPILER ?= clang
PIE ?= 0
O ?= 3
include ../common/rules.mk


%.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(O) $(DEPCFLAGS) -o $@ -c $<

%-clang.o: %.cc $(BUILDSTAMP)
	clang++ $(CPPFLAGS) $(CXXFLAGS) $(O) $(DEPCFLAGS) -o $@ -c $<

%-gcc.o: %.cc $(BUILDSTAMP)
	g++ $(CPPFLAGS) $(CXXFLAGS) $(O) $(DEPCFLAGS) -o $@ -c $<

%-O0.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O0 $(DEPCFLAGS) -o $@ -c $<

%-clang-O0.o: %.cc $(BUILDSTAMP)
	clang++ $(CPPFLAGS) $(CXXFLAGS) -O0 $(DEPCFLAGS) -o $@ -c $<

%-gcc-O0.o: %.cc $(BUILDSTAMP)
	g++ $(CPPFLAGS) $(CXXFLAGS) -O0 $(DEPCFLAGS) -o $@ -c $<


$(BASEPROGRAMS): %: %.o hexdump.o use_int.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^

$(addsuffix -clang,$(BASEPROGRAMS)): %: %.o hexdump.o use_int.o
	clang++ $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^

$(addsuffix -gcc,$(BASEPROGRAMS)): %: %.o hexdump.o use_int.o
	g++ $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^

$(addsuffix -O0,$(BASEPROGRAMS)): %: %.o hexdump.o use_int.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -O0 -o $@ $^

$(addsuffix -clang-O0,$(BASEPROGRAMS)): %: %.o hexdump.o use_int.o
	clang++ $(CXXFLAGS) $(LDFLAGS) -O0 -o $@ $^

$(addsuffix -gcc-O0,$(BASEPROGRAMS)): %: %.o hexdump.o use_int.o
	g++ $(CXXFLAGS) $(LDFLAGS) -O0 -o $@ $^


clean:
	rm -rf $(call variants,$(BASEPROGRAMS)) *.o $(DEPSDIR)

.PHONY: all clean
.PRECIOUS: %-clang.o %-gcc.o %-O0.o %-clang-O0.o %-gcc-O0.o
