PROGRAMS = badallocs integertrailer useafterfree
ALLPROGRAMS = $(PROGRAMS)
all: $(PROGRAMS)


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

$(PROGRAMS) $(addsuffix -O0,$(PROGRAMS)) \
$(addsuffix -clang,$(PROGRAMS)) $(addsuffix -clang-O0,$(PROGRAMS)) \
$(addsuffix -gcc,$(PROGRAMS)) $(addsuffix -gcc-O0,$(PROGRAMS)): %: %.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^


clean:
	rm -rf $(PROGRAMS) $(patsubst %,%-O0,$(PROGRAMS)) \
	  $(patsubst %,%-clang,$(PROGRAMS)) $(patsubst %,%-gcc,$(PROGRAMS)) \
	  $(patsubst %,%-clang-O0,$(PROGRAMS)) $(patsubst %,%-gcc-O0,$(PROGRAMS)) \
	  *.o $(DEPSDIR)

.PHONY: all clean
.PRECIOUS: %-clang.o %-gcc.o %-clang-O0.o %-gcc-O0.o
