PROGRAMS = add $(patsubst addf%.cc,add%,$(wildcard addf[0-9][0-9].cc))
all: $(PROGRAMS)

include ../common/rules.mk


# Rules for making object files (i.e., parts of executables)
# from source files

%.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPCFLAGS) $(O) -o $@ -c $<

addf.o: addf.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPCFLAGS) $(O) -fomit-frame-pointer -fcf-protection=none -o $@ -c $<

addf%.o: addf%.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPCFLAGS) $(O) -fomit-frame-pointer -fcf-protection=none -o $@ -c $<


# Rules for making executables (runnable programs) from object files

add: add.o addf.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^

add%: add.o addf%.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^


clean:
	rm -rf $(ALLPROGRAMS) *.o $(DEPSDIR)

.PHONY: all clean
.PRECIOUS: addf%.o
