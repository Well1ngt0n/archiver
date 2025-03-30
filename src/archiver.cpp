#define ARCHIVER
#include "archiver.h"

#define ERROR_CODE 111

// const std::string Path = "1";

std::map<Bits, size_t> Archiver::CountSymbols(std::string file_name, bool last) {
    std::ifstream file(file_name, std::ios::binary);
    if (file.fail()) {
        std::cerr << "Cant open file with name: " << file_name << std::endl;
        exit(ERROR_CODE);
    }
    std::map<std::vector<bool>, size_t> cnt_symbols;

    for (size_t i = 0; i < file_name.size(); i++) {
        unsigned char c = static_cast<unsigned char>(file_name[i]);
        cnt_symbols[AddTo9(ConvertToBits<unsigned char>(c))]++;
    }

    unsigned char c = 0;
    while (file.read(reinterpret_cast<char *>(&c), sizeof(c))) {
        cnt_symbols[AddTo9(ConvertToBits<unsigned char>(c))]++;
    }

    cnt_symbols[FILENAME_END] = 1;
    if (last) {
        cnt_symbols[END_ARCHIVE] = 1;
        cnt_symbols[ONE_MORE_FILE] = 0;
    } else {
        cnt_symbols[END_ARCHIVE] = 0;
        cnt_symbols[ONE_MORE_FILE] = 1;
    }

    file.close();
    return cnt_symbols;
}

std::map<Bits, size_t> Archiver::GetCodesLengthFromArchivatedFileHeader(BitsIstream &archive) {
    size_t symbols_cnt = static_cast<size_t>(ConvertBitsToInt(archive.Get9Bit()));
    std::vector<std::vector<bool>> alphabet(symbols_cnt);
    for (size_t i = 0; i < symbols_cnt; i++) {
        alphabet[i] = archive.Get9Bit();
    }
    size_t cur_symbs = 0;
    size_t last_ind = 0;
    size_t cur_len = 1;
    std::map<std::vector<bool>, size_t> codes_length;
    while (cur_symbs != symbols_cnt) {
        int32_t cnt = ConvertBitsToInt(archive.Get9Bit());

        for (int i = 0; i < cnt; i++) {
            codes_length[alphabet[last_ind]] = cur_len;
            last_ind++;
        }
        cur_len++;
        cur_symbs += cnt;
    }
    return codes_length;
}

void Archiver::Zip(std::string &archive_name, std::vector<std::string> &file_names) {
    std::ofstream archive_out(archive_name, std::ios::binary);
    BitsOstream archive(archive_out);

    for (size_t i = 0; i < file_names.size(); i++) {
        std::string file_name;
        for (size_t j = file_names[i].size() - 1; j >= 0 && file_names[i][j] != '/'; j--) {
            file_name = file_names[i][j] + file_name;
        }
        bool last = (i + 1 == file_names.size());
        auto symbols_cnt = CountSymbols(file_names[i], last);

        HuffmanCoding encoder(symbols_cnt, HuffmanCoding::ConstructFrom::SymbolsCnt);

        std::ifstream file(file_names[i], std::ios::binary);
        if (file.fail()) {
            std::cerr << "Cant open file with name: " << file_names[i] << std::endl;
            exit(ERROR_CODE);
        }
        encoder.Encode(file_name, file, archive, last);
        file.close();
    }
    archive.Flush();
    archive_out.close();
}

void Archiver::Unzip(std::string &archive_name) {
    std::ifstream archive_in(archive_name, std::ios::binary);
    if (archive_in.fail()) {
        std::cerr << "Cant open file with name: " << archive_name << std::endl;
        exit(ERROR_CODE);
    }
    BitsIstream archive(archive_in);

    bool is_archive_end = false;
    while (!is_archive_end) {
        auto codes_length = GetCodesLengthFromArchivatedFileHeader(archive);
        HuffmanCoding decoder(codes_length, HuffmanCoding::ConstructFrom::CodesLength);
        is_archive_end = decoder.Decode(archive);
    }
    archive_in.close();
}

#ifdef ARCHIVER
int main(int argc, char *argv[]) {
    Archiver archiver;
    if (argc <= 1) {
        std::cout << "use -h to get information about archiver";
        return ERROR_CODE;
    } else {
        if (std::string(argv[1]) == "-h") {
            std::cout << "you can use archiver with three types of flags:" << std::endl;
            std::cout << std::endl;
            std::cout << "-h : user manual" << std::endl;
            std::cout << std::endl;
            std::cout << "-c archive_name file_name1 file_name2 ... file_name{n} :" << std::endl;
            std::cout << "make archive with name archive_name and files with names file_name{i} in it" << std::endl;
            std::cout << std::endl;
            std::cout << "-d archive_name :" << std::endl;
            std::cout << "unzip archive with name archive_name" << std::endl;
        } else if (std::string(argv[1]) == "-c") {
            if (argc <= 3) {
                std::cout << "you must give to the programm name of the archive and names of files" << std::endl;
                return ERROR_CODE;
            }
            std::string archive_name = argv[2];

            std::vector<std::string> files;
            for (size_t i = 3; i < static_cast<size_t>(argc); i++) {
                files.push_back(argv[i]);
            }

            archiver.Zip(archive_name, files);
        } else if (std::string(argv[1]) == "-d") {
            if (argc != 3) {
                std::cout << "you must give to the programm name of the archive and nothing more" << std::endl;
                return ERROR_CODE;
            }
            std::string archive_name = argv[2];
            archiver.Unzip(archive_name);
        }
    }
    return 0;
}
#endif