PROGRAMS = smasher
all: $(PROGRAMS) smasher.asm

include ../common/rules.mk

%.o: %.cc $(BUILDSTAMP)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPCFLAGS) $(O) -o $@ -c $<

$(PROGRAMS): \
%: %.o print_bytes.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(O) -o $@ $^

smasher.asm: smasher ../common/stripasm.pl
	objdump --disassemble=main --disassemble=_Z11string_hashPKc --disassemble=_Z8surprisev $< > $@
	objdump -d $< > $@
	$(call cleanasm,$@)
	perl ../common/stripasm.pl $@ main string_hash surprise

clean:
	rm -rf $(PROGRAMS) *.o $(DEPSDIR)

.PHONY: all clean
