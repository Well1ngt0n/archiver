#include "huffman.h"

int32_t ConvertBitsToInt(Bits v) {
    int32_t num = 0;
    for (size_t i = 0; i < v.size(); i++) {
        num += (static_cast<int32_t>(v[i]) << i);
    }
    return num;
}

Bits AddTo9(Bits v) {
    while (v.size() < BITS_IN_BYTE + 1) {
        v.push_back(false);
    }
    return v;
}

void HuffmanCoding::GenerateCodes() {
    std::multiset<std::pair<size_t, Node<Bits> *>> st;
    for (auto &[symbol, cnt] : symbols_cnt_) {
        st.insert({cnt, new Node<Bits>(symbol)});
    }
    auto get_mn = [&](std::multiset<std::pair<size_t, Node<Bits> *>> &st) {
        auto mn = *st.begin();
        st.erase(st.begin());
        return mn;
    };
    while (st.size() > 1) {
        auto mn1 = get_mn(st);
        auto mn2 = get_mn(st);
        Node<Bits> *new_node = new Node<Bits>();
        new_node->next[0] = mn1.second;
        new_node->next[1] = mn2.second;
        std::pair<size_t, Node<Bits> *> nw = make_pair(mn1.first + mn2.first, new_node);
        st.insert(nw);
    }

    Node<Bits> *root = st.begin()->second;
    if (st.empty()) {
        root = new Node<Bits>();
    } else {
        root = st.begin()->second;
    }

    huffman_trie.ChangeRoot(root);
    huffman_codes = huffman_trie.GetAllPaths();

    // for canonical form
    for (auto &[symbol, code] : huffman_codes) {
        codes_length_[symbol] = code.size();
    }
}

void HuffmanCoding::MakeCodesCanonical() {
    std::map<Bits, Bits> canonical_huffman_code;
    std::vector<std::pair<size_t, Bits>> old_code;
    for (auto &[symbol, code_length] : codes_length_) {
        old_code.push_back(make_pair(code_length, symbol));
    }
    std::sort(old_code.begin(), old_code.end());

    Bits code = {false};
    // error when doesnt exist code with len i
    for (auto &[code_length, symbol] : old_code) {
        while (code.size() < code_length) {
            code.push_back(false);
        }
        canonical_huffman_code[symbol] = code;
        size_t i = code.size() - 1;
        while (i > 0 && code[i]) {
            code[i] = false;
            i--;
        }
        code[i] = true;
    }
    huffman_codes = canonical_huffman_code;

    huffman_trie.ChangeRoot(new Node<Bits>());

    for (auto &[symbol, code] : huffman_codes) {
        huffman_trie.Add(code, symbol);
    }
}

HuffmanCoding::HuffmanCoding(std::map<Bits, size_t> &init_map, HuffmanCoding::ConstructFrom type) {
    if (type == HuffmanCoding::ConstructFrom::SymbolsCnt) {
        symbols_cnt_ = init_map;
        GenerateCodes();
        MakeCodesCanonical();
    } else {
        codes_length_ = init_map;
        MakeCodesCanonical();
    }
}

void HuffmanCoding::Encode(std::string &file_name, std::istream &file, BitsOstream &archive, bool last) {
    size_t cnt_symbols = huffman_codes.size();

    archive.PutNBit(AddTo9(ConvertToBits<size_t>(cnt_symbols)));
    // symbols
    std::map<size_t, std::vector<Bits>> sub;
    for (auto &[symbol, code] : huffman_codes) {
        sub[code.size()].push_back(AddTo9(symbol));
    }
    for (auto &[len_code, symbols] : sub) {
        sort(symbols.begin(), symbols.end(), [&](Bits s1, Bits s2) { return s1 < s2; });
        for (auto &symbol : symbols) {
            archive.PutNBit(symbol);
        }
    }
    // count of codes with len i+1
    size_t cur_len = 1;
    for (auto &[len_code, symbols] : sub) {
        while (cur_len < len_code) {
            archive.PutNBit(AddTo9(ConvertToBits(0)));
            cur_len++;
        }
        if (cur_len == len_code) {
            archive.PutNBit(AddTo9(ConvertToBits<size_t>(symbols.size())));
            cur_len++;
        }
    }
    // filename
    for (size_t i = 0; i < file_name.size(); i++) {
        unsigned char c = static_cast<unsigned char>(file_name[i]);
        archive.PutNBit(huffman_codes[AddTo9(ConvertToBits<unsigned char>(c))]);
    }

    archive.PutNBit(huffman_codes[FILENAME_END]);
    // file
    unsigned char c = 0;
    while (file.read(reinterpret_cast<char *>(&c), sizeof(c))) {
        archive.PutNBit(huffman_codes[AddTo9(ConvertToBits<unsigned char>(c))]);
    }
    // special symbol
    if (last) {
        archive.PutNBit(huffman_codes[END_ARCHIVE]);
    } else {
        archive.PutNBit(huffman_codes[ONE_MORE_FILE]);
    }
}

bool HuffmanCoding::Decode(BitsIstream &archive) {
    auto get_symbol = [&]() {
        huffman_trie.InitTraversal();
        bool is_word_end = false;
        do {
            is_word_end = huffman_trie.NextInTraversal(archive.Get1Bit());
        } while (!is_word_end);
        return huffman_trie.GetTerminal();
    };
    // get name
    bool is_filename_end = false;
    std::string filename;
    while (!is_filename_end) {
        auto symbol = get_symbol();
        if (symbol == FILENAME_END) {
            is_filename_end = true;
        } else {
            filename.push_back(static_cast<char>(ConvertBitsToInt(symbol)));
        }
    }
    std::ofstream cur_file(filename, std::ios::binary);
    // get file
    bool is_file_end = false;
    bool is_archive_end = false;
    while (!is_file_end) {
        auto symbol = get_symbol();

        if (symbol == ONE_MORE_FILE) {
            is_file_end = true;
        } else if (symbol == END_ARCHIVE) {
            is_file_end = true;
            is_archive_end = true;
        } else {
            cur_file.put(static_cast<char>(ConvertBitsToInt(symbol)));
        }
    }
    cur_file.close();
    return is_archive_end;
}