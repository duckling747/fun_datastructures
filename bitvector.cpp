#ifndef BITVECTOR
#define BITVECTOR

#include <iostream>
#include <cstdint>
#include <bit>
#include <bitset>
#include <cassert>

class Bitvector final {
private:
          uint64_t *const data;
    const uint64_t *const end;
          uint64_t top;
          uint64_t _size;

public:
    Bitvector (const size_t s);
    ~Bitvector ();
    uint64_t rank (const uint64_t i);
    uint64_t select (const uint64_t j);
    void push_back (const uint64_t bit);
    void print (void);
    uint64_t size (void);
    bool access (const uint64_t i);

};

Bitvector::Bitvector(const size_t s)
    : data(new uint64_t[s/64+1]), _size(0), end(data+(s/64)+1), top(0)
{}

Bitvector::~Bitvector() {
    delete[] data;
}

uint64_t Bitvector::rank (const uint64_t i) {
    assert (i <= _size);
    
    uint64_t count = 0;
    const uint64_t index = i / 64;
    for (uint64_t j = 0; j < index; j++) {
        count += std::popcount(data[j]);
    }
    return count + std::popcount( data[index] & ((1ULL << (i%64)) - 1) );
}

uint64_t Bitvector::select (const uint64_t j) {
    assert (j < _size);
    
    uint64_t count = 0;
    uint64_t index = 0;
    auto it = data;
    for (; it != end && count + std::popcount(*it) <= j+1 && index + 64 < _size; it++) {
        count += std::popcount(*it);
        index += 64;
    }
    uint8_t shift;
    for (shift = 0; shift <= _size%64 && count != j+1; shift++) {
        count += (( *it >> shift ) & 1U);
    }
    return index + (shift ? shift-1 : shift);
}

void Bitvector::push_back (const uint64_t bit) {
    if (_size % 64 == 0) {
        data[top++] = 0;
    }
    data[top-1] = data[top-1] | (bit << (_size%64));
    _size++;
}

void Bitvector::print (void) {
    for (auto it = data; it != end; it++) {
        for (int8_t b = 56; b >= 0; b-=8) {
            std::cout << std::bitset<8>(*it >> b) << ' ';
        }
        std::cout << '\n';
    }
}

uint64_t Bitvector::size (void) {
    return _size;
}

bool Bitvector::access (const uint64_t i) {
    assert (i < _size);
    return (data[i/64] >> (i%64)) & 1U;
}

#endif
