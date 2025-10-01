BEGIN {
    ORS = "";
    print "#define foreach_builtin_process_image(x)";
    n = split(processes, a, /[ \t]+/);
    for (i = 1; i <= n; ++i) {
        gsub(/^p-/, "", a[i]);
        if (match(a[i], /^[_A-Za-z][_A-Za-z0-9]*$/)) {
            print " x(", a[i], ")";
        }
    }
    print "\n";
    exit;
}
