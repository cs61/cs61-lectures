MMPROGRAMS = matrixmultiply $(patsubst %.cc,%,$(wildcard matrixmultiply-*.cc))
PROGRAMS = $(MMPROGRAMS)
all: $(MMPROGRAMS)

variants = $(1) $(addsuffix -unsafe,$(1)) $(addsuffix -O0,$(1))

O ?= 3
PTHREAD = 1
PIE ?= 0
SAN ?= 0
LIBS ?= -lblas
X86 ?= 0
include ../common/rules.mk
-include config.mk

UNSAFEFLAGS := -U_FORTIFY_SOURCE -fno-stack-protector $(if $(ISLINUX),-no-pie,)

%.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(O) $(DEPCFLAGS) -o $@ -c $<

%-unsafe.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(UNSAFEFLAGS) $(CXXFLAGS) -O1 $(DEPCFLAGS) -o $@ -c $<

%-O0.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O0 $(DEPCFLAGS) -o $@ -c $<

%-O1.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O1 $(DEPCFLAGS) -o $@ -c $<


$(MMPROGRAMS): %: %.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^ $(LIBS)

$(addsuffix -O0,$(MMPROGRAMS)): %: %.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -O0 -o $@ $^ $(LIBS)

$(addsuffix -unsafe,$(MMPROGRAMS)): %: %.o
	$(CXX) $(CXXFLAGS) $(UNSAFEFLAGS) $(LDFLAGS) -O1 -o $@ $^ $(LIBS)


clean:
	rm -f $(call variants,$(MMPROGRAMS)) *.o *.core
	rm -rf $(DEPSDIR) *.dSYM

.PHONY: all clean
.PRECIOUS: %-O0.o %-O1.o %-unsafe.o
