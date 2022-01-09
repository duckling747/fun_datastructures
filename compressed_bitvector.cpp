#ifndef COMPRESSED_BV
#define COMPRESSED_BV
#include <cstdint>
#include <iostream>
#include <cassert>
#include <queue>
#include <vector>
#include <map>
#include <stack>
#include "bitvector.cpp"
#include "partial_sums.cpp"


struct Node {
    uint64_t data;
    uint64_t freq;
    Node *left;
    Node *right;
    Node (uint64_t data, uint64_t freq)
        : data(data), freq(freq), left(nullptr), right(nullptr)
    {}
    Node (uint64_t freq, Node *left, Node *right)
        : freq(freq), left(left), right(right)
    {}
};

class Compressed_bitvector final {
private:
    std::vector<Bitvector*> data;
    Partial_sums *const sums;
public:
    Compressed_bitvector (const uint64_t *const arr, const size_t len, const uint8_t macroblock_bits);
    ~Compressed_bitvector ();
    uint64_t rank (const uint64_t i) const;
    uint64_t select (const uint64_t j) const;
    void push_back (const uint64_t bit);
    void print (void) const;
    uint64_t size (void) const;
    bool access (const uint64_t i) const;
};

Compressed_bitvector::Compressed_bitvector (const uint64_t *const arr, const size_t len, const uint8_t macroblock_bits)
    : sums(new Partial_sums(len)), data{new Bitvector(macroblock_bits)}
{
    auto *const freqs = new std::map<uint64_t, uint64_t>;
    for (size_t i = 0; i < len; i++) {
        (*freqs)[arr[i]]++;
    }
    auto *const q = new std::priority_queue<Node*, std::vector<Node*>,
        decltype([](const Node *const a, const Node *const b){
            return a->freq >= b->freq;
        })
    >;
    for (const auto &a : *freqs) {
        q->push(new Node(a.first, a.second));
    }
    delete freqs;
    while (q->size() > 1) {
        Node *const a = q->top();
        q->pop();
        Node *const b = q->top();
        q->pop();
        q->push(new Node(a->freq + b->freq, a, b));
    }
    auto *const codes = new std::map<uint64_t, std::string>;
    auto *const cq = new std::stack<std::pair<Node*, std::string>>;
    cq->push({ q->top(), "" });
    while (!cq->empty()) {
        const auto next = cq->top();
        cq->pop();
        if (next.first->left == nullptr) {
            (*codes)[next.first->data] = next.second;
            continue;
        }
        cq->push({ next.first->left, next.second+char(0) });
        cq->push({ next.first->right, next.second+char(1) });
        delete next.first;
    }
    delete q;
    delete cq;
    std::cout << "coding...\n";
    uint64_t sum = 0;
    sums->push_back(sum);
    for (size_t i = 0; i < len; i++) {
        sums->push_back(sum += (*codes)[arr[i]].size());
        for (const char &c : (*codes)[arr[i]]) {
            if (data.back()->size() >= macroblock_bits) {
                data.push_back(new Bitvector(macroblock_bits));
            }
            data.back()->push_back( c );
            std::cout << int(c);
        }
        std::cout << '\n';
    }
    delete codes;
}

Compressed_bitvector::~Compressed_bitvector () {
    for (const auto &e : data) delete e;
    delete sums;
}

void Compressed_bitvector::print (void) const {
    std::cout << "data:\n";
    for (const auto &a : data) {
        a->print();
    }
    std::cout << "sums:\n";
    sums->print();
}


#include <random>
static std::random_device rd;
static std::default_random_engine gen(rd());
static std::uniform_int_distribution<char> dis ('a', 'z');
int main (void) {
    
    uint64_t len = 200000;
    uint64_t arr[len] = {};
    for (uint64_t i = 0; i < len; i++) {
        arr[i] = dis(gen);
    }
    const Compressed_bitvector c (arr, len, 64);
    c.print();
}

#endif
