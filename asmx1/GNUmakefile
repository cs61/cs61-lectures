PROGRAMS = $(sort $(patsubst src/fun%.cc,fun%,$(wildcard src/fun*.cc)) $(patsubst fun%.s,fun%,$(wildcard fun[0-9][0-9].s)))
ALLPROGRAMS = $(PROGRAMS)
FULLSOURCES = $(patsubst src/fun%.cc,fun%.s,$(sort $(wildcard src/fun*.cc)))

all: $(PROGRAMS) $(FULLSOURCES)

O = 2
PIE ?= 0
include ../common/rules.mk

%.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(filter-out -g,$(CXXFLAGS)) $(O) $(DEPCFLAGS) -o $@ -c $<

%.o: src/%.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(filter-out -g,$(CXXFLAGS)) $(O) $(DEPCFLAGS) -o $@ -c $<

%.s: src/%.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(filter-out -g,$(CXXFLAGS)) $(O) $(DEPCFLAGS) -o $@ -S $<
	@$(call xrun,$(call cleanasm,$@),CLEANASM $@)


%.o: %.s $(BUILDSTAMP)
	$(CXX) -o $@ -c $<

fun%: fun%.o 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^


clean:
	rm -rf $(DEPSDIR)
	rm -f $(ALLPROGRAMS) *.o $(FULLSOURCES)

asm: $(FULLSOURCES)

.PHONY: all clean asm
