PROGRAMS = forkmix syscall-forkmix forkorder minishell \
	makezombies pipesizer selfpipe childpipe waitdemo \
	racer-poll racer-block racer-blockvar racer-selfpipe \
	racer-alarm racer-selectpipe racer-waitblockwait

all: $(PROGRAMS)

ALLPROGRAMS = $(PROGRAMS) matrixmultiply-blas
CONFIGMK ?= config.mk

O ?= 3
include ../common/rules.mk
-include $(CONFIGMK)

%.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPCFLAGS) $(O) -o $@ -c $<

processex fork1 fork2 fork3 forkmix syscall-forkmix forkorder minishell \
makezombies pipesizer selfpipe childpipe waitdemo: \
%: %.o print_bytes.o
	$(CXX) $(CXXFLAGS) $(O) $(LDFLAGS) -o $@ $^ $(LIBS)

racer-poll racer-block racer-blockvar racer-selfpipe \
racer-alarm racer-selectpipe racer-signalfd racer-threads racer-waitblockwait: \
%: %.o racer.o print_bytes.o
	$(CXX) $(CXXFLAGS) $(O) $(LDFLAGS) -o $@ $^ $(LIBS)


clean:
	rm -f $(ALLPROGRAMS) *.o
	rm -rf $(DEPSDIR) *.dSYM

.PHONY: all always clean
