#ifndef PARTIALSUMS
#define PARTIALSUMS
#include "bitvector.cpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <cassert>

class Partial_sums final {
private:
    std::unique_ptr<Bitvector> B;
    size_t _size;
public:
    Partial_sums ();
    void push_back (const uint64_t sum);
    uint64_t sum (const uint64_t j) const;
    void print (void) const;
    const size_t size (void) const;
    const size_t data_bits_taken (void) const;
    friend std::ostream& operator<< (std::ostream &os, const Partial_sums &s);
};

Partial_sums::Partial_sums ()
    : B(std::make_unique<Bitvector>())
{}

void Partial_sums::push_back (const uint64_t sum) {
    for (auto i = B->size(); i < sum; i++) {
        B->push_back(0);
    }
    B->push_back(1);
    _size++;
}

uint64_t Partial_sums::sum (const uint64_t j) const {
    assert (j < _size);
    return B->select(j);
}

void Partial_sums::print (void) const {
    B->print();
}

const size_t Partial_sums::size (void) const {
    return _size;
}

const size_t Partial_sums::data_bits_taken (void) const {
    return ((B->size() + 63) >> 6) << 6;
}

std::ostream& operator<< (std::ostream &os, const Partial_sums &s) {
    std::stringstream ss;
    for (size_t i = 0; i < s.B->size(); i++) ss << s.B->access(i);
    os << ss.str();
    return os;
}

#endif
