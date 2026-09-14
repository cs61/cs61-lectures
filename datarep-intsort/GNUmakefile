PROGRAMS = intgen arraysum \
	intsort intsort-binsearch intsort-list intsort-multiset intsort-vector
all: $(PROGRAMS)
X86 ?= 0

include ../common/rules.mk

%.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPCFLAGS) $(O) -o $@ -c $<

$(PROGRAMS): \
%: %.o print_bytes.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^


clean:
	rm -rf $(PROGRAMS) $(wildcard intsort[1-6] intsort-tree) *.o $(DEPSDIR)

.PHONY: all clean
