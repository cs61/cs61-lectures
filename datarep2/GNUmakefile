BASEPROGRAMS = mexplore0 mexplore1 mexplore2 mexplore3 mexplore4
PROGRAMS = $(BASEPROGRAMS) mexplore2-O0 mexplore3-O0 mexplore4-O0
all: $(PROGRAMS)

ALLPROGRAMS = $(BASEPROGRAMS) $(wildcard $(patsubst %,%-O?,$(BASEPROGRAMS)))

COMPILER ?= clang
include ../common/rules.mk

%.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(O) $(DEPCFLAGS) -o $@ -c $<

%-O0.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O0 $(DEPCFLAGS) -o $@ -c $<

%-O2.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O2 $(DEPCFLAGS) -o $@ -c $<

$(BASEPROGRAMS): %: %.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^

$(patsubst %,%-O0,$(BASEPROGRAMS)): %: %.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^

$(patsubst %,%-O2,$(BASEPROGRAMS)): %: %.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -O3 -o $@ $^

clean:
	rm -rf $(ALLPROGRAMS) *.o $(DEPSDIR)

.PHONY: all clean
