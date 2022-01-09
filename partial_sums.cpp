#ifndef PARTIALSUMS
#define PARTIALSUMS
#include "bitvector.cpp"
#include <iostream>
#include <vector>

class Partial_sums final {
private:
    Bitvector *const B;
public:
    Partial_sums (const size_t n);
    ~Partial_sums ();
    void push_back (const uint64_t sum);
    uint64_t sum (const uint64_t j) const;
    void print (void) const;
};

Partial_sums::Partial_sums (const size_t n)
    : B(new Bitvector(n*5))
{}

Partial_sums::~Partial_sums () {
    delete B;
}

void Partial_sums::push_back (const uint64_t sum) {
    for (auto i = B->size(); i < sum; i++) {
        B->push_back(0);
    }
    B->push_back(1);
}

uint64_t Partial_sums::sum (const uint64_t j) const {
    return B->select(j);
}

void Partial_sums::print (void) const {
    B->print();
}

#endif
