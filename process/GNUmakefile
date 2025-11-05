PROGRAMS = forkmix syscall-forkmix forkorder manyfork minishell \
	myecho execmyecho spawnmyecho \
	makezombies pipesizer selfpipe childpipe waitdemo
RACERPROGRAMS = racer-poll racer-block racer-blockvar racer-selfpipe \
	racer-alarm racer-selectpipe racer-waitblockwait

all: $(PROGRAMS) $(RACERPROGRAMS)

ALLPROGRAMS = $(PROGRAMS) $(RACERPROGRAMS)
CONFIGMK ?= config.mk

O ?= 3
include ../common/rules.mk
-include $(CONFIGMK)

%.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPCFLAGS) $(O) -o $@ -c $<

$(PROGRAMS): \
%: %.o print_bytes.o
	$(CXX) $(CXXFLAGS) $(O) $(LDFLAGS) -o $@ $^ $(LIBS)

$(RACERPROGRAMS) racer-signalfd racer-threads: \
%: %.o racer.o print_bytes.o
	$(CXX) $(CXXFLAGS) $(O) $(LDFLAGS) -o $@ $^ $(LIBS)


clean:
	rm -f $(ALLPROGRAMS) *.o
	rm -rf $(DEPSDIR) *.dSYM

.PHONY: all always clean
