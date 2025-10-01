#! /bin/sh

# Look for gcc-{14,9}; prefer gcc-12 or above
wantv=12

ver1 () {
    ( $1 -dumpversion 2>/dev/null || echo 0 ) | sed 's/[^0-9].*//'
}

prog="$1"
if "$prog" -v 2>&1 | grep '^gcc' >/dev/null; then
    base=`echo "$prog" | sed 's/^\([^g]\)/g\1/'`
    v=`ver1 "$prog"`
    if [ $v -lt $wantv ]; then
        for vx in 14 13 12 11 10 9; do
            if [ `ver1 "$base-$vx"` -ge $wantv ]; then
                echo "$base-$vx"; exit
            fi
        done
    fi
fi
echo "$prog"
