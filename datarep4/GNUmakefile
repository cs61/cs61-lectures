BASEPROGRAMS = arraysum listinsert vectorinsert vectorinsert-binsearch
all: $(BASEPROGRAMS)

ALLPROGRAMS = $(BASEPROGRAMS) $(patsubst %,%-O0,$(BASEPROGRAMS))

include ../common/rules.mk

%.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(O) $(DEPCFLAGS) -o $@ -c $<

%-O0.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O0 $(DEPCFLAGS) -o $@ -c $<

%-O2.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O2 $(DEPCFLAGS) -o $@ -c $<

$(BASEPROGRAMS): %: %.o arrayinfo.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^

$(patsubst %,%-O0,$(BASEPROGRAMS)): %: %.o arrayinfo.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^


clean:
	rm -rf $(ALLPROGRAMS) *.o $(DEPSDIR)

.PHONY: all clean
