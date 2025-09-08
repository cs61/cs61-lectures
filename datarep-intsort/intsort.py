import sys

ls = []
for line in sys.stdin:
    val = int(line)
    i = 0
    while i < len(ls) and ls[i] < val:
        i += 1
    ls.insert(i, val)

for v in ls:
    sys.stdout.write("{}\n".format(v))
