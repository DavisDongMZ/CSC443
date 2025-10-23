#include "sstable.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdexcept>
#include <algorithm>

// Helper: append file extension to a base path.
// Example: "sst_000001" + ".sst" -> "sst_000001.sst"
static std::string with_ext(const std::string& base, const char* ext) {
    return base + ext;
}

// Constructor: initialize data and index file paths.
SSTable::SSTable(std::string base_no_ext) {
    data_path  = with_ext(base_no_ext, ".sst");
    index_path = with_ext(base_no_ext, ".idx");
}

// Write a 32-bit unsigned integer (binary form) into file
void SSTable::write_u32(int fd, uint32_t v) {
    if (::write(fd, &v, sizeof(v)) != (ssize_t)sizeof(v)) throw std::runtime_error("write_u32");
}
// Write a 64-bit unsigned integer (binary form) into file.
void SSTable::write_u64(int fd, uint64_t v) {
    if (::write(fd, &v, sizeof(v)) != (ssize_t)sizeof(v)) throw std::runtime_error("write_u64");
}
// Read a 32-bit unsigned integer from file at given offset.
uint32_t SSTable::read_u32_at(int fd, uint64_t off) {
    uint32_t v{};
    if (::pread(fd, &v, sizeof(v), off) != (ssize_t)sizeof(v)) throw std::runtime_error("pread u32");
    return v;
}
// Read a 64-bit unsigned integer from file at given offset.
uint64_t SSTable::read_u64_at(int fd, uint64_t off) {
    uint64_t v{};
    if (::pread(fd, &v, sizeof(v), off) != (ssize_t)sizeof(v)) throw std::runtime_error("pread u64");
    return v;
}

// SSTable::build()
// Build an immutable SSTable on disk from sorted key-value pairs.
// Writes two files:
//   - data file (.sst): stores actual key/value records
//   - index file (.idx): stores key -> offset mappings
// Returns an SSTable object ready for reading.
SSTable SSTable::build(const std::string& base_no_ext,
                       const std::vector<std::pair<std::string, std::string>>& sorted_kv) {
    SSTable t(base_no_ext);

    // Open data and index files for writing
    int dfd = ::open(t.data_path.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (dfd < 0) throw std::runtime_error("open data for write failed");
    int ifd = ::open(t.index_path.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (ifd < 0) { ::close(dfd); throw std::runtime_error("open index for write failed"); }

    // Write the total number of entries into the index file
    uint32_t n = (uint32_t)sorted_kv.size();
    write_u32(ifd, n);

    // Keep track of byte offset within the data file
    uint64_t off = 0;
    for (auto& kv : sorted_kv) {
        const std::string& k = kv.first;
        const std::string& v = kv.second;
        uint32_t klen = (uint32_t)k.size(), vlen = (uint32_t)v.size();

        // index Format: [key_len][key_bytes][offset]
        write_u32(ifd, klen);
        if (::write(ifd, k.data(), klen) != (ssize_t)klen) { ::close(ifd); ::close(dfd); throw std::runtime_error("write idx key"); }
        write_u64(ifd, off);

        // data Format: [key_len][val_len][key_bytes][val_bytes]
        write_u32(dfd, klen);
        write_u32(dfd, vlen);
        if (::write(dfd, k.data(), klen) != (ssize_t)klen) { ::close(ifd); ::close(dfd); throw std::runtime_error("write data key"); }
        if (::write(dfd, v.data(), vlen) != (ssize_t)vlen) { ::close(ifd); ::close(dfd); throw std::runtime_error("write data val"); }

        // Update next offset in data file
        off += sizeof(uint32_t)*2 + klen + vlen;
    }
    ::close(ifd);
    ::close(dfd);

    // Open table for reading (load index into memory)
    t.open();
    return t;
}

void SSTable::open() {
    int ifd = ::open(index_path.c_str(), O_RDONLY);
    if (ifd < 0) throw std::runtime_error("open index for read failed");

    // Read number of entries
    uint32_t n{};
    if (::read(ifd, &n, sizeof(n)) != (ssize_t)sizeof(n)) { ::close(ifd); throw std::runtime_error("read n"); }

    // Read index entries one by one
    index.clear(); index.reserve(n);
    for (uint32_t i=0;i<n;++i) {
        uint32_t klen{};
        if (::read(ifd, &klen, sizeof(klen)) != (ssize_t)sizeof(klen)) { ::close(ifd); throw std::runtime_error("read klen"); }
        std::string k(klen, '\0');
        if (::read(ifd, k.data(), klen) != (ssize_t)klen) { ::close(ifd); throw std::runtime_error("read key"); }
        uint64_t off{};
        if (::read(ifd, &off, sizeof(off)) != (ssize_t)sizeof(off)) { ::close(ifd); throw std::runtime_error("read off"); }
        index.push_back({std::move(k), off});
    }
    ::close(ifd);

    data_fd = ::open(data_path.c_str(), O_RDONLY);
    if (data_fd < 0) throw std::runtime_error("open data for read failed");
}

void SSTable::close() {
    if (data_fd >= 0) { ::close(data_fd); data_fd = -1; }
    index.clear();
}

// SSTable::read_record_at()
// --------------------------------------------------------------------
// Given an offset, read one complete key-value record from data file.
// Record format: [key_len][val_len][key_bytes][val_bytes]
bool SSTable::read_record_at(int fd, uint64_t off, std::string& k, std::string& v) {
    uint32_t klen = read_u32_at(fd, off);
    uint32_t vlen = read_u32_at(fd, off + sizeof(uint32_t));
    k.resize(klen); v.resize(vlen);
    if (::pread(fd, k.data(), klen, off + sizeof(uint32_t)*2) != (ssize_t)klen) return false;
    if (::pread(fd, v.data(), vlen, off + sizeof(uint32_t)*2 + klen) != (ssize_t)vlen) return false;
    return true;
}

// SSTable::get()
// --------------------------------------------------------------------
// Binary-search the in-memory index to find key, then read the record
// from the data file using its offset. Returns true if found.
bool SSTable::get(const std::string& key, std::string& value_out) const {
    if (index.empty() || data_fd < 0) return false;
    auto it = std::lower_bound(index.begin(), index.end(), key,
        [](const SSTIndexEntry& e, const std::string& k){ return e.key < k; });
    if (it == index.end() || it->key != key) return false;

    std::string k, v;
    if (!read_record_at(data_fd, it->offset, k, v)) return false;
    if (k != key) return false;
    value_out = std::move(v);
    return true;
}

// SSTable::scan()
// --------------------------------------------------------------------
// Range scan from 'start' to 'end' (inclusive).
// Iterates through index and invokes user-supplied callback 'visit'.
void SSTable::scan(const std::string& start, const std::string& end,
                   const std::function<void(const std::string&, const std::string&)>& visit) const {
    if (index.empty() || data_fd < 0) return;
    auto it = std::lower_bound(index.begin(), index.end(), start,
        [](const SSTIndexEntry& e, const std::string& k){ return e.key < k; });

    for (; it != index.end(); ++it) {
        if (it->key > end) break;
        std::string k, v;
        if (!read_record_at(data_fd, it->offset, k, v)) break;
        if (k < start) continue;
        if (k > end) break;
        visit(k, v);
    }
}
