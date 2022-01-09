#include <bit>
#include <cstdint>
#include <iostream>
#include <bitset>
#include <cassert>
#include <queue>
#include <vector>
#include <map>
#include <stack>
#include "bitvector.cpp"


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
    // TODO: exchange vector with the prefix sums data structure //
    std::vector<uint32_t> sums;
public:
    Compressed_bitvector (const uint64_t *const arr, const size_t len, const uint8_t macroblock_bits);
    ~Compressed_bitvector ();
    uint64_t rank (const uint64_t i);
    uint64_t select (const uint64_t j);
    void push_back (const uint64_t bit);
    void print (void);
    uint64_t size (void);
    bool access (const uint64_t i);
};

Compressed_bitvector::Compressed_bitvector (const uint64_t *const arr, const size_t len, const uint8_t macroblock_bits)
    : sums(), data{new Bitvector(macroblock_bits)}
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
    sums.push_back(0);
    for (size_t i = 0; i < len; i++) {
        sums.push_back(sums.back() + (*codes)[arr[i]].size());
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
    for (const auto &a : data) {
        a->print();
    }
    for (const auto & a : sums) {
        std::cout << a << ' ';
    }
    std::cout << '\n';
}

Compressed_bitvector::~Compressed_bitvector () {
    for (const auto &e : data) delete e;
}

int main (void) {
    uint64_t asdf [21] {1,2,3,4,5,6,7,8,9,9,9,9,9,8, 19,29, 9, 1, 1,2,2};
    Compressed_bitvector c (asdf, 21, 128);
    
}
