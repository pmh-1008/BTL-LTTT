/*
    THUAT TOAN NHAN  (tao tu ma he thong tu h(x))

        Ky hieu:
            l      -- do dai tu ma
            k      -- so bit ban tin,   r = l-k bit kiem tra
            a_i    -- bit thu i cua ban tin  (i = 0..k-1)
                      a_i la he so x^i cua m(x)
            h_j    -- he so x^j cua h(x)    (j = 0..k-1)
                      (bo qua h_k = 1 vi no khong anh huong)
            c_i    -- bit thu i cua tu ma   (i = 0..l-1)
    -----------------------------------------------------------------
 */

#include "Bit.hpp"
#include "BinaryVector.hpp"
#include "GF2Poly.hpp"
#include "Helper.hpp"

#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int l, k;
    string hStr, mStr;
    cin >> l >> k >> hStr >> mStr;

    const int r = l - k;
    GF2Poly hx = GF2Poly::fromBitString(hStr);

    BinaryVector c = VectorHelper::computeCodeword_mul(l, k, hx, mStr);
    cout << "c = [" << c << "]\n";

    PrintHelper::printStateTable(l, k, hx, mStr);

    return 0;
}