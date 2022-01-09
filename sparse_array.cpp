#ifndef SPARSE_ARRAY
#define SPARSE_ARRAY

#include "bitvector.cpp"
#include <vector>

template <class T>
class Sparse_array final {
private:
    T * data;
    T * end;
    Bitvector *const bv;
    size_t _size;
public:
    Sparse_array(const std::vector<T> *const arr);
    ~Sparse_array();
    const T *const operator[](const size_t i) const;
    size_t size (void) const;
};

template <class T>
Sparse_array<T>::Sparse_array(const std::vector<T> *const arr)
    : bv(new Bitvector(arr->size())), _size(arr->size())
{
    size_t nonnulls = 0;
    for (auto it = arr->cbegin(); it != arr->cend(); it++) {
        if (*it) {
            nonnulls++;
            bv->push_back(1);
        } else {
            bv->push_back(0);
        }
    }
    data = new T[nonnulls];
    end = data + nonnulls;
    for (size_t j = 0; j < nonnulls; j++) {
        data[j] = (*arr)[bv->select(j)];
    }
}

template <class T>
Sparse_array<T>::~Sparse_array() {
    delete[] data;
    delete bv;
}

template <class T>
const T *const Sparse_array<T>::operator[](const size_t i) const {
    assert (i < _size);
    if (bv->access(i)) {
        return &data[bv->rank(i)];
    } else {
        return nullptr;
    }
}

template <class T>
size_t Sparse_array<T>::size(void) const {
    return _size;
}


#endif
