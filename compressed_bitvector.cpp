#ifndef COMPRESSED_BV
#define COMPRESSED_BV
#include <cstdint>
#include <iostream>
#include <cassert>
#include <vector>
#include <bit>
#include <sstream>
#include <memory>
#include "bitvector.cpp"
#include "partial_sums.cpp"
#include "compression.cpp"
#include "vbyte_vector.cpp"

template <class T>
class Compressed_bitvector;

template <class T>
std::ostream& operator<< (std::ostream& os, const Compressed_bitvector<T> &b);

template <class T>
class Compressed_bitvector final {
private:
    static constexpr uint8_t maximum_len = sizeof(T) << 3; // (in bits)
    std::unique_ptr<Bitvector> data;
    std::unique_ptr<Partial_sums> sums;
    std::unique_ptr<VByteVector<T>> grv;
    const size_t _size;
public:
    Compressed_bitvector (std::vector<T> &msg);
    friend std::ostream& operator<< <>(std::ostream &os, const Compressed_bitvector &b);
    const T operator[] (const size_t i) const;
    const size_t size (void) const;
    const size_t data_bits_taken (void) const;
};

template <class T>
Compressed_bitvector<T>::Compressed_bitvector (std::vector<T> &msg)
    : _size(msg.size())
{
    static_assert(std::is_unsigned<T>::value, "Unsigned integral required.");
    std::vector<T> order;
    Compress::encode<T>( msg, order );
    grv = std::make_unique<VByteVector<T>>(order);
    for (int i = 0; i < order.size(); i++) {
        //std::cout << order[i] << "==" << grv->get(i) << '\n';
        assert (order[i] == grv->get(i));
    }
    data = std::make_unique<Bitvector>();
    sums = std::make_unique<Partial_sums>();
    for (const auto code : msg) {
        uint8_t count = maximum_len-std::countl_zero(code);
        if (count == 0) count = 1;
        for (uint8_t i = 0; i < count; i++) {
            data->push_back((code >> i) & 1U);
        }
    }
    uint64_t len = 0;
    sums->push_back(len);
    for (const auto code : msg) {
        const uint8_t count = maximum_len-std::countl_zero(code);
        sums->push_back(len += count == 0 ? 1 : count);
    }
}

template <class T>
const T Compressed_bitvector<T>::operator[] (const size_t i) const {
    assert ( i < _size );
    size_t c = 0;
    const auto s = sums->sum(i);
    const auto e = sums->sum(i+1);
    /*
    std::cout << "sums->sum(i): " << s << '\n'
                << "sums->sum(i+1): " << e << '\n';
    */
    for (size_t j = s, k = 0; j < e; j++, k++) {
        c |= data->access(j) << k;
    }
    return grv->get(c);
}

template <class T>
const size_t Compressed_bitvector<T>::size (void) const {
    return _size;
}

template <class T>
std::ostream& operator<< (std::ostream &os, const Compressed_bitvector<T> &b) {
    std::stringstream ss;
    ss << "data: ";
    for (size_t i = 0; i < b.data->size(); i++) ss << b.data->access(i);
    ss << "\nsums: ";
    for (size_t i = 0; i < b.sums->size(); i++) ss << *(b.sums);
    return os << ss.str();
}

template <class T>
const size_t Compressed_bitvector<T>::data_bits_taken (void) const {
    return (((data->size() + 63) >> 6) << 6)
        + grv->data_bits_taken()
        + sums->data_bits_taken();
}

#endif
