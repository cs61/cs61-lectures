ADDPROGRAMS = $(patsubst addf%.cc,add%,$(wildcard addf[0-9][0-9].cc))
PROGRAMS = add addu addv $(ADDPROGRAMS)
all: $(PROGRAMS)

include ../common/rules.mk

%.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPCFLAGS) $(O) -o $@ -c $<

addu addv: \
%: %.o print_bytes.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^

add: \
%: %.o addf.o print_bytes.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^

add01 add02 add03 add04 add05 add06 add07 add08 add09: \
add%: add.o addf%.o print_bytes.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^

clean:
	rm -rf $(PROGRAMS) $(wildcard intsort[1-6]) *.o $(DEPSDIR)

.PHONY: all clean
