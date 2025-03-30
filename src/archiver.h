#include "huffman.h"
#include <string>
#include <iostream>
#include <vector>
#include <fstream>

class Archiver {
private:
    std::map<Bits, size_t> CountSymbols(std::string file_name, bool last);
    std::map<Bits, size_t> GetCodesLengthFromArchivatedFileHeader(BitsIstream &archive);

public:
    void Zip(std::string &archive_name, std::vector<std::string> &file_names);
    void Unzip(std::string &archive_name);
};