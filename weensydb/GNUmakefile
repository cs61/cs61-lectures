DB_PROGRAMS = $(sort $(patsubst %.cc,%,$(wildcard weensydb[0-9][0-9].cc)))
BASEPROGRAMS = wdbclient wdbclientloop wdbblaster $(DB_PROGRAMS)

all: $(BASEPROGRAMS)

ALLPROGRAMS = $(BASEPROGRAMS)

variants = $(1) $(addsuffix -noopt,$(1)) \
	$(addsuffix -clang,$(1)) $(addsuffix -clang-noopt,$(1)) \
	$(addsuffix -gcc,$(1)) $(addsuffix -gcc-noopt,$(1))

O ?= 3
PTHREAD ?= 1
include ../common/rules.mk
-include $(CONFIGMK)

%.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPCFLAGS) $(O) -o $@ -c $<

%-clang.o: %.cc $(BUILDSTAMP)
	clang++ $(CPPFLAGS) $(CXXFLAGS) $(DEPCFLAGS) $(O) -o $@ -c $<

%-gcc.o: %.cc $(BUILDSTAMP)
	$(CXX_GCC) $(CPPFLAGS) $(CXXFLAGS) $(DEPCFLAGS) $(O) -o $@ -c $<

%-noopt.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPCFLAGS) -O0 -o $@ -c $<

%-clang-noopt.o: %.cc $(BUILDSTAMP)
	clang++ $(CPPFLAGS) $(CXXFLAGS) $(DEPCFLAGS) -O0 -o $@ -c $<

%-gcc-noopt.o: %.cc $(BUILDSTAMP)
	$(CXX_GCC) $(CPPFLAGS) $(CXXFLAGS) $(DEPCFLAGS) -O0 -o $@ -c $<

$(PROGRAMS): \
%: %.o helpers.o
	$(CXX) $(CXXFLAGS) $(O) $(LDFLAGS) -o $@ $^ $(LIBS)


# Rules for making executables (runnable programs) from object files

$(BASEPROGRAMS): \
%: %.o helpers.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^ $(LIBS)

$(addsuffix -clang,$(BASEPROGRAMS)): \
%: %.o helpers-clang.o
	clang++ $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^ $(LIBS)

$(addsuffix -gcc,$(BASEPROGRAMS)): \
%: %.o helpers-gcc.o
	$(CXX_GCC) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^ $(LIBS)

$(addsuffix -noopt,$(BASEPROGRAMS)): \
%: %.o helpers-noopt.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -O0 -o $@ $^ $(LIBS)

$(addsuffix -gcc-noopt,$(BASEPROGRAMS)): \
%: %.o helpers-gcc-noopt.o
	$(CXX_GCC) $(CXXFLAGS) $(LDFLAGS) -O0 -o $@ $^ ($LIBS)

$(addsuffix -clang-noopt,$(BASEPROGRAMS)): \
%: %.o helpers-clang-noopt.o
	clang++ $(CXXFLAGS) $(LDFLAGS) -O0 -o $@ $^ $(LIBS)


clean:
	rm -f $(call variants,$(ALLPROGRAMS)) *.o
	rm -rf $(DEPSDIR) *.dSYM

.PHONY: all always clean
.PRECIOUS: %-noopt.o %-clang.o %-gcc.o %-clang-noopt.o %-gcc-noopt.o
