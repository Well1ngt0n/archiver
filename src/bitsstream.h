#include <iostream>
#include <bitset>
#include <vector>

const size_t BITS_IN_BYTE = 8;

class BitsIstream {
private:
    std::istream& in_;
    std::bitset<BITS_IN_BYTE> buff_;
    int bits_in_buffer_ = 0;

    void AddBitsToBuffer();
    bool Get1BitFromBuffer();
    bool Empty() const;

public:
    explicit BitsIstream(std::istream& in);
    bool Get1Bit();
    std::vector<bool> Get9Bit();
};

class BitsOstream {
private:
    std::ostream& out_;
    std::bitset<BITS_IN_BYTE> buff_;
    int bits_in_buffer_ = 0;

    void DropBitsFromBuffer();
    void Put1BitToBuffer(bool bit);
    bool Full() const;

public:
    explicit BitsOstream(std::ostream& out);
    ~BitsOstream();
    void Put1Bit(bool bit);
    void PutNBit(std::vector<bool> bits);
    void Flush();
};