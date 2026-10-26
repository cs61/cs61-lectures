#include "db.hh"

std::mutex dbm[256];
dbmap db;
bool verbose = true;
std::mutex tcm;
std::atomic<int> thctr = 0;
std::condition_variable_any thcv;


void handle_connection(int cfd, std::string remote) {
    FILE* fin = fdopen(cfd, "r");
    FILE* f = fdopen(cfd, "w");

    char buf[BUFSIZ], key[BUFSIZ];
    size_t sz;

    while (fgets(buf, BUFSIZ, fin)) {
        if (sscanf(buf, "GET %s ", key) == 1) {
            auto b = db.bucket(key);
            std::unique_lock<std::mutex> guard(dbm[b]);
            // find item
            auto it = db.bfind(b, key);

            // print value
            if (it != db.bend(b)) {
                fprintf(f, "VALUE %s %zu #%zu\r\n",
                        key, it->value.length(), it->itemid);
                fwrite(it->value.data(), 1, it->value.length(), f);
                fprintf(f, "\r\n");
            }
            fprintf(f, "END\r\n");
            fflush(f);

        } else if (sscanf(buf, "SET %s %zu ", key, &sz) == 2) {
            std::string value;
            value.assign(sz, '\0');
            size_t nr = fread(value.data(), 1, sz, fin);
            assert(nr <= sz);

            auto b = db.bucket(key);
            std::unique_lock<std::mutex> guard(dbm[b]);
            // find item; insert if missing
            auto it = db.bfind(b, key);
            if (it == db.bend(b)) {
                it = db.binsert(b, key);
            }

            // set value
            it->value = value;
            fprintf(f, "STORED #%zu\r\n", it->itemid);
            fflush(f);
            guard.unlock();

        } else if (sscanf(buf, "DELETE %s ", key) == 1) {
            // find item
            auto b = db.bucket(key);
            std::unique_lock<std::mutex> guard(dbm[b]);
            auto it = db.bfind(b, key);

            // remove if found
            if (it != db.bend(b)) {
                auto itemid = it->itemid;
                db.berase(b, it);
                fprintf(f, "DELETED #%zu\r\n", itemid);
            } else {
                fprintf(f, "NOT_FOUND\r\n");
            }
            fflush(f);

        } else if (buf[0] == 'Q') {
            break;

        } else if (remove_trailing_whitespace(buf)) {
            fprintf(f, "ERROR\r\n");
            fflush(f);
        }

        if (verbose && remove_trailing_whitespace(buf)) {
            fprintf(stderr, "%s: %s\n", remote.c_str(), buf);
        }
    }

    if (ferror(fin)) {
        perror("read");
    }
    fclose(fin); // also closes `f`'s underlying fd
    (void) fclose(f);
    std::unique_lock thguard(tcm);
    --thctr;
    thcv.notify_all();
}


int main(int argc, char** argv) {
    // Usage: ./weensydb [PORT]
    int port = 6162;
    if (argc >= 2) {
        port = strtol(argv[1], nullptr, 0);
        assert(port > 0 && port <= 65535);
    }

    // Prepare listening socket
    int fd = open_listen_socket(port);
    assert(fd >= 0);
    fprintf(stderr, "Listening on port %d...\n", port);

    while (true) {
        struct sockaddr addr;
        socklen_t addrlen = sizeof(addr);

        std::unique_lock thguard(tcm);
        while (thctr >= 256) {
            thcv.wait(tcm);
        }
        ++thctr;
        thguard.unlock();

        // Accept connection on listening socket
        int cfd = accept(fd, &addr, &addrlen);
        if (cfd < 0) {
            perror("accept");
            exit(1);
        }

        // Handle connection
        std::thread th(handle_connection, cfd, unparse_sockaddr(&addr, addrlen));
        th.detach();
        tcm.lock();
    }
}
