#include "helpers.hh"
#include <netdb.h>
#include <vector>

int main(int argc, char** argv) {
    const char* host = "localhost";
    const char* port = "6162";
    size_t max_conns = 10000;
    raise_file_limit();

    int opt;
    while ((opt = getopt(argc, argv, "h:p:n:")) >= 0) {
        if (opt == 'h') {
            host = optarg;
        } else if (opt == 'p') {
            port = optarg;
        } else if (opt == 'n') {
            max_conns = strtoul(optarg, nullptr, 0);
        }
    }
    if (max_conns == 0) {
        fprintf(stderr, "./wdbblaster: -n must be > 0\n");
        exit(1);
    }

    // look up host and port
    struct addrinfo hints, *ais;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;        // use IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;    // use TCP
    hints.ai_flags = AI_NUMERICSERV;
    if (strcmp(host, "cs61") == 0) {
        host = "cs61.seas.harvard.edu";
    }
    int r = getaddrinfo(host, port, &hints, &ais);
    if (r != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(r));
        exit(1);
    }

    // open first connection
    (void) elapsed_time();
    auto ai = ais;
    int fd = -1;
    while (ai && fd < 0) {
        fd = socket(ai->ai_family, ai->ai_socktype, 0);
        if (fd < 0) {
            perror("socket");
            exit(1);
        }

        r = connect(fd, ai->ai_addr, ai->ai_addrlen);
        if (r < 0) {
            close(fd);
            fd = -1;
            ai = ai->ai_next;
        }
    }
    if (fd < 0) {
        perror("connect");
        exit(1);
    }

    // open further connections
    std::vector<int> cfds;
    cfds.push_back(fd);
    while (cfds.size() != max_conns) {
        fd = socket(ai->ai_family, ai->ai_socktype, 0);
        if (fd < 0) {
            perror("socket");
            break;
        }

        r = connect(fd, ai->ai_addr, ai->ai_addrlen);
        if (r < 0) {
            perror("connect");
            break;
        }

        cfds.push_back(fd);
        fprintf(stderr, "\r%zu/%zu connections (%g conns/s)    ",
                cfds.size(), max_conns, cfds.size() / elapsed_time());
    }

    fprintf(stderr, "\rHolding open %zu connections\n", cfds.size());
    while (true) {
        usleep(1000000);
    }
}
