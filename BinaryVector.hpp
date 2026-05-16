#pragma once
/*
    Mô tả:
        Lớp BinaryVector đại diện cho vector nhị phân độ dài n
        trên GF(2). Được dùng để biểu diễn:
            - Bản tin m = (m_{k-1}, ..., m_1, m_0)
            - Từ mã v = (v_{l-1}, ..., v_1, v_0)
            - Vector kiểm tra p = (p_{r-1}, ..., p_0)

    Quy ước lưu trữ:
        bits_[0] = bit có chỉ số 0 (LSB / hệ số tự do x^0)
        bits_[n-1] = bit có chỉ số n-1 (MSB / hệ số x^{n-1})
        Khi in ra: MSB trước (giống cách đọc thông thường)
 */

#include "Bit.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cassert>

class BinaryVector {
public:
    // === CONSTRUCTOR ===
    
    BinaryVector() {}

    // Tạo vector toàn 0 độ dài n
    explicit BinaryVector(int n) : bits_(n, Bit(0)) {}

    // Tạo từ vector<int> (MSB-first)
    explicit BinaryVector(const std::vector<int>& v) {
        bits_.resize(v.size());
        for (int i = 0; i < (int)v.size(); i++)
            bits_[i] = Bit(v[v.size()-1-i]);   // MSB-first → LSB-first
    }

    // Tạo từ vector<Bit> (MSB-first)
    explicit BinaryVector(const std::vector<Bit>& v) {
        bits_.resize(v.size());
        for (int i = 0; i < (int)v.size(); i++)
            bits_[i] = v[v.size()-1-i];
    }

    // Xâu nhị phân MSB-first => BinaryVector
    static BinaryVector fromBitString(const std::string& s) {
        BinaryVector bv;
        bv.bits_.resize(s.size());
        for (int i = 0; i < (int)s.size(); i++) {
            if (s[i] != '0' && s[i] != '1')
                throw std::invalid_argument(
                    "BinaryVector: chuoi ky tu khong hop le: " + s);
            bv.bits_[i] = Bit(s[s.size()-1-i] - '0');  // MSB-first → LSB-first
        }
        return bv;
    }

    // === ITERATORS ===

    auto begin() { return bits_.begin(); }
    auto end()   { return bits_.end(); }
    auto begin() const { return bits_.begin(); }
    auto end()   const { return bits_.end(); }

    const std::vector<Bit>& bits() const { return bits_; }
    std::vector<Bit>&       bits()       { return bits_; }

    // === SIZE ===

    int  size()    const { return (int)bits_.size(); }
    int  length()  const { return size(); }
    bool empty()   const { return bits_.empty(); }
    void resize(int n) { bits_.resize(n, Bit(0)); }

    // === TRUY CẬP PHẦN TỬ ===

    // Chỉ số theo LSB-first (bits_[0] = x^0)
    Bit& operator[](int i) {
        if (i < 0 || i >= size())
            throw std::out_of_range("BinaryVector: chi so ngoai pham vi");
        return bits_[i];
    }
    const Bit& operator[](int i) const {
        if (i < 0 || i >= size())
            throw std::out_of_range("BinaryVector: chi so ngoai pham vi");
        return bits_[i];
    }

    // === WEIGHT & HAMMING DISTANCE===

    int weight() const {
        int w = 0;
        for (const Bit& b : bits_) w += b.value();
        return w;
    }

    int hammingDistance(const BinaryVector& o) const {
        if (size() != o.size())
            throw std::invalid_argument(
                "BinaryVector::hammingDistance: do dai khong khop");
        BinaryVector diff = *this + o;
        return diff.weight();
    }

    // === CHECK ===

    bool isZero() const {
        for (const Bit& b : bits_) if (b.value() != 0) return false;
        return true;
    }
    
    // === OPERATORS  ===

    // So sánh
    bool operator==(const BinaryVector& o) const { return bits_ == o.bits_; }
    bool operator!=(const BinaryVector& o) const { return !(*this == o); }
    
    // Cộng & trừ vector (XOR từng bit)
    BinaryVector operator+(const BinaryVector& o) const {
        if (size() != o.size())
            throw std::invalid_argument(
                "BinaryVector::operator+: do dai khong khop");
        BinaryVector res(size());
        for (int i = 0; i < size(); i++) res.bits_[i] = bits_[i] + o.bits_[i];
        return res;
    }

    BinaryVector& operator+=(const BinaryVector& o) { *this = *this + o; return *this; }
    BinaryVector operator-(const BinaryVector& o) const { return *this + o; }
    BinaryVector& operator-=(const BinaryVector& o) { *this = *this + o; return *this; }
    

    // Vector x vector => bit (nhân vô hướng)
    Bit dot(const BinaryVector& o) const {
        if (size() != o.size())
            throw std::invalid_argument(
                "BinaryVector::dot: do dai khong khop");
        Bit res(0);
        for (int i = 0; i < size(); i++) res += bits_[i] * o.bits_[i];
        return res;
    }

    Bit operator*(const BinaryVector& o) const { return dot(o); }

    // Vector x bit => vector
    BinaryVector operator*(const Bit& b) const {
        BinaryVector res(size());
        for (int i = 0; i < size(); i++) res.bits_[i] = bits_[i] * b;
        return res;
    }

    // === CẮT / NỐI ===

    // [other | this] (LSB -> MSB)
    BinaryVector concat(const BinaryVector& other) const {
        BinaryVector res(size() + other.size());
        for (int i = 0; i < other.size(); i++) res.bits_[i] = other.bits_[i];
        for (int i = 0; i < size(); i++) res.bits_[other.size()+i] = bits_[i];
        return res;
    }

    // v[start...start+len-1] (LSB -> MSB)
    BinaryVector subvec(int start, int len) const {
        if (start < 0 || start+len > size())
            throw std::out_of_range("BinaryVector::subvec: chi so ngoai pham vi");
        BinaryVector res(len);
        for (int i = 0; i < len; i++) res.bits_[i] = bits_[start+i];
        return res;
    }

    // === CHUYỂN THÀNH DẠNG CHUỖI ===

    std::string toBitString() const {
        if (bits_.empty()) return "0";
        std::string s;
        for (int i = size()-1; i >= 0; i--) s += bits_[i].toChar();
        return s;
    }

    // === I/O ===
    friend std::ostream& operator<<(std::ostream& os, const BinaryVector& bv) {
        return os << bv.toBitString();
    }

    friend std::istream& operator>>(std::istream& is, BinaryVector& bv) {
        std::string s; is >> s;
        bv = BinaryVector::fromBitString(s);
        return is;
    }

private:
    std::vector<Bit> bits_;   // bits_[0] = LSB
};