/*
    Cung cấp các tiện ích:
        - GF2PolyHelper  : thao tác trên GF2Poly
        - VectorHelper   : thao tác trên BinaryVector
        - PrintHelper    : in bảng trạng thái, ma trận, đa thức
 */

#pragma once

#include "Bit.hpp"
#include "BinaryVector.hpp"
#include "GF2Poly.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <functional>
#include <climits>
#include <stdexcept>
#include <algorithm>


class GF2PolyHelper {
public:
    // === TRANSFORM ===

    // BinaryVector → GF2Poly
    static GF2Poly fromBitVector(const BinaryVector& bv) {
        std::vector<int> c(bv.size());
        for (int i = 0; i < bv.size(); i++) c[i] = bv[i].value();
        return GF2Poly(c);
    }

    // === THAO TÁC TRÊN VÒNG ĐA THỨC ===

    // Tính g(x) = (x^l + 1) / h(x) cho mã vòng C(l, k)
    static GF2Poly computeGeneratorPoly(int l, const GF2Poly& hx) {
        GF2Poly xl1 = GF2Poly::monomial(l) + GF2Poly({1});
        auto [g, rem] = GF2Poly::divmod(xl1, hx);
        if (!rem.isZero())
            throw std::domain_error(
                "computeGeneratorPoly: (x^" + std::to_string(l) +
                                        " + 1) khong chia het cho h(x)");
        if (g.degree() != l - hx.degree())
            throw std::logic_error(
                "computeGeneratorPoly: bac g(x) khong khop r = l - k");
        return g;
    }

    // Tính phần kiểm tra p(x) = x^r * m(x) mod g(x)
    static GF2Poly computeParityPoly(const GF2Poly& mx, const GF2Poly& gx, int r) {
        GF2Poly xrm = GF2Poly::monomial(r) * mx;
        auto [q, p] = GF2Poly::divmod(xrm, gx);
        return p;
    }

    // Tính từ mã hệ thống v(x) = x^r*m(x) + p(x)
    static GF2Poly computeCodeword_div(const GF2Poly& mx, const GF2Poly& gx, int r) {
        GF2Poly xrm = GF2Poly::monomial(r) * mx;
        GF2Poly p   = computeParityPoly(mx, gx, r);
        return xrm + p;
    }

    // Liệt kê tất cả các ước của p(x) trên GF(2) có bậc Deg
    static std::vector<GF2Poly> findDivisors(const GF2Poly& p, int Deg) {
        int d = p.degree();
        if (d < 0 || Deg < 0 || Deg > d) return {};

        std::vector<GF2Poly> divs;
        std::vector<int> bc(Deg + 1, 0);
        bc[Deg] = 1;
        for (int mask = 0; mask < (1 << Deg); mask++) {
            for (int i = 0; i < Deg; i++)
                bc[i] = (mask >> i) & 1;
            GF2Poly cand(bc);
            if ((p % cand).isZero())
                divs.push_back(cand);
        }
        return divs;
    }

    // Xác minh cặp (g, h) thoả g*h = x^l + 1
    static bool verifyGH(const GF2Poly& gx, const GF2Poly& hx, int l) {
        GF2Poly product = gx * hx;
        GF2Poly target  = GF2Poly::monomial(l) + GF2Poly({1});
        return product == target;
    }
};


class VectorHelper {
public:
    // === TRANSFORM ===
    // GF2Poly → BinaryVector (LSB-first, độ dài = max(deg+1, minlen))
    static BinaryVector fromPoly(const GF2Poly& p, int minlen = 0) {
        int len = std::max(p.degree() + 1, minlen);
        if (len <= 0) len = 1;
        BinaryVector bv(len);
        for (int i = 0; i < len; i++)
            bv[i] = Bit(p.coeff(i));
        return bv;
    }

    // === CÁC THAO TÁC VỚI VECTOR ===

    // Khoảng cách Hamming tối thiểu của tập mã
    static int minHammingDistance(const std::vector<BinaryVector>& codewords) {
        if (codewords.size() < 2) return 0;
        int dmin = INT_MAX;
        for (int i = 0; i < (int)codewords.size(); i++)
            for (int j = i+1; j < (int)codewords.size(); j++) {
                int d = codewords[i].hammingDistance(codewords[j]);
                if (d < dmin) dmin = d;
            }
        return dmin;
    }

    // Xây dựng ma trận sinh G từ g(x) (dạng hệ thống)
    static std::vector<BinaryVector> buildGeneratorMatrix(
            const GF2Poly& gx, int l, int k) {
        int r = l - k; (void)r;
        std::vector<BinaryVector> G;
        for (int i = 0; i < k; i++) {
            GF2Poly row = GF2Poly::monomial(i) * gx;
            G.push_back(fromPoly(row, l));
        }
        return G;
    }

