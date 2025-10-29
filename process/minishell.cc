#include "print_bytes.hh"
#include "minishell.hh"
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <cctype>

int main() {
    while (true) {
        // print prompt
        fprintf(stderr, "$ ");

        // read command line
        char buf[BUFSIZ];
        if (fgets(buf, BUFSIZ, stdin) == nullptr) {
            fprintf(stderr, "\n");
            exit(0);
        }

        // parse into words
        shell_args shargs;
        shargs.parse(buf);
        if (shargs.empty()) {
            continue;
        }
        fprintf(stderr, "%zu arg:", shargs.size());
        for (size_t i = 0; i != shargs.size(); ++i) {
            fprintf(stderr, " %s", shargs.args[i]);
        }
        fprintf(stderr, "\n");

        // execute command
        fprintf(stderr, "??? %s\n", buf);
    }
}
