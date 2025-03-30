#include "bitsstream.h"

BitsIstream::BitsIstream(std::istream& in) : in_(in) {
}

bool BitsIstream::Get1Bit() {
    return Get1BitFromBuffer();
}

std::vector<bool> BitsIstream::Get9Bit() {
    std::vector<bool> bits(BITS_IN_BYTE + 1);
    for (size_t i = 0; i < BITS_IN_BYTE + 1; i++) {
        bits[i] = Get1BitFromBuffer();
    }
    return bits;
}

void BitsIstream::AddBitsToBuffer() {
    unsigned char c = 0;
    in_.read(reinterpret_cast<char*>(&c), sizeof(c));
    buff_ = std::bitset<BITS_IN_BYTE>(c);
    bits_in_buffer_ = BITS_IN_BYTE;
}

bool BitsIstream::Get1BitFromBuffer() {
    if (this->Empty()) {
        AddBitsToBuffer();
    }
    bool once_bit = buff_[0];
    buff_ >>= 1;
    bits_in_buffer_--;
    return once_bit;
}

bool BitsIstream::Empty() const {
    return bits_in_buffer_ == 0;
}

void BitsOstream::DropBitsFromBuffer() {
    unsigned char c = 0;
    for (size_t i = 0; i < BITS_IN_BYTE; i++) {
        c += (buff_[i] << i);
    }
    out_.put(static_cast<char>(c));
    bits_in_buffer_ = 0;
    buff_.reset();
}

void BitsOstream::Put1BitToBuffer(bool bit) {
    if (this->Full()) {
        DropBitsFromBuffer();
    }
    buff_[bits_in_buffer_] = bit;
    bits_in_buffer_++;
}

bool BitsOstream::Full() const {
    return bits_in_buffer_ == BITS_IN_BYTE;
}

BitsOstream::BitsOstream(std::ostream& out) : out_(out) {
}

void BitsOstream::Put1Bit(bool bit) {
    return Put1BitToBuffer(bit);
}

void BitsOstream::PutNBit(std::vector<bool> bits) {
    for (size_t i = 0; i < bits.size(); i++) {
        Put1BitToBuffer(bits[i]);
    }
}

void BitsOstream::Flush() {
    DropBitsFromBuffer();
}

BitsOstream::~BitsOstream() {
    Flush();
}