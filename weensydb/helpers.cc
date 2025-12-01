#include "helpers.hh"
#include <netdb.h>


// open_listen_socket(port)
//    Open a socket for listening on `port`. The socket will accept
//    connections from any host, and has a listen queue of 100
//    connections.

int open_listen_socket(int port) {
    // Create socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    // Useful options: close-on-exec, reuse-address
    int r = fcntl(fd, F_SETFD, FD_CLOEXEC);
    if (r < 0) {
        perror("fcntl");
        close(fd);
        return -1;
    }

    int yes = 1;
    r = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (r < 0) {
        perror("setsockopt");
        close(fd);
        return -1;
    }

    // Bind to port
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;
    r = bind(fd, (struct sockaddr*) &address, sizeof(address));
    if (r < 0) {
        perror("bind");
        close(fd);
        return -1;
    }

    // Actually start listening
    r = listen(fd, 100);
    if (r < 0) {
        perror("listen");
        close(fd);
        return -1;
    }

    return fd;
}


// raise_file_limit()
//    Attempt to raise the number of open file descriptors this
//    process is allowed.

void raise_file_limit() {
    struct rlimit rlim;
    int r = getrlimit(RLIMIT_NOFILE, &rlim);
    if (r >= 0 && rlim.rlim_cur != RLIM_INFINITY) {
        if (geteuid() == 0 && rlim.rlim_max < 10240
            && rlim.rlim_max != RLIM_INFINITY) {
            rlim.rlim_cur = rlim.rlim_max = 10240;
        } else {
            rlim.rlim_cur = rlim.rlim_max;
        }
        setrlimit(RLIMIT_NOFILE, &rlim);
    }
}


// remove_trailing_whitespace(s)
//    Modifies `s` by removing trailing whitespace. Returns
//    the length of the resulting string.

int remove_trailing_whitespace(char* s) {
    int len = strlen(s);
    while (len > 0 && isspace((unsigned char) s[len - 1])) {
        --len;
        s[len] = 0;
    }
    return len;
}


// unparse_sockaddr(addr, addrlen)
//    Return a textual description of sockaddr `addr`.

std::string unparse_sockaddr(const struct sockaddr* addr, size_t addrlen) {
    char buf[BUFSIZ], pbuf[BUFSIZ];
    if (getnameinfo(addr, addrlen, buf, sizeof(buf), pbuf, sizeof(pbuf),
                    NI_NUMERICHOST | NI_NUMERICSERV) >= 0) {
        return std::string(buf) + ":" + std::string(pbuf);
    } else {
        return "<?>";
    }
}
