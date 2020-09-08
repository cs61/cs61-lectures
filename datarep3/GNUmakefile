BASEPROGRAMS = array0 array1 struct0 struct1 struct2 struct3 \
	union0 localvar0
PROGRAMS = $(BASEPROGRAMS)
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
