PROGRAMS = ptrarith
ALLPROGRAMS = $(PROGRAMS) $(patsubst %,%-O0,$(PROGRAMS))
all: $(PROGRAMS)


PIE ?= 0
O ?= 3
include ../common/rules.mk


%.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(O) $(DEPCFLAGS) -o $@ -c $<

%-O0.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O0 $(DEPCFLAGS) -o $@ -c $<


ptrarith: ptrarith.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^

ptrarith%: ptrarith%.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^

ptrarith-O0: %: %-O0.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -O0 -o $@ $^


clean:
	rm -rf $(ALLPROGRAMS) *.o $(DEPSDIR)

.PHONY: all clean
