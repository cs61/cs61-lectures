PROGRAMS = opbench ubsignedinc ubsignedloop ubtable
all: $(PROGRAMS)

include ../common/rules.mk

%.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPCFLAGS) $(O) -o $@ -c $<

$(PROGRAMS): \
%: %.o print_bytes.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^


clean:
	rm -rf $(PROGRAMS) *.o $(DEPSDIR)

.PHONY: all clean
