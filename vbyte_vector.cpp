#ifndef VBYTES
#define VBYTES

#include <vector>
#include <memory>
#include <bit>
#include <algorithm>
#include "bitvector.cpp"
#include <iostream>
#include <bitset>

template <class T>
class VByteVector final {
private:
    std::unique_ptr<Bitvector> B;
    const size_t _size;
    const T min_element;

public:
    VByteVector (const std::vector<T> &s);
    T get (const size_t i) const;
    const size_t data_bits_taken (void) const;
    void print (void) const;
};

template <class T>
VByteVector<T>::VByteVector (const std::vector<T> &s)
    : B(std::make_unique<Bitvector>()),
    _size(s.size()),
    min_element(*std::min_element(s.cbegin(), s.cend()))
{
    for (auto it = s.cbegin(); it != s.cend(); it++) {
        T out = *it - min_element;
        for ( ; out >> 7; out >>= 7) {
            for (uint8_t i = 0; i < 7; i++) {
                B->push_back( (out>>i) & 1U );
            }
            B->push_back(0);
        }
        for (uint8_t i = 0; i < 7; i++) {
            B->push_back( (out>>i) & 1U );
        }
        B->push_back(1);
    }
}

template <class T>
T VByteVector<T>::get (const size_t i ) const {
    assert (i < _size);
    size_t loc = 0, j = 7;
    for (; j < B->size() && loc <= i; j+=8) {
        if (B->access(j)) loc++;
    }
    T out (0);
    j-=8;
    for (uint8_t k = 1; k <= 7; k++) {
        out = out << 1 | B->access(j-k);
    }
    for (j-=8; j < B->size() && !B->access(j); j-=8) {
        for (uint8_t k = 1; k <= 7; k++) {
            out = out << 1 | B->access(j-k);
        }
    }
    return out + min_element;
}

template <class T>
const size_t VByteVector<T>::data_bits_taken (void) const {
    return (((B->size() + 63) >> 6) << 6) + (sizeof(min_element)>>3);
}

template <class T>
void VByteVector<T>::print (void) const {
    B->print();
}

#endif
