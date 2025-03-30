#pragma once
#include <map>
#include <string>
#include <bitset>
#include <set>
#include "bittrie.h"
#include "bitsstream.h"
#include <algorithm>
#include <fstream>

using Bits = std::vector<bool>;

const Bits FILENAME_END = {false, false, false, false, false, false, false, false, true};
const Bits ONE_MORE_FILE = {true, false, false, false, false, false, false, false, true};
const Bits END_ARCHIVE = {false, true, false, false, false, false, false, false, true};

template <typename T>
Bits ConvertToBits(T c) {
    if (c == 0) {
        return {false};
    }
    Bits bits_arr;
    while (c > 0) {
        bits_arr.push_back(c & 1);
        c >>= 1;
    }
    return bits_arr;
};

int32_t ConvertBitsToInt(Bits v);

Bits AddTo9(Bits v);

class HuffmanCoding {
private:
    std::map<Bits, size_t> symbols_cnt_;
    std::map<Bits, size_t> codes_length_;
    void GenerateCodes();
    void MakeCodesCanonical();

public:
    enum ConstructFrom { SymbolsCnt, CodesLength };

    BinTrie<Bits> huffman_trie;
    std::map<Bits, Bits> huffman_codes;
    HuffmanCoding(std::map<Bits, size_t> &init_map, HuffmanCoding::ConstructFrom type);
    void Encode(std::string &file_name, std::istream &file, BitsOstream &archive, bool last);  // file -> archive
    bool Decode(BitsIstream &archive);  // extract 1 file from archive, return is file last
};