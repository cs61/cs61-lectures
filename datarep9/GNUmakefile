BASEPROGRAMS = attackme
PROGRAMS = attackme attackme-unsafe
all: $(PROGRAMS) attack.txt~ attackfail.txt~

variants = $(1) $(addsuffix -unsafe,$(1)) $(addsuffix -O0,$(1))


PIE ?= 0
O ?= 3
COMPILER ?= gcc
include ../common/rules.mk

UNSAFEFLAGS := -U_FORTIFY_SOURCE -fno-stack-protector $(if $(ISLINUX),-no-pie,)

%.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(O) $(DEPCFLAGS) -o $@ -c $<

%-unsafe.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(UNSAFEFLAGS) $(CXXFLAGS) -O1 $(DEPCFLAGS) -o $@ -c $<

%-O0.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O0 $(DEPCFLAGS) -o $@ -c $<

%-O1.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O1 $(DEPCFLAGS) -o $@ -c $<


$(filter-out attackme,$(BASEPROGRAMS)): %: %.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^

$(addsuffix -O0,$(BASEPROGRAMS)): %: %.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -O0 -o $@ $^

attackme: attackme-O1.o hexdump.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -O1 -o $@ $^

$(addsuffix -unsafe,$(BASEPROGRAMS)): %: %.o hexdump.o
	$(CXX) $(CXXFLAGS) $(UNSAFEFLAGS) $(LDFLAGS) -O1 -o $@ $^


attackfail.txt~: attackme makeattack.pl
	perl makeattack.pl attackme > attackfail.txt~

attack.txt~: attackme-unsafe makeattack.pl
	perl makeattack.pl attackme-unsafe > attack.txt~


clean:
	rm -rf $(call variants,$(BASEPROGRAMS)) *.o $(DEPSDIR)

.PHONY: all clean
.PRECIOUS: %-O0.o %-O1.o %-unsafe.o