    // Xây dựng ma trận kiểm tra chẵn lẻ H từ h(x)
    static std::vector<BinaryVector> buildParityCheckMatrix(
            const GF2Poly& hx, int l, int k) {
        int r = l - k; (void)r;
        std::vector<BinaryVector> H;
        int dh = hx.degree();
        std::vector<int> hrev(dh + 1);
        for (int i = 0; i <= dh; i++)
            hrev[i] = hx.coeff(dh - i);
        GF2Poly hrevPoly(hrev);
        for (int i = 0; i < r; i++) {
            GF2Poly row = GF2Poly::monomial(i) * hrevPoly;
            H.push_back(fromPoly(row, l));
        }
        return H;
    }

    static BinaryVector computeCodeword_mul(const int l, const int k, 
                                            const GF2Poly& hx, 
                                            const std::string mStr) {
        const int r = l - k;

        std::vector<Bit> c(l);

        for (int i = 0; i < k; i++) {
            c[r + i] = Bit(mStr[i] - '0');
        }

        //  Tinh cac bit kiem tra c[r-1]..c[0]
        for (int i = 1; i <= r; i++) {
            for (int j = 0; j < k; j++) {
                c[r - i] += Bit(hx.coeff(j) * (int)c[l-i-j]);
            }
        }
        return BinaryVector(c);
    }
};


class PrintHelper {
public:

    // === SEPARATOR / TITLE ===

    // In dòng kẻ phân cách
    static void printSeparator(std::ostream& os = std::cout,
                               int width = 60, char ch = '=') {
        os << std::string(width, ch) << "\n";
    }

    // === MATRIX ===

    // In ma trận nhị phân
    static void printMatrix(const std::string& name,
                            const std::vector<BinaryVector>& M,
                            std::ostream& os = std::cout) {
        os << name << " (" << M.size() << " x "
           << (M.empty() ? 0 : M[0].size()) << "):\n";
        for (int i = 0; i < (int)M.size(); i++) {
            os << "  [";
            for (int j = M[i].size()-1; j >= 0; j--) {
                os << M[i][j];
                if (j > 0) os << " ";
            }
            os << "]\n";
        }
    }

    // === STATE TABLE ===

    // In dòng tiêu đề cột bảng LFSR
    static void printTableHeader(int k, int colSz,
                                 std::ostream& os = std::cout) {
        os << std::setw(colSz)     << std::right << "CLK"
           << std::setw(colSz) << std::right << "INP" << "  |";

        for (int i = 0; i < k; ++i)
            os << std::setw(colSz) << ("A" + std::to_string(i));

        os << "  |" << std::setw(colSz) << "OUT";

        os << "\n";
        printSeparator(os, colSz * (k + 3) + 6, '-');
    }

    // In một dòng dữ liệu trong bảng LFSR
    static void printRow(int clk, int colSz, int input,
                         const std::vector<int>& A,
                         int output,
                         std::ostream& os = std::cout) {
        os << std::setw(colSz)     << std::right << clk;
        if (input >= 0)
            os << std::setw(colSz) << std::right << input << "  |";
        else
            os << std::setw(colSz) << std::right << '-' << "  |";

        for (auto i : A)
            os << std::setw(colSz) << i;

        os << "  |" << std::setw(colSz) << output;
        os << "\n";
    }

    // In toàn bộ bảng hoạt động mạch
    static void printStateTable(int l, int k, const GF2Poly hx,
                               const std::string mStr,
                               std::ostream& os = std::cout) {
        const int colSz = 5;
        printTableHeader(k, colSz, os);

        std::vector<int> hc(k + 1);
        std::vector<int> A(k, 0);

        for (int i = 0; i <= k; i++) hc[i] = hx.coeff(i);

        for (int clk = 1; clk <= k; clk++) {
            int input = mStr[k - clk] - '0';

            for (int j = k - 1; j > 0; --j)
                A[j] = A[j - 1];
            A[0] = input;

            printRow(clk, colSz, input, A, input, os);
        }
        printSeparator(os, colSz * (k + 3) + 6, '-');

        for (int clk = k + 1; clk <= l; ++clk) {
            Bit sum(0);
            for (int j = 0; j < k; j++) {
                if (hx.coeff(j) == 0) continue;
                sum += Bit(hx.coeff(j) * A[k - 1 - j]);
            }

            for (int j = k - 1; j > 0; --j)
                A[j] = A[j - 1];
            A[0] = (int)sum;

            printRow(clk, colSz, -1, A, A[0], os);   
        }
    }
};