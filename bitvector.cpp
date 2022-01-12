#ifndef BITVECTOR
#define BITVECTOR

#include <iostream>
#include <cstdint>
#include <bit>
#include <bitset>
#include <cassert>
#include <vector>
#include <x86intrin.h>

class Bitvector final {
private:
    std::vector<uint64_t> data;
    uint64_t _size;

public:
    Bitvector ();
    uint64_t rank (const uint64_t i) const;
    uint64_t select (const uint64_t j) const;
    void push_back (const uint64_t bit);
    void print (void) const;
    uint64_t size (void) const;
    bool access (const uint64_t i) const;
    uint64_t leading_ones (const uint64_t i) const;

};

Bitvector::Bitvector()
    : data(), _size(0)
{}

uint64_t Bitvector::rank (const uint64_t i) const {
    assert (i <= _size);
    
    uint64_t count = 0;
    const uint64_t index = i / 64;
    for (uint64_t j = 0; j < index; j++) {
        count += std::popcount(data[j]);
    }
    return count + std::popcount( data[index] & ((1ULL << (i%64)) - 1) );
}

uint64_t Bitvector::select (const uint64_t j) const {
    assert (j < _size);
    uint64_t count = 0;
    size_t index = 0;
    // std::cout << "finding: " << j+1 << '\n';
    auto pc = std::popcount(data[index]);
    while (index+1 < data.size() && count + pc <= j) {
        count += pc;
        pc = std::popcount(data[++index]);
    }
    // std::cout << "index: " << index << "\n";
    return (index << 6) + std::countr_zero(_pdep_u64(1ULL << (j-count), data[index]));
}

void Bitvector::push_back (const uint64_t bit) {
    if (_size % 64 == 0) {
        data.push_back(0);
        data.shrink_to_fit();
    }
    data.back() = data.back() | (bit << (_size%64));
    _size++;
}

void Bitvector::print (void) const {
    for (auto it = data.cbegin(); it != data.cend(); it++) {
        for (int8_t b = 56; b >= 0; b-=8) {
            std::cout << std::bitset<8>(*it >> b) << ' ';
        }
        std::cout << '\n';
    }
}

uint64_t Bitvector::size (void) const {
    return _size;
}

bool Bitvector::access (const uint64_t i) const {
    assert (i < _size);
    return (data[i/64] >> (i%64)) & 1U;
}

uint64_t Bitvector::leading_ones (const uint64_t i) const {
    assert (i < _size);
    return std::countr_one(data[i/64] >> (i%64));
}

#endif
