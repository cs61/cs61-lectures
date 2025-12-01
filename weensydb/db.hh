#ifndef DB_HH
#define DB_HH
#include "helpers.hh"

struct dbitem {
    std::string key;
    std::string value;
    size_t itemid;

    dbitem(const std::string& k, size_t id) {
        this->key = k;
        this->itemid = id;
    }
};

struct dbmap {
    using bucket_iterator = std::list<dbitem>::iterator;
    static constexpr size_t bucket_count = 1024;

    std::list<dbitem> buckets[bucket_count];
    std::atomic<size_t> next_itemid = 0;

    // Initialize database with a key `hello`.
    dbmap() {
        size_t b = bucket("hello");
        auto it = binsert(b, "hello");
        it->value = "world";
    }

    // Return the index of the bucket containing `key`.
    size_t bucket(const char* key) const {
        return string_hash(key) % bucket_count;
    }

    // Return an iterator to the start of bucket `b`.
    bucket_iterator bbegin(size_t b) {
        assert(b < bucket_count);
        return this->buckets[b].begin();
    }

    // Return an iterator to the end of bucket `b`.
    bucket_iterator bend(size_t b) {
        assert(b < bucket_count);
        return this->buckets[b].end();
    }

    // Return an iterator to the `dbitem` containing `key`,
    // or `this->bend(b)` if `key` is not in the database.
    // Requires that `b == this->bucket(key)`.
    [[gnu::noinline]] bucket_iterator bfind(size_t b, const char* key) {
        assert(b < bucket_count);
        auto it = this->bbegin(b), endit = this->bend(b);
        while (it != endit && it->key != key) {
            ++it;
        }
        return it;
    }

    // Insert a new item containing `key` and return an iterator to it.
    // Requires that `b == this->bucket(key)`.
    [[gnu::noinline]] bucket_iterator binsert(size_t b, const char* key) {
        assert(b < bucket_count);
        return this->buckets[b].insert(this->bend(b), dbitem(key, ++this->next_itemid));
    }

    // Remove the item pointed to by `*it`.
    // Requires that `b` is the bucket containing `it`.
    [[gnu::noinline]] bucket_iterator berase(size_t b, bucket_iterator it) {
        assert(b < bucket_count);
        return this->buckets[b].erase(it);
    }
};

#endif
