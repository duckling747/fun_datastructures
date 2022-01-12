#ifndef COMPRESSIONWHAAT
#define COMPRESSIONWHAAT

#include <vector>
#include <map>
#include <cstdint>
#include <algorithm>

namespace Compress {
    template <class T>
    void encode (std::vector<T> &message, std::vector<T> &order) {
        static_assert(std::is_integral<T>::value, "Integral required.");
        std::map<T, uint64_t> freqs;
        std::map<T, std::vector<size_t>> indexes;
        for (auto it = message.cbegin(); it != message.cend(); it++) {
            freqs[*it]++;
            indexes[*it].push_back(std::distance(message.cbegin(), it));
        }
        std::vector<std::pair<uint64_t, T>> codelengths;
        for (auto it = freqs.cbegin(); it != freqs.cend(); it++) {
            codelengths.emplace_back(it->second, it->first);
        }
        std::sort(codelengths.begin(), codelengths.end(),
                [](std::pair<uint64_t, T> &a, std::pair<uint64_t, T> &b) -> bool {
                    return a.first > b.first || (a.first == b.first && a.second < b.second);
                }
        );
        order.clear(); order.reserve(codelengths.size());
        for (auto it = codelengths.cbegin(); it != codelengths.cend(); it++) {
            order.push_back(it->second);
        }
        T codeword = 0;
        for (auto it = codelengths.cbegin(); it != codelengths.cend(); it++, codeword++) {
            for (auto it2 = indexes[it->second].cbegin(); it2 != indexes[it->second].cend(); it2++) {
                message[*it2] = codeword;
            }
        }
    }
}

#endif
