#include "HashTable.h"
#include <algorithm>

HashTable::HashTable(size_t size) noexcept 
    : _capacity(size), _filled(0) {
    table.resize(_capacity);
}

HashTable::~HashTable() {
    for (auto& chain : table) {
        chain.clear();
    }
    table.clear();
}

size_t HashTable::hash_function(const KeyType &key) const {
    size_t hash = 0;
    for (char c : key) {
        hash = hash * 256 + static_cast<unsigned char>(c);
    }
    return hash % _capacity;
}


void HashTable::insert(const KeyType &key, const ValueType &value) {
    if (getLoadFactor() > 0.75) {
        _capacity *= 2;
        std::vector<std::list<std::pair<KeyType, ValueType>>> newTable(_capacity);
        
        for (const auto& chain : table) {
            for (const auto& pair : chain) {
                size_t newHash = hash_function(pair.first);
                newTable[newHash].push_back(pair);
            }
        }
        table = std::move(newTable);
    }

    size_t index = hash_function(key);
    auto& chain = table[index];

    auto it = std::find_if(chain.begin(), chain.end(), 
        [&key](const std::pair<KeyType, ValueType>& pair) {
            return pair.first == key;
        });

    if (it != chain.end()) {
        it->second = value;
    } 

	else {
        chain.emplace_back(key, value);
        _filled++;
    }
}

bool HashTable::find(const KeyType &key, ValueType &value) const {
    size_t index = hash_function(key);
    const auto& chain = table[index];

    auto it = std::find_if(chain.begin(), chain.end(), 
        [&key](const std::pair<KeyType, ValueType>& pair) {
            return pair.first == key;
        });

    if (it != chain.end()) {
        value = it->second;
        return true;
    }
    return false;
}

void HashTable::remove(const KeyType &key) {
    size_t index = hash_function(key);
    auto& chain = table[index];

    auto it = std::find_if(chain.begin(), chain.end(), 
        [&key](const std::pair<KeyType, ValueType>& pair) {
            return pair.first == key;
        });

    if (it != chain.end()) {
        chain.erase(it);
        _filled--;
    }
}

ValueType& HashTable::operator[](const KeyType &key) {
    size_t index = hash_function(key);
    auto& chain = table[index];

    auto it = std::find_if(chain.begin(), chain.end(), 
        [&key](const std::pair<KeyType, ValueType>& pair) {
            return pair.first == key;
        });

    if (it != chain.end()) {
        return it->second;
    } else {
        chain.emplace_back(key, ValueType());
        _filled++;
        return chain.back().second;
    }
}

double HashTable::getLoadFactor() {
    return static_cast<double>(_filled) / _capacity;
}

