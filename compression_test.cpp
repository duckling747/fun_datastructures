#include <iostream>
#include <cstdint>
#include <chrono>
#include <fstream>
#include <memory>
#include "compressed_bitvector.cpp"

using t = std::chrono::high_resolution_clock;

inline constexpr auto d = [](const std::chrono::time_point<t> &a, const std::chrono::time_point<t> &b) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(b-a).count();
};

std::unique_ptr<std::vector<unsigned char>> read_it (const std::string &fp) {
    std::ifstream f (fp, std::ios::in|std::ios::binary);
    if (!f.good()) {
        std::cerr << "problem opening file\n";
        return nullptr;
    }
    f.seekg (0, std::ios::end);
    auto e = f.tellg();
    f.seekg (0, std::ios::beg);
    auto bytes = std::make_unique<std::vector<unsigned char>>(e);
    f.read(reinterpret_cast<char*>(bytes->data()), e);
    f.close();
    
    return bytes;
}

void write_it (const Compressed_bitvector<unsigned char> &v, const std::string &fp) {
    std::ofstream f (fp, std::ios::out|std::ios::binary|std::ios::trunc);
    if (!f.good()) {
        std::cerr << "problem opening writing file\n";
    }
    for (size_t i = 0; i < v.size() && f.good(); i++) {
        f << v[i];
    }
    f.close();
}

int main (int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "usage: ./a.out <filepath>\n";
        return 1;
    }
    const auto a = t::now();
    auto bb = read_it(argv[1]);
    if (!bb) return 1;
    const auto e = bb->size();
    const Compressed_bitvector<unsigned char> v (*bb);
    const auto b = t::now();
    std::cout << "orig size: " << e << " bytes\n"
              << "size in memory (optimistic estimate): " << v.data_bits_taken()/8 << " bytes\n"
              << "ratio: " << e/(v.data_bits_taken()/8.0l) << '\n'
              << "took: " << d(a,b) << "ms\n";
    write_it(v, argv[1] + std::string(".new"));
}
