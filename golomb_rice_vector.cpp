#ifndef GOLOMB
#define GOLOMB
#include <cstdint>
#include <bit>
#include <vector>
#include <memory>
#include "bitvector.cpp"


template <class T>
class GolombRiceVector final {
private:
    const uint8_t k;
    const uint64_t mask;
    const uint64_t _size;
    
    std::unique_ptr<Bitvector> B;

    uint8_t len (const T &n) const;
    T get_code (size_t &loc) const;
    uint64_t encode (const T &n) const;
    T decode (const uint64_t &c) const;

public:
    GolombRiceVector (const std::vector<T> &s, const uint8_t k = 8);
    T get (const size_t i) const;
    int64_t index_of (const T item) const;
    const size_t size (void) const;
    const size_t data_bits_taken (void) const;
    void print (void) const;

};

template <class T>
GolombRiceVector<T>::GolombRiceVector (const std::vector<T> &s, const uint8_t k)
    : k(k), mask((1ULL << k) - 1), _size(s.size())
{
    static_assert(std::is_unsigned<T>::value, "Unsigned integral required.");
    std::vector<uint64_t> codes (s.size());
    for (size_t i = 0; i < s.size(); i++) {
        codes[i] = encode(s[i]);
    }
    B = std::make_unique<Bitvector>();
    for (size_t j = 0; j < codes.size(); j++) {
        for (int8_t i = len(s[j]) - 1; i >= 0; i--) {
            B->push_back((codes[j] >> i) & 1U);
        }
    }
}

template <class T>
uint64_t GolombRiceVector<T>::encode (const T &n) const {
    const uint64_t q = n >> k;
    const uint64_t ibin = n - (q>>k);
    uint64_t out = 0;
    for (uint8_t i = 0; i < q; i++) {
        out = out << 1ULL | 1U;
    }
    out <<= 1ULL;
    for (int8_t i = k-1; i >= 0; i--) {
        out = out << 1ULL | ((ibin >> i) & 1U);
    }
    return out;
}

template <class T>
T GolombRiceVector<T>::decode (const uint64_t &c) const {
    const uint64_t q = std::countl_one( c << std::countl_zero(c) );
    return (c&mask) + (q>>k);
}

template <class T>
uint8_t GolombRiceVector<T>::len (const T &n) const {
    return (n >> k) + k + 1;
}

template <class T>
T GolombRiceVector<T>::get_code (size_t &loc) const {
    assert (loc < B->size());
    T out(0);
    bool bit;
    do {
        bit = B->access(loc++);
        out = out << 1ULL | bit;
    } while (bit == 1);
    for (uint8_t j = 0; j < k; j++) {
        bit = B->access(loc++);
        out = out << 1ULL | bit;
    }
    return out;
}

template <class T>
T GolombRiceVector<T>::get (const size_t i) const {
    // std::cout << "i: " << i << '\n';
    assert (i < _size);
    size_t loc = 0;
    for (size_t j = 0; j < i; j++) {
        loc += B->leading_ones(loc) + 1 + k;
    }
    
    return decode(get_code(loc));
}

template <class T>
int64_t GolombRiceVector<T>::index_of (const T item) const {
    for (size_t loc = 0, index = 0; index < _size; index++) {
        const T c = get_code(loc);
        if (decode(c) == item) {
            return index;
        }
    }
    return -1;
}

template <class T>
const size_t GolombRiceVector<T>::size (void) const {
    return _size;
}

template <class T>
const size_t GolombRiceVector<T>::data_bits_taken (void) const {
    return ((B->size() + 63) >> 6) << 6;
}

template <class T>
void GolombRiceVector<T>::print(void) const {
    B->print();
}

#endif
