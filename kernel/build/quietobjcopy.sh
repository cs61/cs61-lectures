#! /bin/sh

onexit () {
    test -n "$tmp" && rm -f "$tmp"
}
trap onexit 0
tmp=`mktemp /tmp/objcopy.XXXXXX`
"$@" 2> $tmp
v=$?
cat $tmp | grep -v "empty loadable segment" 1>&2
exit $v
