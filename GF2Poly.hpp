/*
    GF2Poly.h  —  Đa thức trên trường GF(2)

    Mô tả:
        Lớp GF2Poly đại diện cho đa thức p(x) = c_n * x^n + ... + c_1 * x + c_0
        với hệ số c_i ∈ GF(2) = {0, 1}.

    Quy ước lưu trữ:
        coeff_[i] = hệ số của x^i  (LSB-first)
        Ví dụ: x^4 + x + 1  →  coeff_ = {1, 1, 0, 0, 1}

    Phép toán cơ bản trên GF(2):
        Cộng (+)   :  XOR từng hệ số
        Trừ  (-)   :  ≡ Cộng (trong GF(2), -a = a)
        Nhân (*)   :  tích chập, hệ số mod 2
        Chia (/,%) :  phép chia đa thức, trả về (thương, dư)
        Đánh giá   :  p(a), a ∈ GF(2)
        GCD        :  ước chung lớn nhất
 */

#pragma once
#include "Bit.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <iostream>

class GF2Poly {
public:
    // === CONSTRUCTORS ===

    GF2Poly() : coeff_({0}) {}

    // Khởi tạo từ vector hệ số LSB-first
    explicit GF2Poly(const std::vector<int>& coeff) {
        coeff_.resize(coeff.size());
        for (int i = 0; i < (int)coeff.size(); i++)
            coeff_[i] = coeff[i] & 1;
        standardize();
    }

    // Đơn thức x^n
    static GF2Poly monomial(int n) {
        GF2Poly p;
        p.coeff_.assign(n + 1, 0);
        p.coeff_[n] = 1;
        return p;
    }

    // Parse từ chuỗi nhị phân MSB-first (Ví dụ: "10011" → x^4 + x + 1)
    static GF2Poly fromBitString(const std::string& s) {
        if (s.empty())
            throw std::invalid_argument("GF2Poly::fromString: chuoi rong");
        GF2Poly p;
        p.coeff_.resize(s.size());
        for (int i = 0; i < (int)s.size(); i++) {
            if (s[i] != '0' && s[i] != '1')
                throw std::invalid_argument(
                    std::string("GF2Poly::fromString: ky tu khong hop le '")
                    + s[i] + "' trong chuoi: " + s);
            // MSB-first → LSB-first: s[0] là x^(n), s[n] là x^0
            p.coeff_[(int)s.size() - 1 - i] = s[i] - '0';
        }
        return p;
    }

    // === PROPERTIES ===

    // Bậc của đa thức (-1 nếu là đa thức 0)
    int degree() const {
        for (int i = (int)coeff_.size() - 1; i >= 0; i--)
            if (coeff_[i]) return i;
        return -1;
    }

    bool isZero() const { return degree() < 0; }
    bool isOne() const { return degree() == 0 && coeff_[0] == 1; }

    // Hệ số của x^i
    int coeff(int i) const {
        if (i < 0 || i >= (int)coeff_.size()) return 0;
        return coeff_[i];
    }

    // Trọng số (số hệ số bằng 1)
    int weight() const {
        int w = 0;
        for (int c : coeff_) w += c;
        return w;
    }

    // === OPERATORS ===

    // So sánh
    bool operator==(const GF2Poly& o) const {
        int d = degree(), od = o.degree();
        if (d != od) return false;
        for (int i = 0; i <= d; i++)
            if (coeff(i) != o.coeff(i)) return false;
        return true;
    }
    bool operator!=(const GF2Poly& o) const { return !(*this == o); }

    // So sánh bậc (dùng khi sắp xếp)
    bool operator<(const GF2Poly& o)  const { return degree() < o.degree(); }
    bool operator<=(const GF2Poly& o) const { return degree() <= o.degree(); }

    // Cộng trừ
    GF2Poly operator+(const GF2Poly& o) const {
        int n = std::max(coeff_.size(), o.coeff_.size());
        std::vector<int> r(n, 0);
        for (int i = 0; i < (int)coeff_.size(); i++)   r[i] ^= coeff_[i];
        for (int i = 0; i < (int)o.coeff_.size(); i++) r[i] ^= o.coeff_[i];
        return GF2Poly(r);
    }
    GF2Poly& operator+=(const GF2Poly& o) { *this = *this + o; return *this; }
    GF2Poly operator-(const GF2Poly& o) const { return *this + o; }
    GF2Poly& operator-=(const GF2Poly& o) { return *this += o; }

    // Tích chập
    GF2Poly operator*(const GF2Poly& o) const {
        if (isZero() || o.isZero()) return GF2Poly();
        int da = degree(), db = o.degree();
        std::vector<int> r(da + db + 1, 0);
        for (int i = 0; i <= da; i++)
            for (int j = 0; j <= db; j++)
                r[i + j] ^= coeff_[i] & o.coeff_[j];
        return GF2Poly(r);
    }
    GF2Poly& operator*=(const GF2Poly& o) { *this = *this * o; return *this; }

