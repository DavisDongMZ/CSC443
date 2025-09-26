//
// Created by Zekun Liu on 2025-09-23.
//

#ifndef KVDATABASE_KEY_VALUE_PAIR_H
#define KVDATABASE_KEY_VALUE_PAIR_H

#include <string>
#include <utility>

struct KV {
    std::string key;
    std::string value;

    KV(std::string  k, std::string  v) : key(std::move(k)), value(std::move(v)) {}
};

#endif //KVDATABASE_KEY_VALUE_PAIR_H