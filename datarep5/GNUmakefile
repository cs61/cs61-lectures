MB_PROGRAMS = $(patsubst mb%.cc,mb%,$(sort $(wildcard mb[0-9].cc)))
MB_OTHERMALLOC_PROGRAMS :=
ifneq ($(findstring /,$(shell whereis libjemalloc 2>/dev/null)),)
MB_OTHERMALLOC_PROGRAMS += mb0-jemalloc
endif
ifneq ($(findstring /,$(shell whereis libtcmalloc 2>/dev/null)),)
MB_OTHERMALLOC_PROGRAMS += mb0-tcmalloc
TCMALLOCLIB = -ltcmalloc
else ifneq ($(findstring /,$(shell whereis libtcmalloc.so.4 2>/dev/null)),)
MB_OTHERMALLOC_PROGRAMS += mb0-tcmalloc
TCMALLOCLIB = -l:libtcmalloc.so.4
endif

PROGRAMS = $(MB_PROGRAMS) $(MB_OTHERMALLOC_PROGRAMS)
ALLPROGRAMS = $(MB_PROGRAMS) $(patsubst %,%-O0,$(MB_PROGRAMS)) \
	mb0-jemalloc mb0-tcmalloc mb0-jemalloc-O0 mb0-tcmalloc-O0
all: $(PROGRAMS)


PIE ?= 0
O ?= 3
COMPILER ?= clang
PTHREAD = 1
include ../common/rules.mk


%.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(O) $(DEPCFLAGS) -o $@ -c $<

%-O0.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O0 $(DEPCFLAGS) -o $@ -c $<


mb%: mb%.o mb-driver.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^

mb%-O0: mb-%-O0.o mb-driver.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -O0 -o $@ $^

mb%-tcmalloc: mb%.o mb-driver.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^ $(TCMALLOCLIB)

mb%-jemalloc: mb%.o mb-driver.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^ -ljemalloc


clean:
	rm -rf $(ALLPROGRAMS) *.o $(DEPSDIR)

.PHONY: all clean