    // Nhân vô hướng
    GF2Poly operator*(const Bit& b) const {
        if (b.value() == 0) return GF2Poly();
        return *this;
    }

    // Chia: divmod(a, b) → {thương q, dư r}

    using DivResult = std::pair<GF2Poly,GF2Poly>;

    static std::pair<GF2Poly,GF2Poly> divmod(const GF2Poly& a, const GF2Poly& b) {
        if (b.isZero())
            throw std::domain_error("GF2Poly::divmod: chia cho da thuc 0");

        GF2Poly rem = a;
        int db = b.degree();

        // Vector tích luỹ thương
        std::vector<int> qc;
        int maxDeg = a.degree() - db;
        if (maxDeg >= 0) qc.resize(maxDeg + 1, 0);

        while (!rem.isZero() && rem.degree() >= db) {
            int d = rem.degree() - db;   // bậc của đơn thức hiện tại
            qc[d] ^= 1;                   // thêm x^d vào thương
            // Trừ: rem -= x^d * b
            for (int i = 0; i <= db; i++)
                rem.coeff_[i + d] ^= b.coeff_[i];
            rem.standardize();
        }

        GF2Poly quot;
        if (!qc.empty()) quot = GF2Poly(qc);
        return std::pair<GF2Poly,GF2Poly>{ quot, rem };
    }

    // Toán tử / và %
    GF2Poly operator/(const GF2Poly& b) const { return divmod(*this, b).first;  }
    GF2Poly operator%(const GF2Poly& b) const { return divmod(*this, b).second; }
    GF2Poly& operator/=(const GF2Poly& b) { *this = *this / b; return *this; }
    GF2Poly& operator%=(const GF2Poly& b) { *this = *this % b; return *this; }

    // === GCD - UCLN ===
    static GF2Poly gcd(GF2Poly a, GF2Poly b) {
        while (!b.isZero()) {
            GF2Poly t = a % b;
            a = b;
            b = t;
        }
        return a;
    }

    // === ĐÁNH GIÁ  p(a),  a ∈ GF(2) ===

    // Đánh giá tại 0 và 1 — tiện cho kiểm tra
    Bit atZero() const { return Bit(coeff(0)); }  // p(0) = c_0
    Bit atOne()  const {                          // p(1) = XOR tất cả hệ số
        int s = 0;
        for (int c : coeff_) s ^= c;
        return Bit(s);
    }

    // === KIỂM TRA ĐA THỨC NGUYÊN TỐ ===
    
    // Không có ước nào bậc 1..(deg/2) ngoài 1 và chính nó
    bool isIrreducible() const {
        int d = degree();
        if (d <= 0) return false;
        if (d == 1) return true;  

        // Kiểm tra không chia hết cho mọi đa thức bậc 1..(d/2)
        for (int deg_b = 1; deg_b <= d / 2; deg_b++) {
            std::vector<int> bc(deg_b + 1, 0);
            bc[deg_b] = 1;

            // Duyệt tất cả đa thức bậc deg_b (hệ số cao nhất = 1)
            for (int mask = 0; mask < (1 << deg_b); mask++) {
                for (int i = 0; i < deg_b; i++)
                    bc[i] = (mask >> i) & 1;
                GF2Poly b(bc);
                if ((*this % b).isZero())
                    return false;
            }
        }
        return true;
    }

    // === DANG BIEU DIEN ===

    // Dạng chuỗi nhị phân MSB-first: "10011"
    std::string toBitString(int minLen = 0) const {
        int d = std::max(degree(), minLen - 1);
        if (d < 0) return "0";
        std::string s;
        for (int i = d; i >= 0; i--)
            s += (char)('0' + coeff(i));
        return s;
    }

    // Dạng đại số: "x^4 + x + 1"
    std::string toAlgebraicString() const {
        int d = degree();
        if (d < 0) return "0";
        std::string s;
        bool first = true;
        for (int i = d; i >= 0; i--) {
            if (!coeff_[i]) continue;
            if (!first) s += " + ";
            first = false;
            if      (i == 0) s += "1";
            else if (i == 1) s += "x";
            else             s += "x^" + std::to_string(i);
        }
        return s.empty() ? "0" : s;
    }

    // === I/O ===
    
    // Toán tử xuất chuẩn: in dạng chuỗi bit
    friend std::ostream& operator<<(std::ostream& os, const GF2Poly& p) {
        return os << "[" << p.toBitString() << "]";
    }

    // Toán tử nhập chuẩn: Nhập chuỗi nhị phân MSB-first
    friend std::istream& operator>>(std::istream& is, GF2Poly& p) {
        std::string s; is >> s;
        p = GF2Poly::fromBitString(s);
        return is;
    }

    const std::vector<int>& coeffs() const { return coeff_; }

private:
    // Hệ số đa thức: coeff_[i] = hệ số x^i
    std::vector<int> coeff_;

    // Loại bỏ các số 0 thừa ở bậc cao
    void standardize() {
        while (coeff_.size() > 1 && coeff_.back() == 0)
            coeff_.pop_back();
    }
};