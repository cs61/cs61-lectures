#ifndef CS61_MINISHELL_HH
#define CS61_MINISHELL_HH
#include <cctype>
#include <vector>
#include <string>

struct shell_args {
    std::vector<const char*> args;

    inline bool empty() const;
    inline size_t size() const;
    inline char* const* argv() const;

    inline void parse(const char* str);

private:
    std::vector<std::string> strargs;
};


inline void shell_args::parse(const char* str) {
    args.clear();
    strargs.clear();
    const char* s = str;
    while (true) {
        while (isspace((unsigned char) *s)) {
            ++s;
        }
        if (*s == 0) {
            break;
        }
        const char* f = s;
        ++s;
        while (*s && !isspace((unsigned char) *s)) {
            ++s;
        }
        strargs.push_back(std::string(f, s));
    }
    for (auto& arg : strargs) {
        args.push_back(arg.c_str());
    }
    args.push_back(nullptr);
}

inline bool shell_args::empty() const {
    return args.size() <= 1;
}

inline size_t shell_args::size() const {
    auto n = args.size();
    return n - (n > 0);
}

inline char* const* shell_args::argv() const {
    return const_cast<char* const*>(args.data());
}

#endif
