#! /usr/bin/perl -w

open(my $fh, "+<", $ARGV[0]);
my($s) = "";
my($mode) = 1;
while (defined($_ = <$fh>)) {
	if (/^[0-9a-f]+ <(.*?)>/) {
		my($f) = $1;
		$mode = 0;
		for (my $i = 1; $i < @ARGV; ++$i) {
			if (index($f, $ARGV[$i]) >= 0) {
				$mode = 1;
			}
		}
	} elsif (!/^ +[0-9a-f]+:/) {
		$mode = 1;
	}
	$s .= $_ if $mode == 1 && ($_ ne "\n" || length($s) < 2 || substr($s, -2) ne "\n\n");
}
seek($fh, 0, 0);
print $fh $s;
truncate($fh, length($s));
