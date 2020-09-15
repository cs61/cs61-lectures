BASEPROGRAMS = simplealloc divzero ubsignedinc
HEXDUMPPROGRAMS = divzero ubsignedinc
PROGRAMS = simplealloc-O0 simplealloc divzero \
	ubsignedinc-O0 ubsignedinc
ALLPROGRAMS = $(PROGRAMS) $(patsubst %,%-O0,$(PROGRAMS))
all: $(PROGRAMS)


COMPILER ?= clang
PIE ?= 0
O ?= 3
include ../common/rules.mk


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

$(BASEPROGRAMS) $(addsuffix -O0,$(BASEPROGRAMS)) \
$(addsuffix -clang,$(BASEPROGRAMS)) $(addsuffix -clang-O0,$(BASEPROGRAMS)) \
$(addsuffix -gcc,$(BASEPROGRAMS)) $(addsuffix -gcc-O0,$(BASEPROGRAMS)): %: %.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^


clean:
	rm -rf $(BASEPROGRAMS) $(patsubst %,%-O0,$(BASEPROGRAMS)) \
	  $(patsubst %,%-clang,$(BASEPROGRAMS)) $(patsubst %,%-gcc,$(BASEPROGRAMS)) \
	  $(patsubst %,%-clang-O0,$(BASEPROGRAMS)) $(patsubst %,%-gcc-O0,$(BASEPROGRAMS)) \
	  *.o $(DEPSDIR)

.PHONY: all clean
.PRECIOUS: %-clang.o %-gcc.o %-clang-O0.o %-gcc-O0.o
