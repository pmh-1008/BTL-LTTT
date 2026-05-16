/*
    Mô tả:
        Lớp Bit đại diện cho một phần tử trong trường Galois GF(2),
        tức là tập {0, 1} với các phép toán:
            - Cộng (+) ≡ XOR   (mod 2)
            - Nhân (*)  ≡ AND   (mod 2)
            - Phủ định  ≡ NOT

    Tính chất GF(2):
        0 + 0 = 0,  0 + 1 = 1,  1 + 0 = 1,  1 + 1 = 0
        0 * 0 = 0,  0 * 1 = 0,  1 * 0 = 0,  1 * 1 = 1
        -x = x 
 */

#pragma once
#include <iostream>
#include <stdexcept>
#include <string>

class Bit {
public:
    // === CONSTRUCTOR ===
    Bit() : val(0) {}
    Bit(int v) : val(v & 1 ? 1 : 0) {}

    // === ACCESSORS ===
    int  value()  const { return val; }

    // === OPERATORS ===
    // So sánh
    bool operator==(const Bit& o) const { return val == o.val; }
    bool operator!=(const Bit& o) const { return val != o.val; }

    // XOR, AND, OR, NOT
    Bit XOR(const Bit& o) const { return Bit(val ^ o.val); }
    Bit AND(const Bit& o) const { return Bit(val & o.val); }
    Bit OR(const Bit& o)  const { return Bit(val | o.val); }
    Bit operator!() const { return Bit(val ^ 1); }

    // Cộng & Trừ : XOR
    Bit operator+(const Bit& o) const { return Bit(val ^ o.val); }
    Bit& operator+=(const Bit& o)     { val ^= o.val; return *this; }
    Bit operator-(const Bit& o) const { return Bit(val ^ o.val); }
    Bit& operator-=(const Bit& o)     { val ^= o.val; return *this; }

    // Nhân: AND
    Bit operator*(const Bit& o) const { return Bit(val & o.val); }
    Bit& operator*=(const Bit& o)     { val &= o.val; return *this; }

    // Ép kiểu
    explicit operator int()  const { return val; }
    explicit operator bool() const { return val != 0; }

    // === I/O ===
    char toChar()         const { return val ? '1' : '0'; }
    std::string toString() const { return val ? "1" : "0"; }

    friend std::ostream& operator<<(std::ostream& os, const Bit& o) {
        return os << o.val;
    }

    friend std::istream& operator>>(std::istream& is, Bit& o) {
        int v; is >> v;
        if (v != 0 && v != 1)
            throw std::invalid_argument("Bit: gia tri phai la 0 hoac 1");
        o.val = v;
        return is;
    }

private:
    bool val;
};