#ifndef HELPERS_HH
#define HELPERS_HH
#include "hexdump.hh"
#include <cassert>
#include <cctype>
#include <cerrno>
#include <fcntl.h>
#include <sched.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <list>
#include <algorithm>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// open_listen_socket(port)
//    Open a socket for listening on `port`. The socket will accept
//    connections from any host, and has a listen queue of 100
//    connections.

int open_listen_socket(int port);


// is_valid_key(s)
//    Returns true iff `s` is a valid WeensyDB key.

inline bool is_valid_key(const char* s) {
    const char* begin = s;
    while (*s && !isspace((unsigned char) *s)) {
        ++s;
    }
    return *s == 0 && s != begin;
}


// string_hash(s)
//    Returns a hashcode for the string `s`. This hashcode has
//    the property that identical strings have identical
//    hashes. The current implementation is not complicated :)

inline unsigned string_hash(const char* s) {
    return (unsigned char) *s;
}


// remove_trailing_whitespace(s)
//    Modifies `s` by removing trailing whitespace. Returns
//    the length of the resulting string.

int remove_trailing_whitespace(char* s);


// raise_file_limit()
//    Attempt to raise the number of open file descriptors this
//    process is allowed.

void raise_file_limit();


// make_nonblocking(fd)
//    Make file descriptor `fd` nonblocking: attempts to read
//    from `fd` will fail with errno EWOULDBLOCK if no data is
//    available, and attempts to write to `fd` will fail with
//    errno EWOULDBLOCK if no space is available. Not all file
//    descriptors can be made nonblocking, but pipes and network
//    sockets can.

inline int make_nonblocking(int fd) {
    return fcntl(fd, F_SETFL, O_NONBLOCK);
}


// unparse_sockaddr(addr, addrlen)
//    Return a textual description of sockaddr `addr`.

std::string unparse_sockaddr(const struct sockaddr* addr, size_t addrlen);

#endif
